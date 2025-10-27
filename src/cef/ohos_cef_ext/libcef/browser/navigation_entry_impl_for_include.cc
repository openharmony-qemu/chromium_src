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

#if BUILDFLAG(ARKWEB_NAVIGATION)
#include "content/public/browser/favicon_status.h"
#include "third_party/skia/include/core/SkBitmap.h"
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

#if BUILDFLAG(ARKWEB_NAVIGATION)
bool CefNavigationEntryImpl::GetFavicon(void** pixel_data,
                                        int& color_type,
                                        int& alpha_type,
                                        int& pixel_width,
                                        int& pixel_height) {
  CEF_VALUE_VERIFY_RETURN(false, false);
  auto favicon_status = mutable_value()->GetFavicon();
  if (!favicon_status.valid) {
    return false;
  }
  const SkBitmap* bitmap = favicon_status.image.ToSkBitmap();
  if (!bitmap) {
    return false;
  }
  color_type = bitmap->colorType();
  alpha_type = bitmap->alphaType();
  pixel_width = bitmap->width();
  pixel_height = bitmap->height();
  *pixel_data = bitmap->getPixels();
  return true;
}
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)