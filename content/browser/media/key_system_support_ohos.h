// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_MEDIA_KEY_SYSTEM_SUPPORT_OHOS_H_
#define CONTENT_BROWSER_MEDIA_KEY_SYSTEM_SUPPORT_OHOS_H_

#include <string>

#include "base/functional/callback.h"
#include "content/public/common/cdm_info.h"
#include "media/cdm/cdm_capability.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace content {

void GetOHOSCdmCapability(const std::string& key_system,
                          CdmInfo::Robustness robustness,
                          media::CdmCapabilityCB cdm_capability_cb);

}  // namespace content

#endif  // CONTENT_BROWSER_MEDIA_KEY_SYSTEM_SUPPORT_OHOS_H_
