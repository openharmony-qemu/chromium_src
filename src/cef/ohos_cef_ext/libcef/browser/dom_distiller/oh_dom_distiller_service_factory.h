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

#ifndef CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_DOM_DISTILLER_SERVICE_FACTORY_H_
#define CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_DOM_DISTILLER_SERVICE_FACTORY_H_

#include "base/no_destructor.h"
#include "components/dom_distiller/core/distilled_page_prefs.h"
#include "components/dom_distiller/core/distiller_ui_handle.h"
#include "components/dom_distiller/core/dom_distiller_service.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"
#include "components/keyed_service/core/keyed_service.h"

namespace content {
class BrowserContext;
}  // namespace content

namespace oh_dom_distiller {

// A simple wrapper for DomDistillerService to expose it as a
// KeyedService.
class OhDomDistillerContextKeyedService : public KeyedService,
                                        public dom_distiller::DomDistillerService {
 public:
  OhDomDistillerContextKeyedService(
      std::unique_ptr<dom_distiller::DistillerFactory> distiller_factory,
      std::unique_ptr<dom_distiller::DistillerPageFactory>
          distiller_page_factory,
      std::unique_ptr<dom_distiller::DistilledPagePrefs> distilled_page_prefs,
      std::unique_ptr<dom_distiller::DistillerUIHandle> distiller_ui_handle);

  OhDomDistillerContextKeyedService(const OhDomDistillerContextKeyedService&) =
      delete;
  OhDomDistillerContextKeyedService& operator=(
      const OhDomDistillerContextKeyedService&) = delete;

  ~OhDomDistillerContextKeyedService() override = default;
};

class OhDomDistillerServiceFactory : public BrowserContextKeyedServiceFactory {
 public:
  static OhDomDistillerServiceFactory* GetInstance();
  static OhDomDistillerContextKeyedService* GetForBrowserContext(
      content::BrowserContext* context);

 private:
  friend base::NoDestructor<OhDomDistillerServiceFactory>;

  OhDomDistillerServiceFactory();
  ~OhDomDistillerServiceFactory() override;

  std::unique_ptr<KeyedService> BuildServiceInstanceForBrowserContext(
      content::BrowserContext* context) const override;
};

} // namespace oh_dom_distiller

#endif  // CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_DOM_DISTILLER_SERVICE_FACTORY_H_
