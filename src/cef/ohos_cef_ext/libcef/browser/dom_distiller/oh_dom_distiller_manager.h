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

#ifndef CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_DOM_DISTILLER_MANAGER_H_
#define CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_DOM_DISTILLER_MANAGER_H_

#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/no_destructor.h"
#include "components/dom_distiller/core/dom_distiller_service.h"
#include "capi/nweb_extension_distill_item.h"

namespace content {
class WebContents;
}  // namespace content

namespace dom_distiller {
class DomDistillerService;
}  // namespace dom_distiller

namespace oh_dom_distiller {

class OhDomDistillerManager {
 public:
  static OhDomDistillerManager* GetInstance();

  OhDomDistillerManager(const OhDomDistillerManager&) = delete;
  OhDomDistillerManager& operator=(const OhDomDistillerManager&) = delete;

  // Creates a new WebContents and navigates it to view the URL of the current
  // page, while in the background starts distilling the current page. This
  // method takes ownership over the old WebContents after swapping in the new
  // one.
  void DistillCurrentPageAndView(content::WebContents* old_web_contents);

  // Starts distillation in the |source_web_contents|. The viewer needs to be
  // created separately.
  void DistillCurrentPage(
      content::WebContents* source_web_contents,
      const DistillOptions& distill_options,
      base::OnceCallback<void(const std::string& content)> callback);

  // Starts distillation in the |source_web_contents| while navigating the
  // |destination_web_contents| to view the distilled content. This does not
  // take ownership of any WebContents.
  void DistillAndView(content::WebContents* source_web_contents,
                      const DistillOptions& distill_options,
                      content::WebContents* destination_web_contents);

  void AbortDistill(content::WebContents* source_web_contents);

  dom_distiller::DomDistillerService* GetDomDistillerService(
      content::WebContents*);

 private:
  friend class base::NoDestructor<OhDomDistillerManager>;

  OhDomDistillerManager();
  ~OhDomDistillerManager();

  raw_ptr<dom_distiller::DomDistillerService> dom_distiller_service_;
};

} // namespace oh_dom_distiller

#endif  // CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_DOM_DISTILLER_MANAGER_H_
