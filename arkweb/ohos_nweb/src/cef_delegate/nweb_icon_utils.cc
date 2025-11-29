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

#include "nweb_icon_utils.h"

#include "base/logging.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/image/image_skia_rep.h"

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
extern "C" {
void* __real_malloc(size_t);
}  // extern "C"
#endif

namespace OHOS::NWeb {

NWebExtensionActionIconColorType GetColorType(const SkBitmap& bitmap) {
  NWebExtensionActionIconColorType colorType;

  switch (bitmap.colorType()) {
    case kRGBA_8888_SkColorType:
      colorType = NWebExtensionActionIconColorType::RGBA_8888;
      break;
    case kBGRA_8888_SkColorType:
      colorType = NWebExtensionActionIconColorType::BGRA_8888;
      break;
    default:
      colorType = NWebExtensionActionIconColorType::UNKNOWN;
      LOG(ERROR) << "NWebExtensionActionIconColorType convert failed";
      break;
  }
  return colorType;
}

NWebExtensionActionIconAlphaType GetAlphaType(const SkBitmap& bitmap) {
  NWebExtensionActionIconAlphaType alphaType;
  switch (bitmap.alphaType()) {
    case kOpaque_SkAlphaType:
      alphaType = NWebExtensionActionIconAlphaType::OPAQUE;
      break;
    case kPremul_SkAlphaType:
      alphaType = NWebExtensionActionIconAlphaType::PREMUL;
      break;
    case kUnpremul_SkAlphaType:
      alphaType = NWebExtensionActionIconAlphaType::UNPREMUL;
      break;
    default:
      alphaType = NWebExtensionActionIconAlphaType::UNKNOWN;
      LOG(ERROR) << "NWebExtensionActionIconAlphaType convert failed";
      break;
  }
  return alphaType;
}

NWebExtensionActionIconBitmap CreateIconBitmap(const SkBitmap& bitmap) {
  NWebExtensionActionIconBitmap iconBitmap;
  iconBitmap.width = bitmap.width();
  iconBitmap.height = bitmap.height();
  iconBitmap.colorType = GetColorType(bitmap);
  iconBitmap.alphaType = GetAlphaType(bitmap);
  iconBitmap.bitmap = static_cast<uint8_t*>(bitmap.getPixels());
  return iconBitmap;
}

NWebExtensionActionIcon CreateActionIcon(
    const std::vector<gfx::ImageSkiaRep>& imageSkiaReps) {
  NWebExtensionActionIcon actionIcon;
  for (const auto& rep : imageSkiaReps) {
    double scale = rep.scale();
#if defined(ADDRESS_SANITIZER) || defined(HWADDRESS_SANITIZER)
    actionIcon.bitmaps[scale] =
        new NWebExtensionActionIconBitmap(CreateIconBitmap(rep.GetBitmap()));
#else
    NWebExtensionActionIconBitmap* addr =
        (NWebExtensionActionIconBitmap*)__real_malloc(
            sizeof(NWebExtensionActionIconBitmap));
    actionIcon.bitmaps[scale] = new (addr)
        NWebExtensionActionIconBitmap(CreateIconBitmap(rep.GetBitmap()));
#endif
  }
  return actionIcon;
}

NWebExtensionActionIcon* CreateActionIcon(const gfx::Image& icon_image) {
  NWebExtensionActionIcon actionIcon =
      CreateActionIcon(icon_image.AsImageSkia().image_reps());
#if defined(ADDRESS_SANITIZER) || defined(HWADDRESS_SANITIZER)
  NWebExtensionActionIcon* icon = new NWebExtensionActionIcon(actionIcon);
#else
  NWebExtensionActionIcon* addr =
      (NWebExtensionActionIcon*)__real_malloc(sizeof(NWebExtensionActionIcon));
  NWebExtensionActionIcon* icon =
      new (addr) NWebExtensionActionIcon(actionIcon);
#endif
  return icon;
}

void ReleaseActionIcon(NWebExtensionActionIcon* icon) {
  if (!icon) {
    return;
  }

  auto iter = icon->bitmaps.begin();
  while (iter != icon->bitmaps.end()) {
    NWebExtensionActionIconBitmap* bitmap = iter->second;
    iter = icon->bitmaps.erase(iter);

    if (bitmap) {
      delete bitmap;
    }
  }

  delete icon;
}

}  // namespace OHOS::NWeb
