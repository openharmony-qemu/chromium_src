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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_recognise_util.h"

#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_recognise_base.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_recognise_factory.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/html/html_head_element.h"

namespace blink {
WebDistillabilityMatchInfo DocumentRecogniseUtil::DocDistillMatch(
    Document& document,
    const blink::mojom::UrlHostDistillerInfoPtr& distiller_info) {
  WebDistillabilityMatchInfo default_match_info;
  default_match_info.result = WebDistillabilityMatchResult::MATCH_NONE;

  if (!document.GetFrame() || !document.GetFrame()->IsMainFrame()) {
    return default_match_info;
  }

  HTMLElement* body = document.body();
  HTMLElement* head = document.head();
  if (!body || !head) {
    return default_match_info;
  }

  base::TimeTicks start_time = base::TimeTicks::Now();

  // if host match blacklist, return match none
  bool is_need_check_blacklist = !distiller_info->blacklist_regexes.empty();
  if (is_need_check_blacklist &&
      DocumentRecogniseBase::IsDocUrlMatchRegexes(
          document, distiller_info->blacklist_regexes)) {
    LOG(INFO)
        << __func__
        << " [Distiller] MATCH_NONE because host is in blacklist, consume time:"
        << (base::TimeTicks::Now() - start_time).InMillisecondsF();
    default_match_info.distill_by_regex = true;
    return default_match_info;
  }

  auto document_recognise_content = DocumentRecogniseFactory::CreateRecognise(
      DocumentRecogniseType::RECOGNISE_CONTENT);
  const WebDistillabilityMatchInfo& content_match_info =
      document_recognise_content->DocDistillabilityMatch(document,
                                                         distiller_info);
  if (content_match_info.result != WebDistillabilityMatchResult::MATCH_NONE) {
    LOG(INFO) << __func__ << " [Distiller] MATCH_TEMPLATE consume time:"
              << (base::TimeTicks::Now() - start_time).InMillisecondsF();
    return content_match_info;
  }

  auto document_recognise_detail = DocumentRecogniseFactory::CreateRecognise(
      DocumentRecogniseType::RECOGNISE_DETAIL);
  const WebDistillabilityMatchInfo& detail_match_info =
      document_recognise_detail->DocDistillabilityMatch(document,
                                                        distiller_info);
  if (detail_match_info.result != WebDistillabilityMatchResult::MATCH_NONE) {
    LOG(INFO) << __func__ << " [Distiller] MATCH_DETAILS consume time:"
              << (base::TimeTicks::Now() - start_time).InMillisecondsF();
    return detail_match_info;
  }

  LOG(INFO) << __func__ << " [Distiller] MATCH_NONE consume time:"
            << (base::TimeTicks::Now() - start_time).InMillisecondsF() << "ms";
  return default_match_info;
}

}  // namespace blink
