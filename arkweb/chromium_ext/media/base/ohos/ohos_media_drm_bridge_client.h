// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_BRIDGE_CLIENT_H_
#define MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_BRIDGE_CLIENT_H_

#include <stdint.h>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "media/base/media_export.h"

namespace media {

class OHOSMediaDrmBridgeClient;
class OHOSMediaDrmBridgeDelegate;

MEDIA_EXPORT void SetMediaDrmBridgeClient(
    OHOSMediaDrmBridgeClient* media_client);

#if defined(IS_MEDIA_IMPL)
OHOSMediaDrmBridgeClient* GetMediaDrmBridgeClient();
#endif

using UUID = std::vector<uint8_t>;

class MEDIA_EXPORT OHOSMediaDrmBridgeClient {
 public:
  typedef std::unordered_map<std::string, UUID> KeySystemUuidMap;

  OHOSMediaDrmBridgeClient();

  OHOSMediaDrmBridgeClient(const OHOSMediaDrmBridgeClient&) = delete;
  OHOSMediaDrmBridgeClient& operator=(const OHOSMediaDrmBridgeClient&) = delete;

  virtual ~OHOSMediaDrmBridgeClient();

  virtual void AddKeySystemUUIDMappings(KeySystemUuidMap* map);

  virtual media::OHOSMediaDrmBridgeDelegate* GetMediaDrmBridgeDelegate(
      const UUID& scheme_uuid);

 private:
  friend class KeySystemManager;
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_BRIDGE_CLIENT_H_
