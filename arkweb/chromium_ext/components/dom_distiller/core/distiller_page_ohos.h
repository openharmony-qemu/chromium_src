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

#ifndef COMPONENTS_DOM_DISTILLER_CORE_DISTILLER_PAGE_OHOS_H_
#define COMPONENTS_DOM_DISTILLER_CORE_DISTILLER_PAGE_OHOS_H_

#include <string>

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "build/buildflag.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#if BUILDFLAG(ARKWEB_READER_MODE)
#include "arkweb/ohos_nweb/src/capi/nweb_extension_distill_item.h"
#include "base/json/json_reader.h"
#include "third_party/dom_distiller_js/dom_distiller.pb.h"
#include "url/gurl.h"
#endif  // ARKWEB_READER_MODE

namespace dom_distiller {

class TaskTracker;

void ModifyDistillerScriptOhos(std::string& script, bool is_distill_catalog);

#if BUILDFLAG(ARKWEB_READER_MODE)
std::string GetPathWithGURL(const GURL& gurl);
bool ParserXpathAndUpdateConfig(
    const std::string& xpath_content,
    dom_distiller::proto::DistillOptionsConfig* distill_config);
bool ParserJavascriptAndUpdateConfig(
    const std::string& javascript_content,
    dom_distiller::proto::DistillOptionsConfig* distill_config);
dom_distiller::proto::DomDistillerOptions GetCustomDomDistillerOptions(
    bool is_article,
    const GURL& gurl,
    const dom_distiller::proto::DomDistillerOptions& source_options);
bool SetCustomOptions(
    const GURL& gurl,
    dom_distiller::proto::HwCustomOptions* custom_options);
#endif  // ARKWEB_READER_MODE

class DistillerPageOhos {
 public:
  DistillerPageOhos();
  ~DistillerPageOhos() = default;
#if BUILDFLAG(ARKWEB_READER_MODE)
  bool DistillPageOhos(const GURL& url, proto::DomDistillerOptions& options);
  virtual void SetDistillOptions(const DistillOptions& options);
  virtual const DistillOptions* GetDistillOptions();
  virtual void AbortDistill();
#endif  // ARKWEB_READER_MODE

 private:
};
}  // namespace dom_distiller
#endif  // COMPONENTS_DOM_DISTILLER_CORE_DISTILLER_PAGE_OHOS_H_