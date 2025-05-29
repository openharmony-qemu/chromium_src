// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_MEDIA_OHOS_CDM_MEDIA_DRM_ORIGIN_ID_MANAGER_FACTORY_H_
#define CHROME_BROWSER_MEDIA_OHOS_CDM_MEDIA_DRM_ORIGIN_ID_MANAGER_FACTORY_H_

#include "base/memory/singleton.h"
#include "chrome/browser/profiles/profile_keyed_service_factory.h"

class MediaDrmOriginIdManager;
class Profile;

class MediaDrmOriginIdManagerFactory : public ProfileKeyedServiceFactory {
 public:
  static MediaDrmOriginIdManager* GetForProfile(Profile* profile);

  static MediaDrmOriginIdManagerFactory* GetInstance();

 private:
  friend struct base::DefaultSingletonTraits<MediaDrmOriginIdManagerFactory>;

  MediaDrmOriginIdManagerFactory();

  ~MediaDrmOriginIdManagerFactory() override;

  KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* context) const override;

  bool ServiceIsCreatedWithBrowserContext() const override;
};

#endif  // CHROME_BROWSER_MEDIA_OHOS_CDM_MEDIA_DRM_ORIGIN_ID_MANAGER_FACTORY_H_
