// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_drm_bridge_client.h"

#include "base/check.h"
#include "base/logging.h"

namespace media {

static OHOSMediaDrmBridgeClient* g_media_drm_bridge_client = nullptr;

void SetMediaDrmBridgeClient(OHOSMediaDrmBridgeClient* media_client) {
  DCHECK(!g_media_drm_bridge_client);
  g_media_drm_bridge_client = media_client;
}

OHOSMediaDrmBridgeClient* GetMediaDrmBridgeClient() {
  return g_media_drm_bridge_client;
}

OHOSMediaDrmBridgeClient::OHOSMediaDrmBridgeClient() {}

OHOSMediaDrmBridgeClient::~OHOSMediaDrmBridgeClient() {}

void OHOSMediaDrmBridgeClient::AddKeySystemUUIDMappings(KeySystemUuidMap* map) {
}

media::OHOSMediaDrmBridgeDelegate*
OHOSMediaDrmBridgeClient::GetMediaDrmBridgeDelegate(
    const std::vector<uint8_t>& scheme_uuid) {
  return nullptr;
}

}  // namespace media
