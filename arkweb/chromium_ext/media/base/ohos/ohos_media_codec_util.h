// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_MEDIA_CODEC_UTIL_H_
#define MEDIA_BASE_OHOS_MEDIA_CODEC_UTIL_H_

#include "third_party/ohos_ndk/includes/ohos_adapter/media_codec_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "base/compiler_specific.h"
#include "media/base/audio_codecs.h"
#include "media/base/sample_format.h"
#include "media/base/video_codecs.h"

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
  static std::string CodecToOHOSMimeType(AudioCodec codec);
  static std::string CodecToOHOSMimeType(AudioCodec codec,
                                            SampleFormat sample_format);
  static std::string CodecToOHOSMimeType(VideoCodec codec);
  static bool IsPassthroughAudioFormat(AudioCodec codec);
  static bool CanDecode(VideoCodec codec, bool is_secure);
  static bool CanDecode(AudioCodec codec);
};

}  // namespace media

#endif  // MEDIA_BASE_OHOS_MEDIA_CODEC_UTIL_H_
