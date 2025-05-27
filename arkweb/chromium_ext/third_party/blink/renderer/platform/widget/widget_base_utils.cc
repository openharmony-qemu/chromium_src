/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/widget/widget_base_utils.h"
#include "base/logging.h"
#include "gpu/ipc/client/gpu_channel_host.h"
#include "ui/gfx/geometry/dip_util.h"
#include "ui/gfx/presentation_feedback.h"
#include "third_party/blink/renderer/platform/widget/input/widget_input_handler_manager.h"
#include "third_party/blink/renderer/platform/widget/widget_base_client.h"
#include "third_party/blink/renderer/platform/widget/frame_widget.h"
#include "third_party/blink/renderer/platform/widget/compositing/blink_categorized_worker_pool_delegate.h"
#include "ui/base/ime/mojom/text_input_state.mojom-blink.h"
#include "ui/base/mojom/menu_source_type.mojom-blink-forward.h"
#include "cc/animation/animation_timeline.h"
#include "cc/mojo_embedder/async_layer_tree_frame_sink.h"
#include "cc/paint/element_id.h"
#include "cc/trees/browser_controls_params.h"
#include "cc/trees/paint_holding_reason.h"
#include "cc/animation/animation_host.h"
#include "cc/animation/animation_id_provider.h"
#include "cc/raster/categorized_worker_pool.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_settings.h"
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
#include "arkweb/chromium_ext/base/process/process_handle_posix_ex.h"
#include "base/process/process_handle.h"
#include "base/task/post_job.h"
#include "base/task/thread_pool/job_task_source.h"
#include "base/task/thread_pool/thread_pool_impl.h"
#include "base/task/thread_pool/thread_group_impl.h"
#include "base/task/thread_pool/worker_thread.h"
#include "content/child/child_thread_impl.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#include "base/ohos/sys_info_utils_ext.h"
#endif

