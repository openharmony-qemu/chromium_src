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
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/html/html_plugin_element.h"
#include "third_party/blink/renderer/core/html/html_native_loader.h"
#include "third_party/blink/public/platform/web_string.h"

namespace blink {

// LCOV_EXCL_START
HTMLPlugInElementUtils::HTMLPlugInElementUtils(HTMLPlugInElement* pluginElement)
    : plugin_(pluginElement) {}
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_SAME_LAYER)
bool HTMLPlugInElementUtils::CheckNativeType(const char* key) const {
  if (!plugin_->GetDocument().IsActive() || !plugin_->GetDocument().GetFrame()) {
    LOG(ERROR) << "[NativeEmbed] Document is not active or has no frame";
    return false;
  }
  
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

// LCOV_EXCL_START
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
// LCOV_EXCL_STOP

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

void HTMLPlugInElementUtils::AnalysisStretchContentToFillBounds(
    const Element::AttributeModificationParams& params) {
  bool stretch_content_to_fill_bounds = true;
  if (!params.new_value.empty()) {
    base::StringPairs pairs;
    base::SplitStringIntoKeyValuePairs(params.new_value.Utf8(), ':', ';', &pairs);
    for (const auto& pair : pairs) {
      std::string key = base::ToLowerASCII(
          base::TrimWhitespaceASCII(pair.first, base::TRIM_ALL));
      if (key == "object-fit") {
        std::string value = base::ToLowerASCII(
            base::TrimWhitespaceASCII(pair.second, base::TRIM_ALL));
        if (value == "none") {
          stretch_content_to_fill_bounds = false;
        } else if (value == "stretch") {
          stretch_content_to_fill_bounds = true;
        }
        break;
      }
    }
  }
  SetStretchContentToFillBounds(stretch_content_to_fill_bounds);
}

void HTMLPlugInElementUtils::SetStretchContentToFillBounds(bool stretch_content_to_fill_bounds) {
  if (auto* native_loader = plugin_->NativeLoader()) {
    native_loader->SetStretchContentToFillBounds(stretch_content_to_fill_bounds);
  } else {
    stretch_content_to_fill_bounds_ = stretch_content_to_fill_bounds;
  }
}

void HTMLPlugInElementUtils::ProcessStretchContentToFillBounds() {
  if (auto* native_loader = plugin_->NativeLoader()) {
    native_loader->SetStretchContentToFillBounds(stretch_content_to_fill_bounds_);
  }
}

void HTMLPlugInElementUtils::ProcessParamChanges(const Vector<ParamChangeInfo>& changes) {
  if (auto* native_loader = plugin_->NativeLoader()) {
    native_loader->ProcessParamChanges(changes);
  } else {
    buffered_param_changes_.AppendVector(changes);
  }
}

void HTMLPlugInElementUtils::ProcessBufferedParamChanges() {
  if (!buffered_param_changes_.empty()) {
    if (auto* native_loader = plugin_->NativeLoader()) {
      native_loader->ProcessParamChanges(buffered_param_changes_);
    }
  }
}
#endif

}  // namespace blink