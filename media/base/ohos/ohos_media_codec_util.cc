// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_media_codec_util.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include "ohos_adapter_helper.h"

#include "base/logging.h"

namespace media {
// static
CapabilityDataAdapter OHOSMediaCodecUtil::GetCodecCapability(
    const std::string& mime,
    bool isCodec) {
  return OhosAdapterHelper::GetInstance()
      .GetMediaCodecListAdapter()
      .GetCodecCapability(mime, isCodec);
}

}  // namespace media
