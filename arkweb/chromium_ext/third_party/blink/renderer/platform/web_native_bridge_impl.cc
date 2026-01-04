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

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/web_native_bridge_impl.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <utility>

#include "arkweb/chromium_ext/cc/layer/video_layer_ext.h"
#include "base/check.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/location.h"
#include "base/memory/weak_ptr.h"
#include "base/task/bind_post_task.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/thread_pool.h"
#include "cc/layers/video_layer.h"
#include "media/base/native_pipeline_impl.h"
#include "media/base/video_frame.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "third_party/blink/public/common/thread_safe_browser_interface_broker_proxy.h"
#include "third_party/blink/public/platform/media/video_frame_compositor.h"
#include "third_party/blink/public/platform/web_native_client.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "content/child/child_process.h"
namespace blink {

namespace {

// Handles destruction of media::Renderer dependent components after the
// renderer has been destructed on the media thread.
// LCOV_EXCL_START
void DestructionHelper(
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> vfc_task_runner,
    std::unique_ptr<VideoFrameCompositor> compositor,
    std::unique_ptr<media::RendererFactorySelector> renderer_factory_selector,
    std::unique_ptr<WebSurfaceLayerBridge> bridge)
{
  // We release `bridge` after pipeline stop to ensure layout tests receive
  // painted same layer frames before test harness exit.
  main_task_runner->DeleteSoon(FROM_HERE, std::move(bridge));
  // Since the media::Renderer is gone we can now destroy the compositor and
  // renderer factory selector.
  vfc_task_runner->DeleteSoon(FROM_HERE, std::move(compositor));
  main_task_runner->DeleteSoon(FROM_HERE, std::move(renderer_factory_selector));
}

}  // namespace
// LCOV_EXCL_STOP

WebNativeBridgeImpl::WebNativeBridgeImpl(
    WebLocalFrame* frame,
    WebNativeClient* client,
    base::WeakPtr<blink::WebNativeDelegate> delegate,
    std::unique_ptr<media::RendererFactorySelector> renderer_factory_selector,
    std::unique_ptr<VideoFrameCompositor> compositor,
    scoped_refptr<base::SequencedTaskRunner> media_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner>
    video_frame_compositor_task_runner,
    CreateSurfaceLayerBridgeCB create_bridge_callback)
    : frame_(frame),
      main_task_runner_(frame->GetTaskRunner(TaskType::kMediaElementEvent)),
      media_task_runner_(std::move(media_task_runner)),
      client_(client),
      delegate_(delegate),
      vfc_task_runner_(std::move(video_frame_compositor_task_runner)),
      compositor_(std::move(compositor)),
      renderer_factory_selector_(std::move(renderer_factory_selector)),
      create_bridge_callback_(std::move(create_bridge_callback)) {
  DCHECK(renderer_factory_selector_);
  DCHECK(client_);
  DCHECK(delegate_);
  DCHECK(frame_);

  weak_this_ = weak_factory_.GetWeakPtr();

  // Using base::Unretained(this) is safe because the `pipeline` is owned by
  // `this` and the callback will always be made on the main task runner.
  // Not using BindToCurrentLoop() because CreateRenderer() is a sync call.
  auto native_pipeline = std::make_unique<media::NativePipelineImpl>(
      media_task_runner_, main_task_runner_,
      base::BindRepeating(&WebNativeBridgeImpl::CreateRenderer,
                          base::Unretained(this)));

  native_pipeline_controller_ =
      std::make_unique<media::NativePipelineController>(
          std::move(native_pipeline));
  delegate_id_ = delegate_->AddObserver(this);
}

// LCOV_EXCL_START
WebNativeBridgeImpl::~WebNativeBridgeImpl() {
  DVLOG(1) << __func__;
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  LOG(DEBUG) << "[NativeEmbed] ~WebNativeBridgeImpl.";

  // delegate_->PlayerGone(delegate_id_);
  if (delegate_) {
    delegate_->RemoveObserver(delegate_id_);
  }

  // The underlying Pipeline must be stopped before it is destroyed.
  //
  // Note: This destruction happens synchronously on the media thread and
  // `compositor_`, must outlive this process. They will be destructed
  // by the DestructionHelper below after trampolining through the media thread.
  native_pipeline_controller_->Stop();

  // Disconnect from the surface layer. We still preserve the `bridge_` until
  // after pipeline shutdown to ensure any pending frames are painted for tests.
  if (bridge_)
    bridge_->ClearObserver();

  // Destruct compositor resources in the proper order.
  client_->SetCcLayer(nullptr);
  if (bridge_) {
    LOG(DEBUG) << "[NativeEmbed] ResetLayerRectVisibilityChangeCallback.";
  }

  // Handle destruction of things that need to be destructed after the pipeline
  // completes stopping on the media thread.
  media_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&DestructionHelper, std::move(main_task_runner_),
                     std::move(vfc_task_runner_), std::move(compositor_),
                     std::move(renderer_factory_selector_), std::move(bridge_)));
}
// LCOV_EXCL_STOP

std::unique_ptr<media::Renderer> WebNativeBridgeImpl::CreateRenderer(
    absl::optional<media::RendererType> renderer_type) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());

  if (renderer_type) {
    renderer_factory_selector_->SetBaseRendererType(renderer_type.value());
  }

  return renderer_factory_selector_->GetCurrentFactory()->CreateRenderer(
      media_task_runner_, nullptr, nullptr, compositor_.get(),
      base::NullCallback(), client_->TargetColorSpace());
}

