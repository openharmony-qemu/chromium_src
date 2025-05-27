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

#ifndef CONTENT_BROWSER_OHOS_SOFTWARE_COMPOSITOR_HOST_OHOS_H_
#define CONTENT_BROWSER_OHOS_SOFTWARE_COMPOSITOR_HOST_OHOS_H_

#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/browser/renderer_host/render_widget_host_view_base.h"
#include "content/common/content_export.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "arkweb/chromium_ext/third_party/blink/public/mojom/input/software_compositor_ohos.mojom.h"

namespace content {
class RenderWidgetHostImpl;

class SoftwareCompositorHostOhos {
 public:
  SoftwareCompositorHostOhos(RenderWidgetHostImpl* view);
  ~SoftwareCompositorHostOhos();

  SoftwareCompositorHostOhos(const SoftwareCompositorHostOhos&) = delete;
  SoftwareCompositorHostOhos& operator=(const SoftwareCompositorHostOhos&) =
      delete;

  using WebSnapchatCallback = base::OnceCallback<
      void(const char* id, bool state, void* data, int width, int height)>;

  void DemandDrawSwAsync(const char* id,
                         float clip_width,
                         float clip_height,
                         gfx::SizeF size,
                         gfx::PointF offset,
                         WebSnapchatCallback callback);
  void DrawRect(const gfx::Rect& rect);
  void OnDrawSwCallback(WebSnapchatCallback callback, std::string, bool result);
  void SetSharedMemory(size_t stride, size_t buffer_size);

 private:
  void DumpSnapshot(std::string type);  // for inner debug
  gfx::SizeF current_;
  struct SharedMemoryWithSize;
  const raw_ptr<RenderWidgetHostImpl> widget_;
  mojo::Remote<blink::mojom::SoftwareCompositorOhos> software_compositor_;
  std::unique_ptr<SharedMemoryWithSize> software_draw_shm_;
  base::WeakPtrFactory<SoftwareCompositorHostOhos> weak_factory_{this};
};

}  // namespace content

#endif  // CONTENT_BROWSER_OHOS_SOFTWARE_COMPOSITOR_HOST_OHOS_H_
