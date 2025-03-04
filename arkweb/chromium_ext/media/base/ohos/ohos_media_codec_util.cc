// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_media_codec_util.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "base/logging.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace media {
// static
CapabilityData OHOSMediaCodecUtil::GetCodecCapability(const std::string& mime,
                                                      bool isCodec) {
  std::shared_ptr<CapabilityDataAdapter> data =
      OhosAdapterHelper::GetInstance()
          .GetMediaCodecListAdapter()
          .GetCodecCapability(mime, isCodec);

  CapabilityData result;

  if (!data) {
    return result;
  }

  result.maxWidth = data->GetMaxWidth();
  result.maxHeight = data->GetMaxHeight();
  result.maxframeRate = data->GetMaxframeRate();
  return result;
}

}  // namespace media
