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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_DETAIL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_DETAIL_H_

#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_novel_template_features.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_recognise_base.h"

namespace blink {

class DocumentRecogniseDetail : public DocumentRecogniseBase {
 public:
  DocumentRecogniseDetail() = default;
  ~DocumentRecogniseDetail() override;

 protected:
  void ChildDocDistillabilityMatch(
      Document& document,
      const blink::mojom::UrlHostDistillerInfoPtr& distiller_info) override;

 private:
  bool IsMatchDetailsTemplates(Document& document,
                               const std::vector<std::string>& page_template);
  bool IsChapterElementValid(Element* chapter_element);
  void ConfigMatchInfo(const int32_t template_id, const std::vector<std::string>& page_template);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_DETAIL_H_
