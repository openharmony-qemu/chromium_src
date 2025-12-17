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
const double kByteRateD = 8.0;
}

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
void MultiBufferDataSource::VLOUpdateBufferSizes() {
  DVLOG(1) << __func__;
  if (!reader_) {
    return;
  }

  buffer_size_update_counter_ = kUpdateBufferSizeFrequency;
 
  // Use a default bit rate if unknown and clamp to prevent overflow.
  int64_t bitrate = std::clamp<int64_t>(bitrate_, 0, kMaxBitrate);
  if (bitrate == 0) {
    bitrate = kDefaultBitrate;
  }

  // Only scale the buffer window for playback rates greater than 1.0 in
  // magnitude and clamp to prevent overflow.
  double playback_rate = playback_rate_;
 
  playback_rate = std::max(playback_rate, 1.0);
  playback_rate = std::min(playback_rate, kMaxPlaybackRate);
 
  int64_t bytes_per_second = (bitrate / kByteRateD) * playback_rate;
 
  // We pin a few seconds of data behind the current reading position.
  int64_t pin_backward =
      std::clamp(keep_after_playback_seconds_.value() * bytes_per_second,
                  kMinBufferPreload, kMaxBufferPreload);
 
  // We always pin at least kDefaultPinSize ahead of the read position.
  // Normally, the extra space between preload_high and kDefaultPinSize will
  // not actually have any data in it, but if it does, we don't want to throw it
  // away right before we need it.
  int64_t pin_forward = kDefaultPinSize;
 
  // Note that the buffer size is advisory as only non-pinned data is allowed
  // to be thrown away. Most of the time we pin a region that is larger than
  // |buffer_size|, which only makes sense because most of the time, some of
  // the data in pinned region is not present in the cache.
  int64_t buffer_size = pin_forward + pin_backward;
 
  if (url_data_->FullyCached() ||
      (url_data_->length() != kPositionNotSpecified &&
       url_data_->length() < kDefaultPinSize)) {
    // We just make pin_forwards/backwards big enough to encompass the
    // whole file regardless of where we are, with some extra margins.
    pin_forward = std::max(pin_forward, url_data_->length() * 2);
    pin_backward = std::max(pin_backward, url_data_->length() * 2);
    buffer_size = url_data_->length();
  }
 
  reader_->SetMaxBuffer(buffer_size);
  reader_->SetPinRange(pin_backward, pin_forward);
 
  int64_t preload;
  int64_t preload_high;
 
  if (!media_has_played_) {
    preload_high = preload = 1 << url_data_->multibuffer()->block_size_shift();
  } else {
    preload_high = preload = min_cache_;
  }
 
  reader_->SetPreload(preload, preload_high);
}

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