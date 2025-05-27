// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_CDM_COMMON_WIDEVINE_DRM_DELEGATE_OHOS_H_
#define COMPONENTS_CDM_COMMON_WIDEVINE_DRM_DELEGATE_OHOS_H_

#include <stdint.h>

#include "media/base/ohos/ohos_media_drm_bridge_delegate.h"

namespace cdm {

class WidevineDrmDelegateOHOS : public media::OHOSMediaDrmBridgeDelegate {
 public:
  WidevineDrmDelegateOHOS();

  WidevineDrmDelegateOHOS(const WidevineDrmDelegateOHOS&) = delete;
  WidevineDrmDelegateOHOS& operator=(const WidevineDrmDelegateOHOS&) = delete;

  ~WidevineDrmDelegateOHOS() override;

  const std::vector<uint8_t> GetUUID() const override;
  bool OnCreateSession(
      const media::EmeInitDataType init_data_type,
      const std::vector<uint8_t>& init_data,
      std::vector<uint8_t>* init_data_out,
      std::vector<std::string>* optional_parameters_out) override;
};

}  // namespace cdm

#endif  // COMPONENTS_CDM_COMMON_WIDEVINE_DRM_DELEGATE_OHOS_H_
