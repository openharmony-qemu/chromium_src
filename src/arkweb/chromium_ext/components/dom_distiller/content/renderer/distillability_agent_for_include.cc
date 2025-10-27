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

#if BUILDFLAG(ARKWEB_READER_MODE)
constexpr char kBECEHost[] = "host";
constexpr char kBECEResultCode[] = "result_code";
constexpr char kBECEResultReason[] = "result_reason";
constexpr char kBECEPageType[] = "page_type";
constexpr char kBECEMatchTemplateId[] = "match_template_id";
constexpr char kBECEMatchTemplate[] = "match_template";
constexpr char kBECEMatchPrevFeature[] = "match_prev_feature";
constexpr char kBECEMatchNextFeature[] = "match_next_feature";
constexpr char kBECEMatchCatalogFeature[] = "match_catalog_feature";
constexpr char kBECEMatchTime[] = "match_time";

bool UrlIsHomePage(const GURL& url) {
  if (!url.has_path()) {
    return true;
  }
  bool has_ref = url.has_ref() && !url.ref().empty();
  return url.path() == "/" && !has_ref;
}

void AddBECEContent(std::vector<std::string>& statistics_content,
                    std::pair<std::string, std::string> item) {
  if (item.second.empty()) {
    return;
  }
  statistics_content.push_back(item.first + ": " + item.second);
}

void DocDistillableStatistics(blink::WebDistillabilityMatchInfo& match_info,
                              blink::WebDocument& doc,
                              double cost_time_ms) {
  std::vector<std::string> ext_info;
  AddBECEContent(ext_info,
                 {kBECEMatchTime, base::StringPrintf("%.2f", cost_time_ms)});
  if (match_info.result ==
          blink::WebDistillabilityMatchResult::MATCH_TEMPLATE ||
      match_info.result ==
          blink::WebDistillabilityMatchResult::MATCH_FEATURES ||
      match_info.result == blink::WebDistillabilityMatchResult::MATCH_DETAILS) {
    AddBECEContent(ext_info, {kBECEResultCode, "0"}); // 0:成功
    if (match_info.distill_by_regex) {
      AddBECEContent(ext_info, {kBECEResultReason, "success by regex"});
    } else {
      AddBECEContent(ext_info, {kBECEResultReason, "success by template"});
      AddBECEContent(ext_info, {kBECEMatchTemplateId, std::to_string(match_info.template_id)});
      AddBECEContent(ext_info, {kBECEMatchTemplate, match_info.templates});
    }
    if (match_info.result ==
        blink::WebDistillabilityMatchResult::MATCH_DETAILS) {
      AddBECEContent(ext_info, {kBECEPageType, "0"}); // 0:详情页
    } else {
      AddBECEContent(ext_info, {kBECEPageType, "1"}); // 1:正文页
      if (!match_info.distill_by_regex) {
        AddBECEContent(ext_info, {kBECEMatchPrevFeature, match_info.prev_feature});
        AddBECEContent(ext_info, {kBECEMatchNextFeature, match_info.next_feature});
        AddBECEContent(ext_info,
                       {kBECEMatchCatalogFeature, match_info.catalog_feature});
      }
    }
    LOG(INFO) << "[Distiller] kernel_novel_distillable_result: " << base::JoinString(ext_info, "; ");
  }
}

blink::WebDistillabilityMatchResult DetermineDistillableMatchResult(
    blink::WebDocument& doc,
    const blink::mojom::UrlHostDistillerInfoPtr& distiller_info) {
  if (UrlIsHomePage(doc.Url())) {
    LOG(INFO) << "[Distiller] doc url is home page.";
    return blink::WebDistillabilityMatchResult::MATCH_NONE;
  }
  base::TimeTicks start_time = base::TimeTicks::Now();
  blink::WebDistillabilityMatchInfo match_info =
      doc.DistillabilityMatchInfo(distiller_info);
  double cost_time_ms = (base::TimeTicks::Now() - start_time).InMillisecondsF();
  DocDistillableStatistics(match_info, doc, cost_time_ms);
  LOG(INFO) << "[Distiller] match_result:" << (int)match_info.result;
  return match_info.result;
}
#endif