namespace blink {

WidgetBaseUtils::WidgetBaseUtils(WidgetBase* widget_base) : widget_base_(widget_base) {}

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
static void GetThreadIdsAndReport(std::vector<base::internal::WorkerThread*>& workers, 
                                       bool is_created) {
  std::vector<int32_t> thread_ids;
  std::vector<base::internal::WorkerThread*> remain_workers;
  for (auto& worker : workers) {
    if (worker) {
      auto tid = worker->GetRealTid();
      if (tid) {
        thread_ids.push_back(tid);
      } else {
        remain_workers.push_back(worker);
      }
    }
  }
  workers.clear();
  if (remain_workers.size()) {
    workers = std::move(remain_workers);
  }
 
  auto* thread = content::ChildThreadImpl::current();
  if (thread) {
    auto host = thread->child_process_host();
    auto status = is_created ? OHOS::NWeb::ResSchedStatusAdapter::THREAD_CREATED :
                               OHOS::NWeb::ResSchedStatusAdapter::THREAD_DESTROYED;
    host->ReportKeyThreadIds(static_cast<int32_t>(status),
        base::GetCurrentRealPid(), thread_ids,
        static_cast<int32_t>(OHOS::NWeb::ResSchedRoleAdapter::IMAGE_DECODE));
  }
}

void WidgetBaseUtils::ReportForegroundThreadPool() {
  if (!widget_base_->is_worker_pool_initial_) {
    return;
  }
  cc::CategorizedWorkerPool* worker_pool = cc::CategorizedWorkerPool::GetOrCreate(
      &BlinkCategorizedWorkerPoolDelegate::Get());
  cc::CategorizedWorkerPoolJob* worker_pool_job =
    static_cast<cc::CategorizedWorkerPoolJob*>(worker_pool);
  base::JobHandle* foreground_job_handle = worker_pool_job->GetForegroundJobHandle();
  if (!foreground_job_handle) {
    return;
  }
  base::internal::JobTaskSource* task_source =
    foreground_job_handle->GetTaskSource();
  if (!task_source) {
    return;
  }
  base::internal::PooledTaskRunnerDelegate* pool_delegate = task_source->delegate();
  base::internal::ThreadPoolImpl* thread_pool =
    static_cast<base::internal::ThreadPoolImpl*>(pool_delegate);
  if (!thread_pool) {
    return;
  }
  base::internal::ThreadGroupImpl* foreground_thread_group =
    static_cast<base::internal::ThreadGroupImpl*>(thread_pool->GetForegroundThreadGroup());
  if (foreground_thread_group) {
    std::vector<base::internal::WorkerThread*>& create_workers =
      foreground_thread_group->ReportCreateWorkers();
    if (create_workers.size()) {
      GetThreadIdsAndReport(create_workers, true);
    }
    std::vector<base::internal::WorkerThread*>& destroy_workers =
      foreground_thread_group->ReportDestroyWorkers();
    if (destroy_workers.size()) {
      GetThreadIdsAndReport(destroy_workers, false);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void WidgetBaseUtils::SetZoomLevel(float magnify_delta, const gfx::Point& anchor) {
  if (!widget_base_->widget_input_handler_manager_) {
    return;
  }
  widget_base_->widget_input_handler_manager_->manager_utils()->AsyncSetZoomLevel(magnify_delta, anchor);
}
void WidgetBaseUtils::SetOverscrollMode(int mode) {
  if (!widget_base_->widget_input_handler_manager_) {
    return;
  }
  widget_base_->widget_input_handler_manager_->manager_utils()->SetOverscrollMode(mode);
}

void WidgetBaseUtils::SetRequestKeyboardReason(int32_t requestKeyboardReason) {
  requestKeyboardReason_ = requestKeyboardReason;
}

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
gfx::Vector2dF WidgetBaseUtils::GetOverScrollOffset() {
  gfx::Vector2dF overscroll_offset;
  overscroll_offset.set_x(0.0f);
  overscroll_offset.set_y(0.0f);
  if (!widget_base_->widget_input_handler_manager_) {
    return overscroll_offset;
  }
  return widget_base_->widget_input_handler_manager_->manager_utils()->GetOverScrollOffset();
}
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void WidgetBaseUtils::TouchHitTest(const WebPointerEvent& event, size_t i) {
  FrameWidget* frame_widget = widget_base_->client_->FrameWidget();
  if (!frame_widget) {
    return;
  }
  frame_widget->TouchHitTest(event, i);
}
void WidgetBaseUtils::NativeHitTestResult(bool isNative, size_t fingerId, int layerId) {
  if (widget_base_->widget_input_handler_manager_) {
    widget_base_->widget_input_handler_manager_->manager_utils()
      ->NativeHitTestResult(isNative, fingerId, layerId);
  }
}
void WidgetBaseUtils::DidNativeEmbedEvent(blink::WebInputEvent::Type type,
                                          std::string embedId,
                                          int32_t id,
                                          float x,
                                          float y) {
  mojom::blink::NativeTouchType nativeType;
  switch (type) {
    case WebInputEvent::Type::kTouchStart:
      nativeType = mojom::blink::NativeTouchType::DOWN;
      break;
    case WebInputEvent::Type::kTouchMove:
      nativeType = mojom::blink::NativeTouchType::MOVE;
      break;
    case WebInputEvent::Type::kTouchEnd:
      nativeType = mojom::blink::NativeTouchType::UP;
      break;
    default:
      nativeType = mojom::blink::NativeTouchType::CANCEL;
  }
  LOG(DEBUG) << "[NativeEmbed] DidNativeEmbedEvent type is : " << nativeType
             << " x: " << x << " y: " << y;
  widget_base_->widget_host_->DidNativeEmbedEvent(mojom::blink::NativeEmbedTouchEvent::New(
      static_cast<String>(embedId), id, x, y, x, y, nativeType, x, y));
}

void WidgetBaseUtils::MouseHitTest(const WebMouseEvent& event) {
  FrameWidget* frame_widget = widget_base_->client_->FrameWidget();
  if (!frame_widget) {
    return;
  }
  frame_widget->MouseHitTest(event);
}

void WidgetBaseUtils::NativeMouseHitTestResult(bool isNative, int layerId) {
  if (widget_base_->widget_input_handler_manager_) {
    widget_base_->widget_input_handler_manager_->manager_utils()
      ->NativeMouseHitTestResult(isNative, layerId);
  }
}

void WidgetBaseUtils::DidNativeEmbedMouseEvent(
    blink::WebInputEvent::Type type,
    blink::WebInputEvent::Modifiers modifiers,
    std::string embedId,
    bool isHitNativeArea,
    float x,
    float y) {
  mojom::blink::NativeMouseType nativeMouseType;
  switch (type) {
    case WebInputEvent::Type::kMouseDown:
      nativeMouseType = mojom::blink::NativeMouseType::PRESS;
      break;
    case WebInputEvent::Type::kMouseUp:
      nativeMouseType = mojom::blink::NativeMouseType::RELEASE;
      break;
    case WebInputEvent::Type::kMouseMove:
      nativeMouseType = mojom::blink::NativeMouseType::MOVE;
      break;
    default:
      nativeMouseType = mojom::blink::NativeMouseType::CANCEL;
  }
  mojom::blink::NativeMouseButton nativeMouseButton;
  if (modifiers & blink::WebInputEvent::Modifiers::kLeftButtonDown) {
    nativeMouseButton = mojom::blink::NativeMouseButton::LEFT_BUTTON;
  } else if (modifiers & blink::WebInputEvent::Modifiers::kRightButtonDown) {
    nativeMouseButton = mojom::blink::NativeMouseButton::RIGHT_BUTTON;
  } else if (modifiers & blink::WebInputEvent::Modifiers::kMiddleButtonDown) {
    nativeMouseButton = mojom::blink::NativeMouseButton::MIDDLE_BUTTON;
  }
  LOG(DEBUG) << "[NativeEmbed] DidNativeEmbedEvent mouse event type is : "
             << nativeMouseType << " button: " << nativeMouseButton
             << " x: " << x << " y: " << y << " embedId: " << embedId;
  widget_base_->widget_host_->DidNativeEmbedMouseEvent(
      mojom::blink::NativeEmbedMouseEvent::New(
          static_cast<String>(embedId), x, y, x, y, nativeMouseType,
          nativeMouseButton, isHitNativeArea, x, y));
}
#endif
} // namespace blink

