/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

namespace blink {

namespace {
const uint16_t kRate = 1000;
const uint16_t kDefaultByteRate = 250;
const uint16_t kByteUnit = 8;
}

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
void MultiBufferDataSource::SetVLOParams(uint16_t preload,
                                         uint16_t max,
                                         uint16_t min,
                                         uint16_t bitrate,
                                         uint16_t moov_size,
                                         std::string video_id) {
  video_id_ = video_id;
  byte_rate_ = bitrate / kByteUnit;
  if (byte_rate_ <= 0) {
    byte_rate_ = kDefaultByteRate;
    LOG(INFO) << "VideoOpt bitrate is invalid, value=" << bitrate;
  }
 
  max_cache_ = byte_rate_ * max * kRate;
  min_cache_ = byte_rate_ * min * kRate;
  preload_cache_ = (byte_rate_ * preload + moov_size) * kRate;
  LOG(INFO) << "VideoOpt video_id_=" << video_id_ << "; byte_rate_=" << byte_rate_
            << "; preload_cache_=" << preload_cache_ << "; moov=" << moov_size
            << "; max=" << max << "; min=" << min << "; preload=" << preload
            << "; max_cache_=" << max_cache_ << "; min_cache_=" << min_cache_;
  UpdateBufferSizes();
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

}