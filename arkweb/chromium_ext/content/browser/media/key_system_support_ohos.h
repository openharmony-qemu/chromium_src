// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_MEDIA_KEY_SYSTEM_SUPPORT_OHOS_H_
#define CONTENT_BROWSER_MEDIA_KEY_SYSTEM_SUPPORT_OHOS_H_

#include <string>

#include "content/common/content_export.h"
#include "content/public/common/cdm_info.h"
#include "media/base/cdm_capability.h"

namespace content {

void CONTENT_EXPORT
GetOHOSCdmCapability(const std::string& key_system,
                     CdmInfo::Robustness robustness,
                     media::CdmCapabilityCB cdm_capability_cb);

}  // namespace content

#endif  // CONTENT_BROWSER_MEDIA_KEY_SYSTEM_SUPPORT_OHOS_H_
