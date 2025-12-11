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
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_PLUGIN_ELEMENT_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_PLUGIN_ELEMENT_UTILS_H_

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/html/html_plugin_element.h"

namespace blink {
class HTMLPlugInElement;

struct ParamChangeInfo {
  enum class Status { kAdd, kUpdate, kDelete };

  ParamChangeInfo(Status s,
                  const AtomicString& i,
                  const AtomicString& n,
                  const AtomicString& v)
      : status(s), id(i), name(n), value(v) {}

  Status status;
  AtomicString id;
  AtomicString name;
  AtomicString value;
};

class HTMLPlugInElementUtils {
 public:
  HTMLPlugInElementUtils(HTMLPlugInElement* pluginElement);
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  bool CheckNativeType(const char* key) const;
  bool CheckIntrinsicSizeEnable() const;
  bool IsCssDisplayChangeEnabled() const;
  void SetNativeEmbedOverlay(bool native_embed_overlay);
  void SetNativeEmbedOverlayInfinity(bool native_embed_overlay_infinity);
  void ProcessParamChanges(const Vector<ParamChangeInfo>& changes);
  void ProcessBufferedParamChanges();
  void SetStretchContentToFillBounds(bool stretch_content_to_fill_bounds);
  void ProcessStretchContentToFillBounds();
  void AnalysisStretchContentToFillBounds(const Element::AttributeModificationParams& params);
  bool IsOverlay() const {
    return native_embed_overlay_;
  }
  bool IsOverlayInfinity() const {
    return native_embed_overlay_infinity_;
  }
#endif
 private:
#if BUILDFLAG(ARKWEB_TEST)
  friend class HTMLPlugInElementUtilsTest;
#endif
  raw_ptr<HTMLPlugInElement> plugin_;
  bool native_embed_overlay_{false};
  bool native_embed_overlay_infinity_{false};
  bool stretch_content_to_fill_bounds_{true};
  Vector<ParamChangeInfo> buffered_param_changes_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_PLUGIN_ELEMENT_UTILS_H_