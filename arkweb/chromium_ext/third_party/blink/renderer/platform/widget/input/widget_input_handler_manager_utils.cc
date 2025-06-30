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

#include "third_party/blink/renderer/platform/widget/input/widget_input_handler_manager_utils.h"

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/widget/widget_base_utils.h"
#include "third_party/blink/renderer/platform/widget/widget_base.h"
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
#include "cc/mojo_embedder/software_compositor_registry_ohos.h"
#include "third_party/blink/renderer/platform/widget/input/software_compositor_proxy_ohos.h"
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "third_party/blink/renderer/platform/widget/input/arkweb_widget_input_handler_impl_ext.h"
#endif


namespace blink {

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
class SoftwareCompositorProxyRegistryOhos
    : public cc::mojo_embedder::SoftwareCompositorRegistryOhos {
 public:
  explicit SoftwareCompositorProxyRegistryOhos(
      scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner)
      : compositor_thread_default_task_runner_(
            std::move(compositor_task_runner)) {}

  ~SoftwareCompositorProxyRegistryOhos() {
    // Ensure the proxy has already been release on the compositor thread
    // before destroying this object.
    DCHECK(!proxy_);
  }

  void CreateProxy(InputHandlerProxy* handler) {
    DCHECK(compositor_thread_default_task_runner_->BelongsToCurrentThread());
    proxy_ = std::make_unique<SoftwareCompositorProxyOhos>();

    if (renderer_) {
      proxy_->SetSoftwareRenderer(renderer_);
    }
  }

  SoftwareCompositorProxyOhos* proxy() { return proxy_.get(); }

  void RegisterSoftwareRenderer(
      cc::mojo_embedder::SoftwareCompositorRendererOhos* software_renderer)
      override {
    DCHECK(compositor_thread_default_task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(nullptr, renderer_);
    renderer_ = software_renderer;
    if (proxy_) {
      proxy_->SetSoftwareRenderer(software_renderer);
    }
  }

  void UnregisterSoftwareRenderer(
      cc::mojo_embedder::SoftwareCompositorRendererOhos* software_renderer)
      override {
    DCHECK(compositor_thread_default_task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(software_renderer, renderer_);
    renderer_ = nullptr;
  }

  void DestroyProxy() {
    DCHECK(compositor_thread_default_task_runner_->BelongsToCurrentThread());
    proxy_.reset();
  }

 private:
  scoped_refptr<base::SingleThreadTaskRunner>
      compositor_thread_default_task_runner_;
  std::unique_ptr<SoftwareCompositorProxyOhos> proxy_;
  raw_ptr<cc::mojo_embedder::SoftwareCompositorRendererOhos> renderer_ = nullptr;
};
#endif


WidgetInputHandlerManagerUtils::WidgetInputHandlerManagerUtils(
    WidgetInputHandlerManager* manager) : manager_(manager) {
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  software_proxy_registry_ =
      std::make_unique<SoftwareCompositorProxyRegistryOhos>(
          manager_->compositor_thread_default_task_runner_);
#endif
}
WidgetInputHandlerManagerUtils::~WidgetInputHandlerManagerUtils() {}

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
void WidgetInputHandlerManagerUtils::AttachSoftwareCompositorOhos(
    mojo::PendingReceiver<mojom::blink::SoftwareCompositorOhos>
        compositor_request) {
  if (software_proxy_registry_ && software_proxy_registry_->proxy()) {
    software_proxy_registry_->proxy()->BindChannel(
        std::move(compositor_request));
  }
}

void WidgetInputHandlerManagerUtils::CreateProxy() {
  if (software_proxy_registry_) {
    software_proxy_registry_->CreateProxy(manager_->input_handler_proxy_.get());
  }
}

void WidgetInputHandlerManagerUtils::DestroyProxy() {
  if (software_proxy_registry_) {
    software_proxy_registry_->DestroyProxy();
  }
}

cc::mojo_embedder::SoftwareCompositorRegistryOhos*
WidgetInputHandlerManagerUtils::GetSoftwareCompositorRegistryOhos() {
  DCHECK(software_proxy_registry_);
  return software_proxy_registry_.get();
}
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void WidgetInputHandlerManagerUtils::SetZoomLevel(float magnify_delta,
                                             const gfx::Point& anchor) {
  if (!manager_->input_handler_proxy_) {
    return;
  }
  manager_->input_handler_proxy_->SynchronouslyZoomBy(magnify_delta, anchor);
}

void WidgetInputHandlerManagerUtils::AsyncSetZoomLevel(float magnify_delta,
                                                  const gfx::Point& anchor) {
  manager_->InputThreadTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&WidgetInputHandlerManagerUtils::SetZoomLevel,
                                AsWeakPtr(), magnify_delta, anchor));
}

void WidgetInputHandlerManagerUtils::SetOverscrollMode(int mode) {
  if (!manager_->input_handler_proxy_) {
    return;
  }
  manager_->input_handler_proxy_->proxy_utils()->SetOverscrollMode(mode);
}

void WidgetInputHandlerManagerUtils::ScrollBy(float delta_x, float delta_y) {
  if (!manager_->input_handler_proxy_) {
    return;
  }
  manager_->input_handler_proxy_->proxy_utils()->ScrollBy(delta_x, delta_y);
}

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
gfx::Vector2dF WidgetInputHandlerManagerUtils::GetOverScrollOffset() {
  gfx::Vector2dF overscroll_offset;
  overscroll_offset.set_x(0.0f);
  overscroll_offset.set_y(0.0f);
  if (!manager_->input_handler_proxy_) {
    return overscroll_offset;
  }
  return manager_->input_handler_proxy_->proxy_utils()->GetOverScrollOffset();
}
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void WidgetInputHandlerManagerUtils::DidNativeEmbedEvent(
    blink::WebInputEvent::Type type,
    std::string embedId,
    int32_t id,
    float x,
    float y) {
  manager_->main_thread_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&WidgetBaseUtils::DidNativeEmbedEvent,
                                manager_->widget_->utils()->GetWeakPtr(), type,
                                embedId, id, x, y));
}

