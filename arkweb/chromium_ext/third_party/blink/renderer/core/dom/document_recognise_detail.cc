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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_recognise_detail.h"

#include "arkweb/chromium_ext/third_party/blink/public/mojom/dom_distiller/reader_mode_config.mojom-shared.h"
#include "arkweb/chromium_ext/third_party/blink/public/mojom/dom_distiller/reader_mode_config.mojom.h"
#include "base/strings/string_util.h"

namespace blink {

namespace {
bool IsChapterElementHasValidChildren(Element* chapter_element) {
  if (!chapter_element) {
    return false;
  }
  unsigned children_count = chapter_element->CountChildren();
  if (children_count == 0) {
    LOG(INFO) << "[Distiller] chapter element has no children.";
    return false;
  }
  if (children_count == 1 && chapter_element->firstChild()->IsTextNode()) {
    LOG(INFO) << "[Distiller] chapter element has only one text node children, "
                 "not meet requirement";
    return false;
  }
  return true;
}

Element* QueryElement(
    Document& document,
    const WTF::AtomicString& details_template) {
  return document.QuerySelector(details_template);
}
}  // namespace

DocumentRecogniseDetail::~DocumentRecogniseDetail() = default;

bool DocumentRecogniseDetail::IsChapterElementValid(Element* chapter_element) {
  if (!chapter_element) {
    return false;
  }
  if (IsElementDisplayNone(chapter_element)) {
    LOG(INFO) << "[Distiller] chapter element display none.";
    return false;
  }
  if (!IsChapterElementHasValidChildren(chapter_element)) {
    LOG(INFO) << "[Distiller] chapter element has no child.";
    return false;
  }
  return true;
}

bool DocumentRecogniseDetail::IsMatchDetailsTemplates(
    Document& document,
    const std::vector<std::string>& page_template) {
  if (!QueryElement(
          document,
          WTF::AtomicString::FromUTF8(
              page_template[static_cast<int32_t>(
                                blink::mojom::DetailTemplateIndex::INFO)]
                  .c_str()))) {
    return false;
  }
  if (!QueryElement(
          document,
          WTF::AtomicString::FromUTF8(
              page_template[static_cast<int32_t>(
                                blink::mojom::DetailTemplateIndex::INTRO)]
                  .c_str()))) {
    return false;
  }
  Element* chapter_element = QueryElement(
      document,
      WTF::AtomicString::FromUTF8(
          page_template[static_cast<int32_t>(
                            blink::mojom::DetailTemplateIndex::CHAPTERS)]
              .c_str()));
  return IsChapterElementValid(chapter_element);
}

void DocumentRecogniseDetail::ChildDocDistillabilityMatch(
    Document& document,
    const blink::mojom::UrlHostDistillerInfoPtr& distiller_info) {
  bool distill_by_regex = !distiller_info->detail_regexes.empty();
  if (distill_by_regex) {
    match_info_.distill_by_regex = true;
    if (IsDocUrlMatchRegexes(document, distiller_info->detail_regexes)) {
      match_info_.result = WebDistillabilityMatchResult::MATCH_DETAILS;
    }
    return;
  }

  // in order to improve query speed
  std::unordered_set<int32_t> applicable_templates_set;
  const std::vector<int32_t>& applicable_templates =
      distiller_info->templates_for_host;
  for (int32_t item : applicable_templates) {
    applicable_templates_set.insert(item);
  }

  const auto* novel_details_page_templates =
      NovelTemplateFeatures::GetInstance().GetDetailsPageTemplates();
  if (novel_details_page_templates) {
    bool applicable_templates_configured = !applicable_templates_set.empty();
    size_t valid_size = static_cast<int32_t>(blink::mojom::DetailTemplateIndex::CHAPTERS) + 1;
    for (const auto& details_template : *novel_details_page_templates) {
      // 若配置了applicable templates，过滤不在其中的
      if (applicable_templates_configured &&
          applicable_templates_set.find(details_template.first) ==
              applicable_templates_set.end()) {
        continue;
      }
      if (details_template.second.size() != valid_size) {
        continue;
      }
      if (IsMatchDetailsTemplates(document, details_template.second)) {
        ConfigMatchInfo(details_template.first, details_template.second);
        return;
      }
    }
  }
}

void DocumentRecogniseDetail::ConfigMatchInfo(
    const int32_t template_id,
    const std::vector<std::string>& page_template) {
  match_info_.result = WebDistillabilityMatchResult::MATCH_DETAILS;
  match_info_.template_id = template_id;
  match_info_.templates.clear();
  match_info_.templates = base::JoinString(page_template, ",");
}

}  // namespace blink
