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
constexpr char kBECEKernelNovelDistillableResult[] = "kernel_novel_distillable_result";

bool UrlIsHomePage(const GURL& url) {
  if (!url.has_path()) {
    return true;
  }
  bool has_ref = url.has_ref() && !url.ref().empty();
  return url.path() == "/" && !has_ref;
}

void DocDistillableStatistics(content::RenderFrame* render_frame,
                              blink::WebDistillabilityMatchInfo& match_info,
                              blink::WebDocument& doc,
                              double cost_time_ms) {
  base::Value::Dict ext_info;
  ext_info.Set(kBECEHost, GURL(doc.Url()).host());
  ext_info.Set(kBECEMatchTime, base::StringPrintf("%.2f", cost_time_ms));
  if (match_info.result ==
          blink::WebDistillabilityMatchResult::MATCH_TEMPLATE ||
      match_info.result ==
          blink::WebDistillabilityMatchResult::MATCH_FEATURES ||
      match_info.result == blink::WebDistillabilityMatchResult::MATCH_DETAILS) {
    ext_info.Set(kBECEResultCode, "0"); // 0:成功
    if (match_info.distill_by_regex) {
      ext_info.Set(kBECEResultReason, "success by regex");
    } else {
      ext_info.Set(kBECEResultReason, "success by template");
      ext_info.Set(kBECEMatchTemplateId, std::to_string(match_info.template_id));
      ext_info.Set(kBECEMatchTemplate, match_info.templates);
    }
    if (match_info.result ==
        blink::WebDistillabilityMatchResult::MATCH_DETAILS) {
      ext_info.Set(kBECEPageType, "0"); // 0:详情页
    } else {
      ext_info.Set(kBECEPageType, "1"); // 1:正文页
      if (!match_info.distill_by_regex) {
        ext_info.Set(kBECEMatchPrevFeature, match_info.prev_feature);
        ext_info.Set(kBECEMatchNextFeature, match_info.next_feature);
        ext_info.Set(kBECEMatchCatalogFeature, match_info.catalog_feature);
      }
    }
    // 经分打点数据上报
    auto json = base::WriteJson(ext_info);
    if (json && render_frame) {
      render_frame->ReportDistillableResult(kBECEKernelNovelDistillableResult, json.value());
    }
  }
}

blink::WebDistillabilityMatchResult DetermineDistillableMatchResult(
    content::RenderFrame* render_frame,
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
  DocDistillableStatistics(render_frame, match_info, doc, cost_time_ms);
  LOG(INFO) << "[Distiller] match_result:" << (int)match_info.result;
  return match_info.result;
}
#endif