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

namespace blink {
class HTMLPlugInElement;

class HTMLPlugInElementUtils {
 public:
  HTMLPlugInElementUtils(HTMLPlugInElement* pluginElement);
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  bool CheckNativeType(const char* key) const;
  bool CheckIntrinsicSizeEnable() const;
  bool IsCssDisplayChangeEnabled() const;
#endif
 private:
  raw_ptr<HTMLPlugInElement> plugin_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_PLUGIN_ELEMENT_UTILS_H_