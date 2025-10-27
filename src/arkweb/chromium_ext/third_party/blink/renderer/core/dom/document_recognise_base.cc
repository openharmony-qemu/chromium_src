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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_recognise_base.h"

#include "third_party/re2/src/re2/stringpiece.h"
#include "third_party/re2/src/re2/re2.h"

namespace blink {
const WebDistillabilityMatchInfo& DocumentRecogniseBase::DocDistillabilityMatch(
    Document& document,
    const blink::mojom::UrlHostDistillerInfoPtr& distiller_info) {
  ChildDocDistillabilityMatch(document, distiller_info);
  LOG(INFO) << " [Distiller] DocumentRecogniseBase::DocDistillabilityMatch match result:" << (int)match_info_.result
            << " distill_by_regex:" << match_info_.distill_by_regex
            << " templates:" << match_info_.templates
            << " prev_feature:" << match_info_.prev_feature
            << " next_feature:" << match_info_.next_feature
            << " catalog_feature:" << match_info_.catalog_feature;
  return match_info_;
}

bool DocumentRecogniseBase::IsElementDisplayNone(Element* element) {
  if (element == nullptr) {
    LOG(WARNING) << __func__ << " [Distiller] param element is null.";
    return false;
  }

  return element->GetLayoutObject() == nullptr;
}

bool DocumentRecogniseBase::IsDocUrlMatchRegexes(
    const Document& document,
    const std::vector<std::string>& regexes) {
  GURL gurl = GURL(document.Url());
  if (regexes.empty() || !gurl.is_valid()) {
    LOG(INFO) << __func__ << " [Distiller] regexes or url is empty, no need distill by regex";
    return false;
  }

  // /path#fragment
  std::string path_ref_string;
  if (gurl.has_path()) {
    path_ref_string.append(gurl.path());
  }
  if (gurl.has_ref()) {
    path_ref_string.append("#");
    path_ref_string.append(gurl.ref());
  }
  if (path_ref_string.empty()) {
    LOG(INFO) << __func__ << " [Distiller] url can't find path or ref.";
    return false;
  }

  re2::StringPiece input(path_ref_string);
  for (const std::string& regexStr : regexes) {
    re2::RE2 pattern(regexStr);
    if (pattern.ok() && re2::RE2::FullMatch(input, pattern)) {
      LOG(INFO) << __func__ << " [Distiller] distill by regex: match";
      return true;
    }
  }

  LOG(INFO) << __func__ << " [Distiller] distill by regex: not match";
  return false;
}
}  // namespace blink