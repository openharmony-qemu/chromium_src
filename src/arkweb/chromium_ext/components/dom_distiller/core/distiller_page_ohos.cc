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

#include "arkweb/chromium_ext/components/dom_distiller/core/distiller_page_ohos.h"

#if BUILDFLAG(ARKWEB_READER_MODE)
#include "content/public/browser/browser_context.h"
#include "arkweb/ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_reader_mode_config.h"
#include "arkweb/ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_reader_mode_config_utils.h"
#include "third_party/re2/src/re2/re2.h"
#include "third_party/re2/src/re2/stringpiece.h"
#include "third_party/dom_distiller_js/dom_distiller.pb.h"
#endif  // ARKWEB_READER_MODE

namespace dom_distiller {

#if BUILDFLAG(ARKWEB_READER_MODE)
const char* kExtractCatalogInfoPlaceholder = "$$CATALOGINFO";
const char* kDomDistillerJsPlaceholder = "$$DOMDISTILLERJS";

void ModifyDistillerScriptOhos(std::string& script, bool is_distill_catalog) {
  std::string extract_catalog = is_distill_catalog ? "true" : "false";
  size_t extract_catalog_offset = script.find(kExtractCatalogInfoPlaceholder);
  if (std::string::npos == extract_catalog_offset ||
    std::string::npos != script.find(kExtractCatalogInfoPlaceholder, extract_catalog_offset + 1)) {
      LOG(ERROR) << __func__ << " [Distiller] kExtractCatalogInfoPlaceholder invalid";
      return;
  }
  script =
      script.replace(extract_catalog_offset,
                     strlen(kExtractCatalogInfoPlaceholder), extract_catalog);

  auto dom_distiller_js =
    nweb_ex::AlloyBrowserReaderModeConfig::GetInstance()->GetDomDistillerJs();
  if (dom_distiller_js == nullptr || (*dom_distiller_js).empty()) {
    LOG(ERROR) << __func__ << " [Distiller] dom_distiller_js is empty";
    return;
  }

  size_t dom_distiller_js_offset = script.find(kDomDistillerJsPlaceholder);
  if (std::string::npos == dom_distiller_js_offset ||
    std::string::npos != script.find(kDomDistillerJsPlaceholder, dom_distiller_js_offset + 1)) {
    LOG(ERROR) << __func__ << " [Distiller] kDomDistillerJsPlaceholder invalid";
    return;
  }
  script = script.replace(dom_distiller_js_offset, strlen(kDomDistillerJsPlaceholder), *dom_distiller_js);
}

void ParserXpathAndSetConfig(
    const std::string& xpath_str,
    dom_distiller::proto::DomDistillerOptions& distiller_options) {
  if (xpath_str.empty()) {
    return;
  }

  absl::optional<base::Value> xpath_json = base::JSONReader::Read(xpath_str);
  if (!xpath_json || !xpath_json.has_value() || !xpath_json->is_dict()) {
    LOG(INFO) << "[Distiller] xpath json illegal.";
    return;
  }

  const base::Value::Dict& xpath_dict = xpath_json->GetDict();
  bool xpath_has_value = false;
  SET_XPATH_CONFIG("book_name", book_name, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("chapter_name", chapter_name, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("description", description, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("image_url", image_url, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("author", author, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("content", content, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("category", category, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("last_chapter_name", last_chapter_name, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("last_chapter_url", last_chapter_url, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("start_read_link", start_read_link, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("next_content_page", next_content_page, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("prev_content_page", prev_content_page, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("current_catalog_page", current_catalog_page, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("all_chapters", all_chapters, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("latest_chapters", latest_chapters, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("next_catalog_page", next_catalog_page, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("prev_catalog_page", prev_catalog_page, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("whole_catalog_page", whole_catalog_page, xpath_dict, distiller_options, xpath_has_value);
  SET_XPATH_CONFIG("all_catalog_pages", all_catalog_pages, xpath_dict, distiller_options, xpath_has_value);

  if (!xpath_has_value) {
    LOG(INFO) << "[Distiller] xpath invalid.";
    distiller_options.clear_xpath_config();
  }
}

void SetOptionsXpath(
    const GURL& gurl,
    dom_distiller::proto::DomDistillerOptions& distiller_options) {
  if (!gurl.is_valid()) {
    return;
  }
  std::string url_host = gurl.host();
  if (url_host.empty()) {
    return;
  }
  const nweb_ex::BrowserReaderModeWhitelistConfig* reader_white_list =
      nweb_ex::AlloyBrowserReaderModeConfig::GetInstance()
          ->QueryWhitelistConfig(url_host);
  if (!reader_white_list || reader_white_list->distill_config.empty()) {
    return;
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
    LOG(INFO) << __func__ << " [Distill] url can't find path or ref.";
    return;
  }
  re2::StringPiece input(path_ref_string);
  const std::vector<nweb_ex::BrowserReaderModeDistillConfig>& distill_config =
      reader_white_list->distill_config;
  for (auto& url_reg_xpath : distill_config) {
    re2::RE2 pattern(url_reg_xpath.url_regex);
    if (pattern.ok() && re2::RE2::FullMatch(input, pattern)) {
      ParserXpathAndSetConfig(url_reg_xpath.xpath, distiller_options);
      return;
    }
  }
}
#endif  // ARKWEB_READER_MODE

DistillerPageOhos::DistillerPageOhos() {}

#if BUILDFLAG(ARKWEB_READER_MODE)
void DistillerPageOhos::SetDistillOptions(const DistillOptions& options) {}

const DistillOptions* DistillerPageOhos::GetDistillOptions() {
  return nullptr;
}

void DistillerPageOhos::AbortDistill() {}

bool DistillerPageOhos::DistillPageOhos(
    const GURL& gurl,
    dom_distiller::proto::DomDistillerOptions& options) {
  bool is_distill_catalog = false;
  if (const auto* distill_options = GetDistillOptions()) {
    is_distill_catalog =
        distill_options->distill_type == DistillType::NOVEL_BOOKDETAIL;
  }

  options.clear_xpath_config();
  SetOptionsXpath(gurl, options);
  return is_distill_catalog;
}
#endif  // ARKWEB_READER_MODE
}  // namespace dom_distiller