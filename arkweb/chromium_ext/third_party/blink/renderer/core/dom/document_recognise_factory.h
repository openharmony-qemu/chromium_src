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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_FACTORY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_FACTORY_H_

#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_recognise_base.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_recognise_content.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_recognise_detail.h"

namespace blink {

enum class DocumentRecogniseType {
  RECOGNISE_CONTENT = 0,
  RECOGNISE_DETAIL = 1,
};

class DocumentRecogniseFactory {
 public:
  static std::shared_ptr<DocumentRecogniseBase> CreateRecognise(
      DocumentRecogniseType type) {
    switch (type) {
      case DocumentRecogniseType::RECOGNISE_CONTENT:
        return std::make_shared<DocumentRecogniseContent>();
      case DocumentRecogniseType::RECOGNISE_DETAIL:
        return std::make_shared<DocumentRecogniseDetail>();
      default:
        return nullptr;
    }
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_FACTORY_H_
