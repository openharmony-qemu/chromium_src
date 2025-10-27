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

#include "ohos_cef_ext/libcef/browser/content_settings/oh_host_content_settings_observer_factory.h"

#include "components/keyed_service/content/browser_context_dependency_manager.h"

OhHostContentSettingsObserver*
OhHostContentSettingsObserverFactory::GetForContext(
    content::BrowserContext* browser_context) {
  return static_cast<OhHostContentSettingsObserver*>(
      GetInstance()->GetServiceForBrowserContext(browser_context, true));
}

OhHostContentSettingsObserverFactory*
OhHostContentSettingsObserverFactory::GetInstance() {
  return base::Singleton<OhHostContentSettingsObserverFactory>::get();
}

OhHostContentSettingsObserverFactory::OhHostContentSettingsObserverFactory()
    : BrowserContextKeyedServiceFactory(
          "OhHostContentSettingsObserver",
          BrowserContextDependencyManager::GetInstance()) {}

OhHostContentSettingsObserverFactory::~OhHostContentSettingsObserverFactory() {}

KeyedService* OhHostContentSettingsObserverFactory::BuildServiceInstanceFor(
    content::BrowserContext* context) const {
  return new OhHostContentSettingsObserver(context);
}

content::BrowserContext*
OhHostContentSettingsObserverFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return context;
}

void OhHostContentSettingsObserverFactory::RegisterObserver(
    content::BrowserContext* context) {
  new OhHostContentSettingsObserver(context);
}
 