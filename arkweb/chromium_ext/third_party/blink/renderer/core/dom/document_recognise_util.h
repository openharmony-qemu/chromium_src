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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_UTIL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_UTIL_H_

#include "arkweb/chromium_ext/third_party/blink/public/mojom/dom_distiller/reader_mode_config.mojom.h"
#include "arkweb/chromium_ext/third_party/blink/platform/web_distillability_match.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"

namespace blink {
class CORE_EXPORT DocumentRecogniseUtil {
  STATIC_ONLY(DocumentRecogniseUtil);

 public:
  static WebDistillabilityMatchInfo DocDistillMatch(
      Document& document,
      const blink::mojom::UrlHostDistillerInfoPtr& distiller_info);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_UTIL_H_
