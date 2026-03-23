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

distillability_service->NotifyDidMeaningfulLayout(doc.Url().GetString().Utf8());

// check host is in whitelist
blink::mojom::UrlHostDistillerInfoPtr distiller_info = nullptr;
distillability_service->GetHostDistillerInfo(GURL(doc.Url()).host(),
                                             &distiller_info);
if (!distiller_info || !distiller_info->is_distillable) {
  LOG(INFO) << "[Distiller] doc url is not in whitelist.";
  return;
}

blink::WebDistillabilityMatchResult match_type =
    DetermineDistillableMatchResult(doc, distiller_info);
mojom::PageDistillableInfoPtr page_info =
    mojom::PageDistillableInfo::New(mojom::PageType::kPageOthers,
                                    "",
                                    false,
                                    "");
if (match_type != blink::WebDistillabilityMatchResult::MATCH_NONE) {
  page_info->pageDistillable = true;
  page_info->distillablePageUrl = doc.Url().GetString().Utf8();
  page_info->title = doc.Title().Utf8();
  if (match_type == blink::WebDistillabilityMatchResult::MATCH_DETAILS) {
    page_info->pageType = mojom::PageType::kPageNovelDetails;
  } else {
    page_info->pageType = mojom::PageType::kPageNovelContent;
  }
}
distillability_service->NotifyIsPageDistillable(std::move(page_info),
                                                is_last,
                                                is_mobile_friendly);