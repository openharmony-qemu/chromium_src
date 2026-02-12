/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "third_party/blink/renderer/core/loader/native_loader.h"

#include <algorithm>
#include <limits>

#include "arkweb/chromium_ext/media/mojo/mojom/native_bridge.mojom.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/html/html_plugin_element_utils.h"
#include "base/time/time.h"
#include "cc/layers/layer.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/public/platform/task_type.h"
#include "third_party/blink/renderer/core/core_initializer.h"
#include "third_party/blink/renderer/core/core_probes_inl.h"
#include "third_party/blink/renderer/core/css/style_engine.h"
#include "third_party/blink/renderer/core/dom/shadow_root.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_client.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/frame/page_scale_constraints_set.h"
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
#include "third_party/blink/renderer/core/html/html_plugin_element.h"
#include "third_party/blink/renderer/core/layout/layout_native.h"
#include "third_party/blink/renderer/core/layout/layout_view.h"
#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/platform/bindings/exception_messages.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_fetcher.h"
#include "third_party/blink/renderer/platform/web_native_bridge.h"
#include "third_party/blink/renderer/platform/wtf/cross_thread_functional.h"
#include "third_party/blink/renderer/platform/wtf/functional.h"

namespace blink {

namespace {

// LCOV_EXCL_START
float PageConstraintInitalScale(const Document& document) {
  float scale = 1.0;
  if (auto* page = document.GetPage()) {
    scale = page->GetPageScaleConstraintsSet().FinalConstraints().initial_scale;
  }
  return scale;
}

gfx::Rect BoundsToViewport(const gfx::Rect& bounding_rect,
                           const Document& document) {
  return gfx::Rect(bounding_rect.origin(),
                   gfx::ScaleToCeiledSize(bounding_rect.size(),
                                          PageConstraintInitalScale(document)));
}

gfx::Point PositionToViewport(const gfx::Point& bounding_rect_position,
                              const Document& document) {
  return gfx::ScaleToCeiledPoint(bounding_rect_position,
                                 PageConstraintInitalScale(document));
}

}  // anonymous namespace

NativeLoader::NativeLoader(HTMLPlugInElement* plugin_element)
    : ExecutionContextLifecycleStateObserver(GetExecutionContext()),
      cc_layer_(nullptr),
      plugin_element_(plugin_element) {
  LOG(INFO) << "[NativeEmbed] NativeLoader::NativeLoader element "
            << plugin_element_->ToString();
  ResetMojoState();

  CurrentFrame()->GetFrameScheduler()->RegisterStickyFeature(
      SchedulingPolicy::Feature::kEnableCacheNativeEmbed,
      {SchedulingPolicy::DisableBackForwardCache()});
}

NativeLoader::~NativeLoader() {
  LOG(INFO) << "[NativeEmbed] NativeLoader::~NativeLoader ";
}

void NativeLoader::Dispose() {
  LOG(INFO) << "[NativeEmbed] NativeLoader::Dispose ";
  ClearNativeResource();
}

void NativeLoader::AttachToNewFrame() {
  // Reset mojo state that is coupled to |old_document|'s execution context.
  // NOTE: |native_bridge_host_remote_| is also coupled to |old_document|'s
  // frame.
  ResetMojoState();
  ScheduleLoadResource();
}

void NativeLoader::ResetMojoState() {
  native_bridge_host_remote_ = MakeGarbageCollected<DisallowNewWrapper<
      HeapMojoAssociatedRemote<media::mojom::blink::NativeBridgeHost>>>(
      GetExecutionContext());
  if (native_bridge_observer_remote_set_) {
    native_bridge_observer_remote_set_->Value().Clear();
  }
  native_bridge_observer_remote_set_ = MakeGarbageCollected<DisallowNewWrapper<
      HeapMojoAssociatedRemoteSet<media::mojom::blink::NativeBridgeObserver>>>(
      GetExecutionContext());
}

String NativeLoader::GetTypeAttribute() const {
  return plugin_element_->TypeAttribute();
}

String NativeLoader::GetSrcAttribute() const {
  return plugin_element_->SrcAttribute();
}

String NativeLoader::GetIdAttribute() const {
  return plugin_element_->IdAttribute();
}

String NativeLoader::GetTagName() const {
  return plugin_element_->tagName();
}

ParamMap NativeLoader::GetParamList() const {
  return plugin_element_->ParamList();
}

LocalFrame* NativeLoader::CurrentFrame() {
  return plugin_element_->GetDocument().GetFrame();
}

void NativeLoader::ScheduleLoadResource() {
  LOG(INFO) << "NativeEmbed NativeLoader::ScheduleLoadResource";
  LoadResource(CurrentFrame());
}
// LCOV_EXCL_STOP

void NativeLoader::LoadResource(LocalFrame* frame) {
  LOG(INFO) << "NativeEmbed NativeLoader::LoadResource";
  DCHECK(frame);
  web_native_bridge_ = frame->Client()->CreateWebNativeBridge(*this, this);
  if (!web_native_bridge_) {
    return;
  }

  GetNativeBridgeHostRemote().OnNativeBridgeAdded(
      AddNativeBridgeObserverAndPassReceiver(),
      web_native_bridge_->GetDelegateId());

  web_native_bridge_->StartPipeline();
}

gfx::Rect NativeLoader::TransformRect(gfx::Rect rect) {
  LayoutObject* layoutObject = plugin_element_->GetLayoutObject();
  if (layoutObject) {
    gfx::Transform transform = layoutObject->LocalToAbsoluteTransform();
    auto trandfromRect = transform.MapRect(gfx::RectF(rect));
    rect.set_width(std::round(trandfromRect.size().width()));
    rect.set_height(std::round(trandfromRect.size().height()));
    LOG(INFO) << "NativeEmbed NativeLoader::TransformRect: " << rect.ToString();
  }
  return rect;
}

void NativeLoader::OnCreateNativeSurface(int native_embed_id,
                                         RectChangeCB rect_changed_cb) {
  LOG(INFO) << "[NativeEmbed] NativeLoader::OnCreateNativeSurface";
  if (!native_bridge_observer_remote_set_ || !cc_layer_) {
    return;
  }

  if (!cc_layer_update_ || bounding_rect_.IsEmpty()) {
    plugin_element_->GetDocument().UpdateStyleAndLayoutForNode(
        plugin_element_, DocumentUpdateReason::kPlugin);
    bounding_rect_ = plugin_element_->PixelSnappedBoundingBox();
    if (auto* layout_object = plugin_element_->GetLayoutObject()) {
      if (const auto& replaced = To<LayoutReplaced>(layout_object)) {
        bounding_rect_.set_size(
            ToPixelSnappedRect(replaced->ReplacedContentRect()).size());
      }
    }
  }
  LOG(INFO) << "NativeEmbed NativeLoader::OnCreateNativeSurface:"
            << bounding_rect_.ToString();

  native_embed_id_ = native_embed_id;
  bounding_rect_changed_cb_ = rect_changed_cb;
  cc_layer_->layer_utils()->SetNativeEmbedId(native_embed_id_);
  cc_layer_->layer_utils()->SetMayContainNative(true);
  cc_layer_->SetNeedsCommit();

  auto embed_info = media::mojom::blink::NativeEmbedInfo::New();
  auto bounds_to_viewport =
      BoundsToViewport(bounding_rect_, plugin_element_->GetDocument());
  if (!cc_layer_update_) {
    // Create phase requires change the origin of the bounding_rect with page
    // initial_scale.
    bounds_to_viewport.set_origin(PositionToViewport(
        bounding_rect_.origin(), plugin_element_->GetDocument()));
  }
  // We will use the position relative to visual viewport.
  bounding_rect_.set_origin(bounds_to_viewport.origin());

  embed_info->rect = TransformRect(bounds_to_viewport);
  if (!bounding_rect_changed_cb_.is_null()) {
    bounding_rect_changed_cb_.Run(bounds_to_viewport);
  }

  embed_info->embed_id = native_embed_id_;
  embed_info->type = GetTypeAttribute().IsNull() ? "" : GetTypeAttribute();
  embed_info->element_id = GetIdAttribute().IsNull() ? "" : GetIdAttribute();
  embed_info->source = GetSrcAttribute().IsNull() ? "" : GetSrcAttribute();
  embed_info->tag = GetTagName().IsNull() ? "" : GetTagName();
  if (!GetParamList().empty()) {
    embed_info->params = GetParamList();
  }

  for (auto& observer : native_bridge_observer_remote_set_->Value()) {
    // TODO: We actually only have one observer now so just using std::move
    // here.
    observer->OnCreateNativeSurface(embed_info.Clone());
  }

  if (first_update_visibility_) {
    NotifyVisibilityChange(visibility_);
  }

  if (!pending_param_changes_.empty()) {
    ProcessPendingParamChanges();
  }
}

// LCOV_EXCL_START
void NativeLoader::OnLayerRectVisibilityChange(bool visibility) {
  NotifyVisibilityChange(visibility);
}
// LCOV_EXCL_STOP

void NativeLoader::OnLayerRectChange(const gfx::Rect& rect) {
  if (bounding_rect_.ApproximatelyEqual(rect, 1) ||
      !native_bridge_observer_remote_set_) {
    return;
  }

  if (bounding_rect_.size() != rect.size()) {
    bounding_rect_ = rect;
    if (!bounding_rect_changed_cb_.is_null()) {
      bounding_rect_changed_cb_.Run(
          BoundsToViewport(bounding_rect_, plugin_element_->GetDocument()));
    }
  } else {
    bounding_rect_.set_origin(rect.origin());
  }
  cc_layer_update_ = true;
  LOG(INFO) << "NativeEmbed NativeLoader::OnLayerRectChange:"
            << bounding_rect_.ToString();
  auto bounds_to_viewport = BoundsToViewport(bounding_rect_, plugin_element_->GetDocument());
  for (auto& observer : native_bridge_observer_remote_set_->Value()) {
    observer->OnEmbedRectChange(TransformRect(bounds_to_viewport));
  }
  auto* frame = CurrentFrame();
  if (!frame) {
    return;
  }
  float factor = frame->Client()->GetDeviceScaleFactor(*this);
  LOG(INFO) << "NativeEmbed NativeLoader::GetDeviceScaleFactor:" << factor;
  if (web_native_bridge_) {
    web_native_bridge_->UpdateDeviceScaleFactor(factor);
  }
}

// LCOV_EXCL_START
void NativeLoader::OnDestroyNativeSurface() {
  LOG(INFO) << "[NativeEmbed] NativeLoader::OnDestroyNativeSurface";
  bounding_rect_changed_cb_.Reset();
  if (!native_bridge_observer_remote_set_) {
    return;
  }

  for (auto& observer : native_bridge_observer_remote_set_->Value()) {
    observer->OnDestroyNativeSurface();
  }
}

void NativeLoader::Repaint() {
  LOG(INFO) << "NativeLoader::Repaint";
  if (cc_layer_) {
    cc_layer_->SetNeedsDisplay();
  }

  if (auto* layout_object = plugin_element_->GetLayoutObject()) {
    layout_object->SetShouldDoFullPaintInvalidation();
  }

  auto combined_callback = CrossThreadBindOnce(
      &NativeLoader::ReportFirstPaintTime, WrapCrossThreadWeakPersistent(this));

  auto* frame = CurrentFrame();
  if (!frame) {
    return;
  }
  frame->GetPage()->GetChromeClient().NotifyPresentationTime(
      *frame, std::move(combined_callback));
}
// LCOV_EXCL_STOP

void NativeLoader::SetCcLayer(cc::Layer* cc_layer) {
  LOG(INFO) << "[NativeEmbed] NativeLoader::SetCcLayer";
  if (cc_layer == cc_layer_) {
    return;
  }

  auto* plugin_element_client =
      static_cast<DisplayItemClient*>(plugin_element_->GetLayoutObject());
  if (plugin_element_client) {
    plugin_element_client->Invalidate(PaintInvalidationReason::kJustCreated);
  }

  plugin_element_->SetNeedsCompositingUpdate();
  cc_layer_ = cc_layer;
  if (cc_layer_) {
    LOG(DEBUG) << "[NativeEmbed] set native flag SetNativeType:"
               << GetTypeAttribute();
    cc_layer_->layer_utils()->SetMayContainNative(true);
    cc_layer_->SetNeedsPushProperties();
    bool is_infinity_overlay = plugin_element_->Utils()->IsOverlayInfinity();
    bool is_standard_overlay = plugin_element_->Utils()->IsOverlay();
    if (is_infinity_overlay) {
      SetNativeEmbedOverlayInfinity(is_infinity_overlay);
    } else if (is_standard_overlay && !is_infinity_overlay) {
      SetNativeEmbedOverlay(is_standard_overlay);
    }
    // cc_layer_->SetIsNativeVideo(GetTypeAttribute() == "native/video");
  }
}

// LCOV_EXCL_START
void NativeLoader::ClearNativeResource() {
  LOG(INFO) << "NativeEmbed NativeLoader::ClearNativeResource";

  if (web_native_bridge_) {
    web_native_bridge_.reset();
    native_bridge_observer_remote_set_->Value().Clear();
  }

  if (auto* layout_object = plugin_element_->GetLayoutObject()) {
    layout_object->SetShouldDoFullPaintInvalidation();
  }
}

void NativeLoader::ContextDestroyed() {
  DVLOG(3) << "contextDestroyed(" << static_cast<void*>(this) << ")";
  LOG(INFO) << "[NativeEmbed] NativeLoader::ContextDestroyed";
  // Clear everything in the Media Element
  ClearNativeResource();
}

cc::Layer* NativeLoader::CcLayer() const {
  return cc_layer_;
}

void NativeLoader::Trace(Visitor* visitor) const {
  visitor->Trace(plugin_element_);
  visitor->Trace(native_bridge_host_remote_);
  visitor->Trace(native_bridge_observer_remote_set_);
  ExecutionContextLifecycleStateObserver::Trace(visitor);
}

media::mojom::blink::NativeBridgeHost&
NativeLoader::GetNativeBridgeHostRemote() {
  // It is an error to call this before having access to the document's frame.
  auto* frame = CurrentFrame();
  DCHECK(frame);
  if (!native_bridge_host_remote_->Value().is_bound()) {
    frame->GetRemoteNavigationAssociatedInterfaces()->GetInterface(
        native_bridge_host_remote_->Value().BindNewEndpointAndPassReceiver(
            plugin_element_->GetDocument().GetTaskRunner(
                TaskType::kInternalMedia)));
  }
  return *native_bridge_host_remote_->Value().get();
}

mojo::PendingAssociatedReceiver<media::mojom::blink::NativeBridgeObserver>
NativeLoader::AddNativeBridgeObserverAndPassReceiver() {
  mojo::PendingAssociatedRemote<media::mojom::blink::NativeBridgeObserver>
      observer;
  auto observer_receiver = observer.InitWithNewEndpointAndPassReceiver();
  native_bridge_observer_remote_set_->Value().Add(
      std::move(observer),
      plugin_element_->GetDocument().GetTaskRunner(TaskType::kInternalMedia));
  return observer_receiver;
}
// LCOV_EXCL_STOP

void NativeLoader::ReportFirstPaintTime(
    const viz::FrameTimingDetails& frame_timing_details) {
  base::TimeTicks first_paint_time =
      frame_timing_details.presentation_feedback.timestamp;
  LOG(INFO) << "ReportFirstPaintTime(" << first_paint_time << ")";
  for (auto& observer : native_bridge_observer_remote_set_->Value()) {
    observer->OnNativeEmbedFirstFramePaint(
        native_embed_id_, GetIdAttribute().IsNull() ? "" : GetIdAttribute());
  }
}

// LCOV_EXCL_START
void NativeLoader::CleanupVisibilityForRemovedLayer(bool visibility) {
  if (!plugin_element_->Utils()->IsCssDisplayChangeEnabled()) {
    return;
  }
  LOG(INFO) << "[NativeEmbed] CssDisplayVisibility: " << visibility;
  NotifyVisibilityChange(visibility);
}
// LCOV_EXCL_STOP

void NativeLoader::NotifyVisibilityChange(bool visibility) {
  visibility_ = visibility;
  first_update_visibility_ = true;
  if (native_embed_id_ != -1) {
    for (auto& observer : native_bridge_observer_remote_set_->Value()) {
      observer->OnLayerRectVisibilityChange(visibility_, native_embed_id_);
    }
  }
}

// LCOV_EXCL_START
void NativeLoader::SetNativeEmbedOverlayInfinity(bool native_embed_overlay_infinity) {
  LOG(INFO) << "[NativeEmbed] NativeLoader::SetNativeEmbedOverlayInfinity: "
            << native_embed_overlay_infinity;
  cc_layer_->SetNativeEmbedOverlayInfinity(native_embed_overlay_infinity);
  if (native_embed_overlay_infinity) {
    cc_layer_->layer_utils()->SetShouldInterceptTouchEvent(true);
  }
}

void NativeLoader::SetNativeEmbedOverlay(bool native_embed_overlay) {
  LOG(INFO) << "[NativeEmbed] NativeLoader::SetNativeEmbedOverlay: "
            << native_embed_overlay;
  cc_layer_->SetNativeEmbedOverlay(native_embed_overlay);
  if (native_embed_overlay) {
    cc_layer_->layer_utils()->SetShouldInterceptTouchEvent(true);
  }
}

void NativeLoader::SetStretchContentToFillBounds(bool stretch_content_to_fill_bounds) {
  LOG(INFO) << "[NativeEmbed] NativeLoader: SetstretchContentToFillBounds"
            << stretch_content_to_fill_bounds;
  stretch_content_to_fill_bounds_ = stretch_content_to_fill_bounds;
  if (web_native_bridge_) {
    web_native_bridge_->SetStretchContentToFillBounds(
        stretch_content_to_fill_bounds_);
  }
}

bool NativeLoader::GetStretchContentToFillBounds() {
  return stretch_content_to_fill_bounds_;
}

void NativeLoader::ProcessParamChanges(const Vector<ParamChangeInfo>& changes) {
  Vector<media::mojom::blink::NativeEmbedParamItemPtr> mojo_items;
  for (const auto& info : changes) {
    auto item = media::mojom::blink::NativeEmbedParamItem::New();
    switch (info.status) {
      case ParamChangeInfo::Status::kAdd:
        item->status = media::mojom::blink::NativeEmbedParamStatus::kAdd;
        break;
      case ParamChangeInfo::Status::kUpdate:
        item->status = media::mojom::blink::NativeEmbedParamStatus::kUpdate;
        break;
      case ParamChangeInfo::Status::kDelete:
        item->status = media::mojom::blink::NativeEmbedParamStatus::kDelete;
        break;
    }
    item->id = info.id.IsNull() ? "" : info.id.Ascii().data();
    item->name = info.name.IsNull() ? "" : info.name.Ascii().data();
    item->value = info.value.IsNull() ? "" : info.value.Ascii().data();
    pending_param_changes_.push_back(std::move(item));
  }

  if (param_update_task_pending_) {
    return;
  }

  if (plugin_element_) {
    param_update_task_pending_ = true;
    plugin_element_->GetDocument().GetTaskRunner(TaskType::kInternalMedia)->PostTask(
        FROM_HERE, base::BindOnce(&NativeLoader::ProcessPendingParamChanges, weak_ptr_factory_.GetWeakPtr()));
  } else {
    param_update_task_pending_ = false;
    pending_param_changes_.clear();
  }
}

constexpr size_t kMaxParamsPerIPC = 500;

void NativeLoader::ProcessPendingParamChanges() {
  LOG(INFO) << "[NativeEmbed] NativeLoader::ProcessPendingParamChanges: "
            << pending_param_changes_.size();
  param_update_task_pending_ = false;
  if (native_embed_id_ == -1) {
    return;
  }

  if (pending_param_changes_.empty() || !current_plugin_element()) {
    return;
  }

  auto all_changes = std::move(pending_param_changes_);
  pending_param_changes_.clear();
  for (size_t i = 0; i < all_changes.size(); i += kMaxParamsPerIPC) {
    auto param_info = media::mojom::blink::NativeEmbedParamChangeInfo::New();
    param_info->embed_id = native_embed_id_;
    param_info->object_attribute_id = GetIdAttribute().IsNull() ? "" : GetIdAttribute();
    size_t end = std::min(i + kMaxParamsPerIPC, static_cast<size_t>(all_changes.size()));
    for (size_t j = i; j < end; ++j) {
      param_info->param_items.push_back(std::move(all_changes[j]));
    }
    for (auto& observer : native_bridge_observer_remote_set_->Value()) {
      observer->OnEmbedObjectParamChange(param_info.Clone());
    }
  }

  
}
// LCOV_EXCL_STOP
}  // namespace blink
                     