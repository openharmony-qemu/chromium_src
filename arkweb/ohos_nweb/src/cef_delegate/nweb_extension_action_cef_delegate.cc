/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "nweb_extension_action_cef_delegate.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#include "base/logging.h"
#include "third_party/skia/include/core/SkBitmap.h"

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
extern "C" {
void* __real_malloc(size_t);
}       // extern "C"
#endif

namespace OHOS::NWeb {

NWebExtensionActionIconColorType GetColorTypeFromSkBitmap(
    const SkBitmap& bitmap) {
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

NWebExtensionActionIconAlphaType GetAlphaTypeFromSkBitmap(
    const SkBitmap& bitmap) {
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

NWebExtensionActionIconBitmap CreateIconBitmapFromImage(
    const SkBitmap& bitmap) {
  NWebExtensionActionIconBitmap iconBitmap;

  iconBitmap.colorType = GetColorTypeFromSkBitmap(bitmap);
  iconBitmap.alphaType = GetAlphaTypeFromSkBitmap(bitmap);
  iconBitmap.width = bitmap.width();
  iconBitmap.height = bitmap.height();
  iconBitmap.bitmap = static_cast<uint8_t *>(bitmap.getPixels());

  return iconBitmap;
}

NWebExtensionActionIcon CreateFromImageSkiaReps(
    const std::vector<gfx::ImageSkiaRep>& imageSkiaReps) {
  NWebExtensionActionIcon actionIcon;
  for (auto rep : imageSkiaReps) {
    double scale = rep.scale();
#if defined(ADDRESS_SANITIZER) || defined(HWADDRESS_SANITIZER)
    actionIcon.bitmaps[scale] = new NWebExtensionActionIconBitmap(
        CreateIconBitmapFromImage(rep.GetBitmap()));
#else
    NWebExtensionActionIconBitmap* addr =
        (NWebExtensionActionIconBitmap*)__real_malloc(
            sizeof(NWebExtensionActionIconBitmap));
    actionIcon.bitmaps[scale] = new (addr) NWebExtensionActionIconBitmap(
        CreateIconBitmapFromImage(rep.GetBitmap()));
#endif
  }
  return actionIcon;
}

namespace {
std::shared_ptr<NWebExtensionActionApiCallback> g_action_api_listener =
    nullptr;
}

// static
void NWebExtensionActionCefDelegate::RegisterWebExtensionApiListener(
    std::shared_ptr<NWebExtensionActionApiCallback> action_api_listener) {
  LOG(INFO)
      << "NWebExtensionActionCefDelegate::RegisterWebExtensionApiListener";
  g_action_api_listener = action_api_listener;
}

// static
void NWebExtensionActionCefDelegate::UnRegisterWebExtensionApiListener() {
  LOG(INFO)
      << "NWebExtensionActionCefDelegate::UnRegisterWebExtensionApiListener";
  g_action_api_listener = nullptr;
}

std::unique_ptr<NWebExtensionActionCefDelegate> NWebExtensionActionCefDelegate::instance = nullptr;
std::mutex NWebExtensionActionCefDelegate::mtx;

// static
NWebExtensionActionCefDelegate* NWebExtensionActionCefDelegate::GetInstance() {
  std::lock_guard<std::mutex> lock(mtx);
  if (!instance) {
    instance = std::make_unique<NWebExtensionActionCefDelegate>();
  }
  return instance.get();
}

// static
NO_SANITIZE("cfi-icall")
void NWebExtensionActionCefDelegate::OnSetIcon(std::string extension_id,
                                               const gfx::Image& icon_image,
                                               int32_t tab_id) {
  if (!g_action_api_listener) {
    LOG(ERROR) << "No web extension action api listener";
    return;
  }

  LOG(INFO) << "OnSetIcon extension ID:" << extension_id;
  NWebExtensionActionIcon actionIcon =
      CreateFromImageSkiaReps(icon_image.AsImageSkia().image_reps());
#if defined(ADDRESS_SANITIZER) || defined(HWADDRESS_SANITIZER)
  NWebExtensionActionIcon* icon =
      new NWebExtensionActionIcon(actionIcon);
#else
  NWebExtensionActionIcon* addr =
      (NWebExtensionActionIcon*)__real_malloc(sizeof(actionIcon));
  NWebExtensionActionIcon* icon =
      new (addr) NWebExtensionActionIcon(actionIcon);
#endif
  g_action_api_listener->OnSetIcon(extension_id.c_str(), icon, tab_id);
  icon->bitmaps = std::map<double, NWebExtensionActionIconBitmap*>();
}

NO_SANITIZE("cfi-icall")
void NWebExtensionActionCefDelegate::OnDisable(const std::string& extensionId,
                                               std::optional<int>& tabId) {
  if (!g_action_api_listener) {
    LOG(ERROR) << "No web extension action api listener";
    return;
  }

  LOG(INFO) << "OnDisable extension ID:" << extensionId;

  if (!g_action_api_listener->OnDisable) {
    LOG(ERROR) << "g_action_api_listener OnDisable is nullptr";
    return;
  }

  g_action_api_listener->OnDisable(extensionId, tabId);
}

NO_SANITIZE("cfi-icall")
void NWebExtensionActionCefDelegate::OnEnable(const std::string& extensionId,
                                              std::optional<int>& tabId) {
  if (!g_action_api_listener) {
    LOG(ERROR) << "No web extension action api listener";
    return;
  }

  LOG(INFO) << "OnEnable extension ID:" << extensionId;

  if (!g_action_api_listener->OnEnable) {
    LOG(ERROR) << "g_action_api_listener OnEnable is nullptr";
    return;
  }

  g_action_api_listener->OnEnable(extensionId, tabId);
}

}  // namespace OHOS::NWeb
