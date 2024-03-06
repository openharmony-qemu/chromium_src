// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_MEDIA_CODEC_UTIL_H_
#define MEDIA_BASE_OHOS_MEDIA_CODEC_UTIL_H_

#include "media_codec_adapter.h"
#include "ohos_adapter_helper.h"

namespace media {
using namespace OHOS::NWeb;
using namespace std;

struct CapabilityData {
  int32_t maxWidth = 0;
  int32_t maxHeight = 0;
  int32_t maxframeRate = 0;
};

class OHOSMediaCodecUtil {
 public:
  static CapabilityData GetCodecCapability(const std::string& mime,
                                           bool isCodec);
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_MEDIA_CODEC_UTIL_H_