void WidgetInputHandlerManagerUtils::SetGestureEventResult(bool result,
                                                      bool stopPropagation) {
  if (!manager_->input_handler_proxy_) {
    return;
  }
  manager_->input_handler_proxy_->proxy_utils()->SetGestureEventResult(result, stopPropagation);
}

void WidgetInputHandlerManagerUtils::TouchHitTest(const WebPointerEvent& event,
                                             size_t fingerId) {
  manager_->main_thread_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&WidgetBaseUtils::TouchHitTest,
                                manager_->widget_->utils()->GetWeakPtr(), event,
                                fingerId));
}

void WidgetInputHandlerManagerUtils::NativeHitTestResult(bool isNative,
                                                    size_t fingerId,
                                                    int layerId) {
  manager_->compositor_thread_default_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&WidgetInputHandlerManagerUtils::AsyncNativeHitTestResult, AsWeakPtr(),
                     isNative, fingerId, layerId));
}

void WidgetInputHandlerManagerUtils::AsyncNativeHitTestResult(bool isNative,
                                                         size_t fingerId,
                                                         int layerId) {
  if (manager_->input_handler_proxy_) {
    manager_->input_handler_proxy_->proxy_utils()->NativeHitTestResult(isNative, fingerId, layerId);
  }
}

void WidgetInputHandlerManagerUtils::SetNativeEmbedMode(bool flag) {
  if (!manager_->input_handler_proxy_) {
    return;
  }
  manager_->input_handler_proxy_->proxy_utils()->SetNativeEmbedMode(flag);
}

void WidgetInputHandlerManagerUtils::DidNativeEmbedMouseEvent(
    blink::WebInputEvent::Type type,
    blink::WebInputEvent::Modifiers modifiers,
    std::string embedId,
    bool isHitNativeArea,
    float x,
    float y) {
  manager_->main_thread_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&WidgetBaseUtils::DidNativeEmbedMouseEvent,
                                manager_->widget_->utils()->GetWeakPtr(), type,
                                modifiers, embedId, isHitNativeArea, x, y));
}

void WidgetInputHandlerManagerUtils::SetMouseEventResult(bool result, bool stopPropagation) {
  if (!manager_->input_handler_proxy_) {
    return;
  }
  manager_->input_handler_proxy_->proxy_utils()->SetMouseEventResult(result, stopPropagation);
}

void WidgetInputHandlerManagerUtils::MouseHitTest(const WebMouseEvent& event) {
  manager_->main_thread_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&WidgetBaseUtils::MouseHitTest,
                     manager_->widget_->utils()->GetWeakPtr(), event));
}

void WidgetInputHandlerManagerUtils::NativeMouseHitTestResult(bool isNative, int layerId) {
  manager_->compositor_thread_default_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&WidgetInputHandlerManagerUtils::AsyncNativeMouseHitTestResult, AsWeakPtr(),
                     isNative, layerId));
}

void WidgetInputHandlerManagerUtils::AsyncNativeMouseHitTestResult(bool isNative, int layerId) {
  if (manager_->input_handler_proxy_) {
    manager_->input_handler_proxy_->proxy_utils()->NativeMouseHitTestResult(isNative, layerId);
  }
}
#endif

}