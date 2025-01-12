// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_BRIDGE_DELEGATE_H_
#define MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_BRIDGE_DELEGATE_H_

#include <stdint.h>

#include <string>
#include <vector>

#include "media/base/eme_constants.h"
#include "media/base/media_export.h"
#include "media/base/ohos/ohos_media_drm_bridge_client.h"

namespace media {

class MEDIA_EXPORT OHOSMediaDrmBridgeDelegate {
 public:
  OHOSMediaDrmBridgeDelegate();

  OHOSMediaDrmBridgeDelegate(const OHOSMediaDrmBridgeDelegate&) = delete;
  OHOSMediaDrmBridgeDelegate& operator=(const OHOSMediaDrmBridgeDelegate&) =
      delete;

  virtual ~OHOSMediaDrmBridgeDelegate();

  virtual const UUID GetUUID() const = 0;

  virtual bool OnCreateSession(
      const EmeInitDataType init_data_type,
      const std::vector<uint8_t>& init_data,
      std::vector<uint8_t>* init_data_out,
      std::vector<std::string>* optional_parameters_out);
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_OHOS_MEDIA_DRM_BRIDGE_DELEGATE_H_
