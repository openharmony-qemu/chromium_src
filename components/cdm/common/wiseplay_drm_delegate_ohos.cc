// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/cdm/common/wiseplay_drm_delegate_ohos.h"

#include "base/logging.h"
#include "third_party/wiseplay/cdm/wiseplay_cdm_common.h"
#include "media/cdm/cenc_utils.h"

namespace cdm {

WiseplayDrmDelegateOhos::WiseplayDrmDelegateOhos() {}

WiseplayDrmDelegateOhos::~WiseplayDrmDelegateOhos() {}

const std::vector<uint8_t> WiseplayDrmDelegateOhos::GetUUID() const {
  return std::vector<uint8_t>(kWiseplayUuid,
                              kWiseplayUuid + std::size(kWiseplayUuid));
}

bool WiseplayDrmDelegateOhos::OnCreateSession(
    const media::EmeInitDataType init_data_type,
    const std::vector<uint8_t>& init_data,
    std::vector<uint8_t>* init_data_out,
    std::vector<std::string>* /* optional_parameters_out */) {
  if (init_data_type != media::EmeInitDataType::CENC) {
    return true;
  }
  return media::GetPsshData(init_data, GetUUID(), init_data_out);
}

}  // namespace cdm