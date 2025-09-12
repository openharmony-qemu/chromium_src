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

#ifndef THIRD_PARTY_BLINK_PLATFORM_WEB_DISTILLABILITY_MATCH_H_
#define THIRD_PARTY_BLINK_PLATFORM_WEB_DISTILLABILITY_MATCH_H_

#include <string>

namespace blink {
enum class WebDistillabilityMatchResult {
  MATCH_NONE = 0,
  MATCH_TEMPLATE = 1,
  MATCH_FEATURES = 2,
  MATCH_BOTH = 3,
  MATCH_DETAILS = 4
};

struct WebDistillabilityMatchInfo {
  WebDistillabilityMatchResult result = WebDistillabilityMatchResult::MATCH_NONE;
  // 是否正则匹配成功，默认false(模板匹配)
  bool distill_by_regex = false;
  // 模板id
  int32_t template_id = 0;
  // 模板字串，逗号分隔,比如"div#nr,[class*=\"nr_page\"]"
  std::string templates;
  // 匹配到的上页文本特征,比如"上一章"
  std::string prev_feature;
  // 匹配到的下页文本特征,比如"下一章"
  std::string next_feature;
  // 匹配到的目录文本特征,比如"目录"
  std::string catalog_feature;
};
}  // namespace blink
#endif // THIRD_PARTY_BLINK_PLATFORM_WEB_DISTILLABILITY_MATCH_H_
