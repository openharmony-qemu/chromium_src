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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_SEGMENTATION_MULTIBUFFER_DATA_PROVIDER_H
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_SEGMENTATION_MULTIBUFFER_DATA_PROVIDER_H

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/video_common_data_provider.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace blink {
class WebAssociatedURLLoader;

class SegmentationMultiBufferDataProvider : public VideoCommonDataProvider {
 public:
  SegmentationMultiBufferDataProvider(
      UrlData* url_data,
      MultiBufferBlockId pos,
      int32_t preload_size,
      int32_t request_size,
      uint16_t byte_rate,
      std::string id,
      bool is_client_audio_element,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner);
  ~SegmentationMultiBufferDataProvider() override;

  void Start() override;

  void NotifyLengthSet() override;

  // MultiBuffer::DataProvider implementation
  bool Available() const override;
  int64_t AvailableBytes() const override;
  scoped_refptr<media::DataBuffer> Read() override;
  void SetDeferred(bool defer) override;

 private:
  void StartNextIfNeeded();

  void StartRequest(int64_t start);

  // Returns the block for a particular byte position.
  MultiBufferBlockId block(int64_t byte_pos) const {
    return byte_pos >> block_size_shift();
  }

  // Returns the block for a particular byte position, rounding up.
  MultiBufferBlockId block_ceil(int64_t byte_pos) const {
    return block(byte_pos + (1LL << block_size_shift()) - 1);
  }

  int32_t preload_size_ = -1;
  int32_t request_size_ = -1;
  int32_t range_size_ = -1;
  int32_t num_;

  int16_t cur_index_ = -1;
  int16_t read_index_ = 0;
  int64_t cur_end_ = 0;

  std::vector<std::pair<int64_t, int64_t>> range_vector_;
  std::vector<std::unique_ptr<VideoRangeURLLoaderClient>> client_vector_;
  std::vector<std::list<scoped_refptr<media::DataBuffer>>> fifo_vector_;

  base::WeakPtrFactory<SegmentationMultiBufferDataProvider> weak_factory_{this};
};

}  // namespace blink

#endif // THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIA_SEGMENTATION_MULTIBUFFER_DATA_PROVIDER_H

