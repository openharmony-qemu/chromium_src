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

#ifndef CEF_LIBCEF_BROWSER_CONTENT_SETTINGS_OH_HOST_CONTENT_SETTINGS_OBSERVER_H_
#define CEF_LIBCEF_BROWSER_CONTENT_SETTINGS_OH_HOST_CONTENT_SETTINGS_OBSERVER_H_

#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "components/content_settings/core/browser/content_settings_observer.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/keyed_service/core/keyed_service.h"
#include "content/public/browser/browser_context.h"
#include "base/memory/raw_ptr.h"

class OhHostContentSettingsObserver : public KeyedService,
                                      public content_settings::Observer {
 public:
  explicit OhHostContentSettingsObserver(content::BrowserContext* context)
      : context_(context) {
    HostContentSettingsMapFactory::GetForProfile(context)->AddObserver(this);
  }

  ~OhHostContentSettingsObserver() override;

  // Overridden from content_settings::Observer:
  void OnContentSettingChanged(const ContentSettingsPattern& primary_pattern,
                               const ContentSettingsPattern& secondary_pattern,
                               ContentSettingsType content_type) override;

  // DISALLOW_COPY_AND_ASSIGN
  OhHostContentSettingsObserver(const OhHostContentSettingsObserver&) = delete;
  OhHostContentSettingsObserver& operator=(
      const OhHostContentSettingsObserver&) = delete;

 private:
  raw_ptr<content::BrowserContext> context_;
};

#endif  // CEF_LIBCEF_BROWSER_CONTENT_SETTINGS_OH_HOST_CONTENT_SETTINGS_OBSERVER_H_
