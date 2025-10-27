// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_drm_bridge_delegate.h"

#include "base/check.h"
#include "base/logging.h"

namespace media {

OHOSMediaDrmBridgeDelegate::OHOSMediaDrmBridgeDelegate() {}

OHOSMediaDrmBridgeDelegate::~OHOSMediaDrmBridgeDelegate() {}

bool OHOSMediaDrmBridgeDelegate::OnCreateSession(
    const EmeInitDataType init_data_type,
    const std::vector<uint8_t>& init_data,
    std::vector<uint8_t>* init_data_out,
    std::vector<std::string>* optional_parameters_out) {
  LOG(INFO) << "[DRM]" << __func__;
  DCHECK(init_data_out->empty());
  DCHECK(optional_parameters_out->empty());
  LOG(INFO) << "[DRM]" << __func__;
  return true;
}

}  // namespace media
