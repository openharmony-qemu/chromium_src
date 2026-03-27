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

#if BUILDFLAG(ARKWEB_READER_MODE)
#define SET_DISTILL_META_CONFIG(config_type, distill_meta_param, func_name,          \
                                meta_dict, distill_config, has_value)                \
  const std::string* value_##func_name = (meta_dict).FindString(distill_meta_param); \
  if (value_##func_name && !value_##func_name->empty()) {                            \
    (distill_config)->mutable_##config_type()->set_##func_name(*value_##func_name);  \
    has_value = true;                                                                \
  }
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

namespace {
void ParserXpathAndSetConfig(
    const std::string& xpath_str, dom_distiller::proto::DomDistillerOptions& distiller_options) {
  if (xpath_str.empty()) {
    return;
  }

  absl::optional<base::Value> xpath_json = base::JSONReader::Read(xpath_str);
  if (!xpath_json || !xpath_json.has_value() || !xpath_json->is_dict()) {
    LOG(INFO) << "[Distiller] xpath json illegal.";
    return;
  }

  const base::Value::Dict& dict = xpath_json->GetDict();
  bool has_value = false;
  SET_DISTILL_META_CONFIG(xpath_config, "book_name", book_name, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "chapter_name", chapter_name, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "description", description, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "image_url", image_url, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "author", author, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "content", content, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "category", category, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "last_chapter_name", last_chapter_name, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "last_chapter_url", last_chapter_url, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "start_read_link", start_read_link, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "next_content_page", next_content_page, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "prev_content_page", prev_content_page, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "current_catalog_page", current_catalog_page,
                          dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "all_chapters", all_chapters, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "latest_chapters", latest_chapters, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "next_catalog_page", next_catalog_page, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "prev_catalog_page", prev_catalog_page, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "whole_catalog_page", whole_catalog_page, dict, &distiller_options, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "all_catalog_pages", all_catalog_pages, dict, &distiller_options, has_value);
  if (!has_value) {
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
  std::optional<nweb_ex::BrowserReaderModeWhitelistConfig> reader_white_list =
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

std::string GetPathWithGURL(const GURL& gurl) {
  // /path#fragment
  std::string path_ref_string;
  if (gurl.has_path()) {
    path_ref_string.append(gurl.path());
  }
  if (gurl.has_ref()) {
    path_ref_string.append("#");
    path_ref_string.append(gurl.ref());
  }
  return path_ref_string;
}

bool ParserXpathAndUpdateConfig(
    const std::string& xpath_content,
    dom_distiller::proto::DistillOptionsConfig* distill_config) {
  if (xpath_content.empty()) {
    LOG(WARNING) << "[Distiller] xpath_content empty.";
    return false;
  }

  absl::optional<base::Value> xpath_json = base::JSONReader::Read(xpath_content);
  if (!xpath_json || !xpath_json.has_value() || !xpath_json->is_dict()) {
    LOG(WARNING) << "[Distiller] distill xpath config json illegal.";
    return false;
  }

  const base::Value::Dict& dict = xpath_json->GetDict();
  bool has_value = false;
  SET_DISTILL_META_CONFIG(xpath_config, "book_name", book_name, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "chapter_name", chapter_name, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "description", description, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "image_url", image_url, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "author", author, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "content", content, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "category", category, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "last_chapter_name", last_chapter_name, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "last_chapter_url", last_chapter_url, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "start_read_link", start_read_link, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "next_content_page", next_content_page, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "prev_content_page", prev_content_page, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "current_catalog_page", current_catalog_page, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "all_chapters", all_chapters, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "latest_chapters", latest_chapters, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "next_catalog_page", next_catalog_page, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "prev_catalog_page", prev_catalog_page, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "whole_catalog_page", whole_catalog_page, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(xpath_config, "all_catalog_pages", all_catalog_pages, dict, distill_config, has_value);

  if (!has_value) {
    LOG(WARNING) << "[Distiller] not valid xpath_content: " << xpath_content;
    distill_config->clear_xpath_config();
  }
  return has_value;
}

bool ParserJavascriptAndUpdateConfig(
    const std::string& javascript_content,
    dom_distiller::proto::DistillOptionsConfig* distill_config) {
  if (javascript_content.empty()) {
    LOG(WARNING) << "[Distiller] javascript_content empty.";
    return false;
  }

  absl::optional<base::Value> javascript_json = base::JSONReader::Read(javascript_content);
  if (!javascript_json || !javascript_json.has_value() || !javascript_json->is_dict()) {
    LOG(WARNING) << "[Distiller] distill javascript config json illegal.";
    return false;
  }

  const base::Value::Dict& dict = javascript_json->GetDict();
  bool has_value = false;
  SET_DISTILL_META_CONFIG(javascript_config, "book_name", book_name, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "chapter_name", chapter_name, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "description", description, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "image_url", image_url, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "author", author, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "content", content, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "category", category, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "last_chapter_name", last_chapter_name, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "last_chapter_url", last_chapter_url, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "start_read_link", start_read_link, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "next_content_page", next_content_page, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "prev_content_page", prev_content_page, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "current_catalog_page", current_catalog_page,
                          dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "all_chapters", all_chapters, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "latest_chapters", latest_chapters, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "next_catalog_page", next_catalog_page, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "prev_catalog_page", prev_catalog_page, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "whole_catalog_page", whole_catalog_page,
                          dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "all_catalog_pages", all_catalog_pages, dict, distill_config, has_value);
  SET_DISTILL_META_CONFIG(javascript_config, "reserved", reserved, dict, distill_config, has_value);

  if (!has_value) {
    LOG(WARNING) << "[Distiller] not valid javascript_content: " << javascript_content;
    distill_config->clear_javascript_config();
  }
  return has_value;
}
}  // namespace

dom_distiller::proto::DomDistillerOptions GetCustomDomDistillerOptions(
    bool is_article, const GURL& gurl, const dom_distiller::proto::DomDistillerOptions& source_options) {
  if (is_article) {
    return source_options;
  }
  dom_distiller::proto::DomDistillerOptions result_options = source_options;
  const auto* config_data = nweb_ex::AlloyBrowserReaderModeConfig::GetInstance()->GetReaderModeConfigData();
  if (!config_data) {
    return result_options;
  }

  if (!nweb_ex::AlloyBrowserReaderModeConfig::GetInstance()->isV2Config()) {
    SetOptionsXpath(gurl, result_options);
    return result_options;
  }

  auto* custom_options = result_options.mutable_custom_options();
  bool custom_options_has_value = false;
  std::string dom_distiller_config_reserved = config_data->globalConfig.domDistillerConfig.reserved;
  if (!dom_distiller_config_reserved.empty()) {
    custom_options->mutable_dom_distiller_config()->set_reserved(dom_distiller_config_reserved);
    custom_options_has_value = true;
  }
  custom_options_has_value |= SetCustomOptions(gurl, custom_options);
  if (!custom_options_has_value) {
    result_options.clear_custom_options();
  }
  return result_options;
}

bool SetCustomOptions(const GURL& gurl, dom_distiller::proto::HwCustomOptions* custom_options) {
  bool custom_options_has_value = false;
  std::string url_path = GetPathWithGURL(gurl);
  if (url_path.empty()) {
    return custom_options_has_value;
  }
  re2::StringPiece input(url_path);
  auto* cust_config = nweb_ex::AlloyBrowserReaderModeConfig::GetInstance()->QueryHostCustConfig(gurl.host());
  if (!cust_config) {
    return custom_options_has_value;
  }
  if (cust_config->custMetaConfig.isDynamicJsWebSite) {
    auto* cust_meta_config = custom_options->mutable_cust_meta_config();
    cust_meta_config->set_is_dynamic_js_web_site(cust_config->custMetaConfig.isDynamicJsWebSite);
    cust_meta_config->set_dom_mutation_duration_ms(cust_config->custMetaConfig.domMutationDurationMs);
    cust_meta_config->set_dom_text_change_count(cust_config->custMetaConfig.domTextChangeCount);
    custom_options_has_value = true;
  }
  if (!cust_config->custMetaConfig.reserved.empty()) {
    auto* cust_meta_config = custom_options->mutable_cust_meta_config();
    cust_meta_config->set_reserved(cust_config->custMetaConfig.reserved);
    custom_options_has_value = true;
  }
  for (auto& url_reg : cust_config->distillConfig) {
    re2::RE2 pattern(url_reg.urlReg);
    if (!pattern.ok() || !re2::RE2::FullMatch(input, pattern)) {
      LOG(WARNING) << __func__ << " [Distiller] not match reg";
      continue;
    }
    LOG(INFO) << __func__ << " [Distiller] match reg";
    custom_options_has_value |= ParserXpathAndUpdateConfig(url_reg.xpath, custom_options->mutable_distill_config());
    custom_options_has_value |= ParserJavascriptAndUpdateConfig(url_reg.javascript,
                                                                custom_options->mutable_distill_config());
    if (!url_reg.reserved.empty()) {
      custom_options->mutable_distill_config()->set_reserved(url_reg.reserved);
      custom_options_has_value = true;
    }
    break;
  }
  return custom_options_has_value;
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
  const auto* distill_options = GetDistillOptions();
  if (distill_options) {
    is_distill_catalog =
        distill_options->distill_type == DistillType::NOVEL_BOOKDETAIL;
    if (!distill_options->distill_ext_info.empty()) {
      options.set_distill_ext_info(distill_options->distill_ext_info);
    }
  }

  return is_distill_catalog;
}
#endif  // ARKWEB_READER_MODE
}  // namespace dom_distiller