// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_COMMON_MEDIA_CHROME_MEDIA_DRM_BRIDGE_CLIENT_OHOS_H_
#define CHROME_COMMON_MEDIA_CHROME_MEDIA_DRM_BRIDGE_CLIENT_OHOS_H_

#include "components/cdm/common/widevine_drm_delegate_ohos.h"
#include "media/base/ohos/ohos_media_drm_bridge_client.h"

class ChromeMediaDrmBridgeClientOHOS : public media::OHOSMediaDrmBridgeClient {
 public:
  explicit ChromeMediaDrmBridgeClientOHOS();

  ChromeMediaDrmBridgeClientOHOS(const ChromeMediaDrmBridgeClientOHOS&) = delete;
  ChromeMediaDrmBridgeClientOHOS& operator=(
      const ChromeMediaDrmBridgeClientOHOS&) = delete;

  ~ChromeMediaDrmBridgeClientOHOS() override;

 private:
  void AddKeySystemUUIDMappings(KeySystemUuidMap* map) override;
  media::OHOSMediaDrmBridgeDelegate* GetMediaDrmBridgeDelegate(
      const media::UUID& scheme_uuid) override;

  std::vector<std::string> key_system_uuid_mappings_;
  cdm::WidevineDrmDelegateOHOS widevine_delegate_;
};

#endif  // CHROME_COMMON_MEDIA_CHROME_MEDIA_DRM_BRIDGE_CLIENT_OHOS_H_