// LCOV_EXCL_START
void WebNativeBridgeImpl::StartPipeline() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  LOG(INFO) << "[NativeEmbed] WebNativeBridgeImpl::StartPipeline.";
  vfc_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&VideoFrameCompositor::SetOnNewProcessedFrameCallback,
                     base::Unretained(compositor_.get()),
                     base::BindPostTaskToCurrentDefault(base::BindOnce(
                         &WebNativeBridgeImpl::OnFirstFrame, weak_this_))));

  media::CreateTextureCB create_texture_cb = base::BindPostTaskToCurrentDefault(
      base::BindOnce(&WebNativeBridgeImpl::OnSurfaceCreated, weak_this_));
  media::DestroyTextureCB destroy_texture_cb =
      base::BindPostTaskToCurrentDefault(
          base::BindOnce(&WebNativeBridgeImpl::OnSurfaceDestroyed, weak_this_));

  native_pipeline_controller_->Start(this, std::move(create_texture_cb),
                                     std::move(destroy_texture_cb));

  vfc_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&content::ChildProcess::ReportCompositorKeyThread,
          base::Unretained(content::ChildProcess::current()), true));
}

void WebNativeBridgeImpl::OnFirstFrame(base::TimeTicks frame_time,
                                       bool is_frame_readable) {
  if (client_) {
    client_->Repaint();
  }
}

void WebNativeBridgeImpl::OnSurfaceCreated(media::RectChangedCB rect_changed_cb,
                                           int native_embed_id) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  LOG(INFO) << "[NativeEmbed] WebNativeBridgeImpl::OnSurfaceCreated";

  client_->OnCreateNativeSurface(native_embed_id, rect_changed_cb);
}

void WebNativeBridgeImpl::OnSurfaceDestroyed() {
  LOG(INFO) << "[NativeEmbed] WebNativeBridgeImpl::OnSurfaceDestroyed";
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  client_->OnDestroyNativeSurface();
}

void WebNativeBridgeImpl::OnLayerRectChange(const gfx::Rect& rect) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  LOG(DEBUG) << "[NativeEmbed] WebNativeBridgeImpl::OnLayerRectChange";

  client_->OnLayerRectChange(rect);

  layer_rect_ = rect;
}

void WebNativeBridgeImpl::OnLayerRectVisibilityChange(bool visibility) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  LOG(DEBUG) << "[NativeEmbed] OnLayerRectVisibilityChange: " << visibility;

  client_->OnLayerRectVisibilityChange(visibility);
}

void WebNativeBridgeImpl::CleanupVisibilityForRemovedLayer(bool visibility) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  LOG(DEBUG) << "[NativeEmbed] CleanupVisibilityForRemovedLayer: " << visibility;

  client_->CleanupVisibilityForRemovedLayer(visibility);
}

gfx::Size WebNativeBridgeImpl::NaturalSize() const {
  return layer_rect_.size();
}

void WebNativeBridgeImpl::OnSetLayer() {
  LOG(INFO) << "[NativeEmbed] WebNativeBridgeImpl::OnSetLayer";
  DVLOG(1) << __func__;
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  DCHECK(!bridge_);

  if (!surface_layer_for_same_layer_) {
    ActivateSurfaceLayerForSameLayer();
  }
}

void WebNativeBridgeImpl::ActivateSurfaceLayerForSameLayer() {
  DCHECK(!bridge_);

  // use surface layer mode
  surface_layer_for_same_layer_ = true;
  bridge_ = std::move(create_bridge_callback_).Run(this, compositor_->GetUpdateSubmissionStateCallback());
  
  media::RectChangedCB rect_change_cb = base::BindRepeating(
      &WebNativeBridgeImpl::OnLayerRectChange, weak_this_);
  media::RectVisibilityChangedCB rect_visibility_change_cb =
      base::BindRepeating(&WebNativeBridgeImpl::OnLayerRectVisibilityChange,
                          weak_this_);
  media::LayerRemovedVisibilityChangedCB layer_removed_visibility_change_cb =
      base::BindRepeating(&WebNativeBridgeImpl::CleanupVisibilityForRemovedLayer,
                          weak_this_);
  if (client_) {
    bridge_->SetStretchContentToFillBounds(client_->GetStretchContentToFillBounds());
  }
  surface_layer_ = bridge_->CreateSurfaceLayer(std::move(rect_change_cb), std::move(rect_visibility_change_cb),
      std::move(layer_removed_visibility_change_cb));
  client_->SetCcLayer(surface_layer_.get());
  vfc_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(
          &VideoFrameCompositor::EnableSubmission,
          compositor_->GetWeakPtr(), bridge_->GetSurfaceId(),
          media::kNoTransformation,
          false));
  bridge_->SetContentsOpaque(false);
}

void WebNativeBridgeImpl::OnWebLayerUpdated() {}

void WebNativeBridgeImpl::RegisterContentsLayer(cc::Layer* layer)
{
  DCHECK(bridge_);
  bridge_->SetContentsOpaque(false);
  client_->SetCcLayer(layer);
}

void WebNativeBridgeImpl::UnregisterContentsLayer(cc::Layer* layer)
{
  // `client_` will unregister its cc::Layer if given a nullptr.
  client_->SetCcLayer(nullptr);
}

void WebNativeBridgeImpl::OnSurfaceIdUpdated(viz::SurfaceId surface_id) {
}

void WebNativeBridgeImpl::SetStretchContentToFillBounds(bool stretch_content_to_fill_bounds) {
  if (bridge_) {
    bridge_->SetStretchContentToFillBounds(stretch_content_to_fill_bounds);
  }
}
void WebNativeBridgeImpl::UpdateDeviceScaleFactor(float device_scale_factor) {
  if (compositor_) {
    compositor_->UpdateDeviceScaleFactor(device_scale_factor);
  }
}
// LCOV_EXCL_STOP
}  // namespace blink
