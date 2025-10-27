/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef CEF_LIBCEF_BROWSER_CONTENT_SETTINGS_OH_HOST_CONTENT_SETTINGS_OBSERVER_FACTORY_H_
#define CEF_LIBCEF_BROWSER_CONTENT_SETTINGS_OH_HOST_CONTENT_SETTINGS_OBSERVER_FACTORY_H_

#include "base/memory/singleton.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"
#include "ohos_cef_ext/libcef/browser/content_settings/oh_host_content_settings_observer.h"

class KeyedService;

namespace contenet {
class BrowserContext;
}

class OhHostContentSettingsObserverFactory
    : public BrowserContextKeyedServiceFactory {
 public:
  // Returns the OhHostContentSettingsObserver that supports BrowserContext for
  // |browser_context|.
  static OhHostContentSettingsObserver* GetForContext(
      content::BrowserContext* browser_context);

  static OhHostContentSettingsObserverFactory* GetInstance();

  void RegisterObserver(content::BrowserContext* context);

  // DISALLOW_COPY_AND_ASSIGN
  OhHostContentSettingsObserverFactory(
      const OhHostContentSettingsObserverFactory&) = delete;
  OhHostContentSettingsObserverFactory& operator=(
      const OhHostContentSettingsObserverFactory&) = delete;

 private:
  friend struct base::DefaultSingletonTraits<
      OhHostContentSettingsObserverFactory>;

  OhHostContentSettingsObserverFactory();
  ~OhHostContentSettingsObserverFactory() override;

  // BrowserContextKeyedServiceFactory implementation:
  KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* profile) const override;
  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;
};

#endif  // CEF_LIBCEF_BROWSER_CONTENT_SETTINGS_OH_HOST_CONTENT_SETTINGS_OBSERVER_FACTORY_H_
