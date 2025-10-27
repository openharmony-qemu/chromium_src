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

#include "ohos_cef_ext/libcef/browser/content_settings/oh_host_content_settings_observer.h"

#include "content/public/browser/storage_partition.h"
#include "content/public/renderer/render_thread.h"
#include "services/network/cookie_manager.h"

OhHostContentSettingsObserver::~OhHostContentSettingsObserver() {
  if (!context_) {
    return;
  }

  if (HostContentSettingsMapFactory::GetForProfile(context_)) {
    HostContentSettingsMapFactory::GetForProfile(context_)->RemoveObserver(
        this);
  }
}

void OhHostContentSettingsObserver::OnContentSettingChanged(
    const ContentSettingsPattern& primary_pattern,
    const ContentSettingsPattern& secondary_pattern,
    ContentSettingsType content_type) {
  if (!context_) {
    return;
  }

  if (content_type == ContentSettingsType::COOKIES) {
    ContentSettingsForOneType cookies_settings =
        HostContentSettingsMapFactory::GetForProfile(context_)
            ->GetSettingsForOneType(ContentSettingsType::COOKIES);
    context_->ForEachLoadedStoragePartition(
        [&](content::StoragePartition* storage_partition) {
          storage_partition->GetCookieManagerForBrowserProcess()
              ->SetContentSettings(ContentSettingsType::COOKIES,
                                   std::move(cookies_settings),
                                   base::NullCallback());
        });
  }
}
 