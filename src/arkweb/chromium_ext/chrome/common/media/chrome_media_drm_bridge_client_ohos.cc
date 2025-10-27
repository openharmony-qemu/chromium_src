// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/media/chrome_media_drm_bridge_client_ohos.h"

#include "base/logging.h"

ChromeMediaDrmBridgeClientOHOS::ChromeMediaDrmBridgeClientOHOS() {}

ChromeMediaDrmBridgeClientOHOS::~ChromeMediaDrmBridgeClientOHOS() {}

void ChromeMediaDrmBridgeClientOHOS::AddKeySystemUUIDMappings(
    KeySystemUuidMap* map) {}

media::OHOSMediaDrmBridgeDelegate* ChromeMediaDrmBridgeClientOHOS::GetMediaDrmBridgeDelegate(
    const media::UUID& scheme_uuid) {
  LOG(INFO) << "[DRM]" << __func__;
  if (scheme_uuid == widevine_delegate_.GetUUID()) {
    return &widevine_delegate_;
  }
  return nullptr;
}
