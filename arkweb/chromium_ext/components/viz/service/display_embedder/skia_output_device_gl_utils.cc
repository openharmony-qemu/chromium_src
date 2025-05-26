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

#include "arkweb/chromium_ext/components/viz/service/display_embedder/skia_output_device_gl_utils.h"

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ui/gl/gl_surface.h"

namespace viz {
SkiaOutputDeviceGLUtils::SkiaOutputDeviceGLUtils(SkiaOutputDeviceGL* impl)
    : skiaOutPutDeviceGl_(impl)
{
  supports_damage_region_ = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                .GetSystemPropertiesInstance()
                                .GetBoolParameter("web.damageRegion.enable", 1);
}

gfx::SwapResult SkiaOutputDeviceGLUtils::SwapBuffers(
    const std::optional<gfx::Rect>& update_rect,
    SkiaOutputDevice::BufferPresentedCallback& feedback,
    OutputSurfaceFrame& frame)
{
  gfx::SwapResult result;
  auto data = frame.data;
  if (!base::ohos::IsPageScale() && supports_damage_region_) {
    result = skiaOutPutDeviceGl_->gl_surface_->SwapBuffersWithDamage(
        {update_rect->x(),
         skiaOutPutDeviceGl_->gl_surface_->GetSize().height() -
             update_rect->y() - update_rect->height(),
         update_rect->width(), update_rect->height()},
        std::move(feedback), std::move(data));
    LOG(DEBUG) << "Present calling SwapBuffersWithDamage [" << update_rect->x()
               << ", "
               << skiaOutPutDeviceGl_->gl_surface_->GetSize().height() -
                      update_rect->y() - update_rect->height()
               << ", " << update_rect->width() << ", " << update_rect->height()
               << "]";
  } else {
    result = skiaOutPutDeviceGl_->gl_surface_->SwapBuffers(std::move(feedback),
                                                           std::move(data));
  }
  return result;
}

}  // namespace viz
