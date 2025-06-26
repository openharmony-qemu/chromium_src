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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/html/html_plugin_element_utils.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/html/html_plugin_element.h"
#include "third_party/blink/renderer/core/html/html_native_loader.h"
#include "third_party/blink/public/platform/web_string.h"

namespace blink {

HTMLPlugInElementUtils::HTMLPlugInElementUtils(HTMLPlugInElement* pluginElement)
    : plugin_(pluginElement) {}

#if BUILDFLAG(ARKWEB_SAME_LAYER)
bool HTMLPlugInElementUtils::CheckNativeType(const char* key) const {
  auto settings = plugin_->GetDocument().GetSettings();
  if (plugin_->GetObjectContentType() != HTMLPlugInElement::ObjectContentType::kNone) {
    LOG(ERROR) << "[NativeEmbed] It's a standard object content type "
               << (int)plugin_->GetObjectContentType();
    return false;
  }

  if (!settings || !settings->GetNativeEmbedModeEnabled()) {
    LOG(ERROR) << "[NativeEmbed] Native embed mode is not enabled.";
    return false;
  }

  auto rule = settings->NativeEmbedRule();
  auto valid_key = WebString::FromUTF8(key);
  if (rule.find(valid_key) == rule.end()) {
    LOG(ERROR) << "[NativeEmbed] Invalid native embed tag " << valid_key.Utf8();
    return false;
  }

  if (!plugin_->service_type_.StartsWith(rule[valid_key])) {
    LOG(ERROR) << "[NativeEmbed] Registered type " << rule[valid_key].Utf8()
               << " doens't match " << plugin_->service_type_;
    return false;
  }
  return true;
}

bool HTMLPlugInElementUtils::CheckIntrinsicSizeEnable() const {
  auto settings = plugin_->GetDocument().GetSettings();
  if (!settings || !settings->GetNativeEmbedModeEnabled()) {
    return false;
  }
  return settings->GetIntrinsicSizeEnabled();
}

bool HTMLPlugInElementUtils::IsCssDisplayChangeEnabled() const {
  auto settings = plugin_->GetDocument().GetSettings();
  if (!settings || !settings->GetNativeEmbedModeEnabled() || !plugin_->IsNativeType()) {
    return false;
  }
  return settings->GetCSSDisplayChangeEnabled();
}

void HTMLPlugInElementUtils::SetNativeEmbedOverlay(bool native_embed_overlay) {
  if (native_embed_overlay_ == native_embed_overlay) {
    return;
  }
  native_embed_overlay_ = native_embed_overlay;
  if (auto* native_loader = plugin_->NativeLoader()) {
    native_loader->SetNativeEmbedOverlay(native_embed_overlay_);
  }
}

void HTMLPlugInElementUtils::SetNativeEmbedOverlayInfinity(bool native_embed_overlay_infinity) {
  if (native_embed_overlay_infinity_ == native_embed_overlay_infinity) {
    return;
  }
  native_embed_overlay_infinity_ = native_embed_overlay_infinity;
  if (auto* native_loader = plugin_->NativeLoader()) {
    native_loader->SetNativeEmbedOverlayInfinity(native_embed_overlay_infinity_);
  }
}
#endif

}  // namespace blink