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

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/segmentation_multibuffer_data_provider.h"

#include <utility>
#include <sstream>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "base/bits.h"
#include "base/functional/callback_helpers.h"
#include "base/location.h"
#include "base/metrics/histogram_macros.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/task/single_thread_task_runner.h"
#include "net/http/http_byte_range.h"
#include "net/http/http_request_headers.h"
#include "services/network/public/cpp/cors/cors.h"
#include "third_party/blink/public/mojom/fetch/fetch_api_request.mojom.h"
#include "third_party/blink/public/platform/web_network_state_notifier.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/platform/web_url_error.h"
#include "third_party/blink/public/platform/web_url_response.h"
#include "third_party/blink/public/web/web_associated_url_loader.h"
#include "third_party/blink/renderer/platform/media/cache_util.h"
#include "third_party/blink/renderer/platform/media/resource_fetch_context.h"
#include "third_party/blink/renderer/platform/media/url_index.h"
#include "third_party/bounds_checking_function/include/securec.h"

namespace blink {
// bitRate < 1000kb/s, slice size=256KB(8 blocks)
// 1000kb/s <= bitRate < 2000kb/s, slice size=512KB(16 blocks)
// bitRate > 2000kb/s, slice size=768KB(24 blocks)
const uint16_t low_byte_rate = 125;
const uint16_t high_byte_rate = 250;
const int32_t low_range_size = 8;
const int32_t mid_range_size = 16;
const int32_t high_range_size = 24;

SegmentationMultiBufferDataProvider::SegmentationMultiBufferDataProvider(
    UrlData* url_data,
    MultiBufferBlockId pos,
    int32_t preload_size,
    int32_t request_size,
    uint16_t byte_rate,
    std::string id,
    bool is_client_audio_element,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner)
    : VideoCommonDataProvider(url_data,
                           pos,
                           id,
                           is_client_audio_element,
                           std::move(task_runner)) {
  preload_size_ = preload_size;

  if (byte_rate < low_byte_rate) {
    range_size_ = low_range_size;
  } else if (byte_rate >= high_byte_rate) {
    range_size_ = high_range_size;
  } else {
    range_size_ = mid_range_size;
  }

  int32_t remainder = request_size % range_size_;
  if (remainder == 0) {
    request_size_ = request_size;
  } else {
    request_size_ = request_size + range_size_ - remainder;
  }

  num_ = request_size_ / range_size_;

  range_vector_.reserve(num_);
  client_vector_.reserve(num_);
  fifo_vector_.reserve(num_);
}

SegmentationMultiBufferDataProvider::~SegmentationMultiBufferDataProvider() =
    default;

void SegmentationMultiBufferDataProvider::Start() {
  int64_t start = pos_ << block_size_shift();

  if (pos_ == 0) {
    cur_end_ = start + (preload_size_ << block_size_shift()) - 1;
  } else {
    cur_end_ = start + (request_size_ << block_size_shift()) - 1;
  }
  StartRequest(start);
}

void SegmentationMultiBufferDataProvider::StartRequest(int64_t start) {
  int64_t previous = start;
  if (urlData()->length() != kPositionNotSpecified) {
    if (start >= urlData()->length()) {
      LOG(INFO) << "VideoOpt: StartRequest vid=" << id_ << "; start=" << start
                << "; len=" << urlData()->length()
                << "(hash" << std::hex << base::FastHash(base::byte_span_from_ref(this)) << ")";
      return;
    }

    if (cur_end_ >= urlData()->length()) {
      LOG(INFO) << "VideoOpt: StartRequest vid=" << id_
                << "; cur_end_=" << cur_end_ << "; len=" << urlData()->length()
                << "(hash" << std::hex << base::FastHash(base::byte_span_from_ref(this)) << ")";
      cur_end_ = urlData()->length() - 1;
    }
  }

  range_vector_.clear();
  client_vector_.clear();
  fifo_vector_.clear();

  cur_index_ = -1;
  read_index_ = 0;

  if (start == 0) {
    LOG(DEBUG) << "VideoOpt: StartRequest: start:" << start << ", cur_end_:" << cur_end_;
    range_vector_.push_back(std::make_pair(start, cur_end_));
    std::unique_ptr<VideoRangeURLLoaderClient> client =
        std::make_unique<VideoRangeURLLoaderClient>(this, start, cur_end_, id_,
                                                 task_runner_);
    client_vector_.push_back(std::move(client));
    fifo_vector_.push_back(std::list<scoped_refptr<media::DataBuffer>>());

    cur_index_++;
    client_vector_[cur_index_]->SetFifo(&fifo_vector_[cur_index_]);
    client_vector_[cur_index_]->Start();
  } else {
    int64_t end;
    while (start <= cur_end_) {
      end = start + (range_size_ << block_size_shift()) - 1;
      end = end > cur_end_ ? cur_end_ : end;

      LOG(DEBUG) << "VideoOpt: StartRequest: start:" << start << ", end_" << end 
                 << ", range_size_:" << range_size_;
      range_vector_.push_back(std::make_pair(start, end));
      std::unique_ptr<VideoRangeURLLoaderClient> client =
          std::make_unique<VideoRangeURLLoaderClient>(this, start, end, id_,
                                                   task_runner_);
      client_vector_.push_back(std::move(client));
      fifo_vector_.push_back(std::list<scoped_refptr<media::DataBuffer>>());

      cur_index_++;
      client_vector_[cur_index_]->SetFifo(&fifo_vector_[cur_index_]);
      client_vector_[cur_index_]->Start();

      start = end + 1;
    }
  }

  LOG(INFO) << "VideoOpt: StartRequest vid=" << id_ 
            << "; start=" << previous << "; end=" << cur_end_
            << "; bs=" << block(previous) << "; be=" << block_ceil(cur_end_)
            << "; pos=" << pos_ << "; num=" << cur_index_
            << "; preload_size_=" << preload_size_
            << "; request_size_=" << request_size_ << "; range=" << range_size_
            << "(hash" << std::hex << base::FastHash(base::byte_span_from_ref(this)) << ")";
}

int64_t SegmentationMultiBufferDataProvider::AvailableBytes() const {
  int64_t bytes = 0;
  for (const auto& i : fifo_vector_[read_index_]) {
    if (i->end_of_stream()) {
      return bytes;
    }
    bytes += i->data_size();
  }
  return bytes;
}

bool SegmentationMultiBufferDataProvider::Available() const {
  if (read_index_ > cur_index_ || fifo_vector_.empty() ||
      fifo_vector_[read_index_].empty()) {
    return false;
  }
  if (urlData()->length() != kPositionNotSpecified &&
      range_vector_[read_index_].second >= urlData()->length() - 1) {
    if (fifo_vector_[read_index_].back()->end_of_stream()) {
      return true;
    }
  }
  if (!fifo_vector_[read_index_].front()->end_of_stream() &&
    fifo_vector_[read_index_].front()->data_size() == block_size()) {
    return true;
  }
  return false;
}

scoped_refptr<media::DataBuffer> SegmentationMultiBufferDataProvider::Read() {
  DCHECK(Available());
  scoped_refptr<media::DataBuffer> ret = fifo_vector_[read_index_].front();
  fifo_vector_[read_index_].pop_front();

  ++pos_;

  if ((pos_ << block_size_shift()) > range_vector_[read_index_].second &&
      read_index_ < cur_index_) {
    read_index_++;
  }

  return ret;
}

void SegmentationMultiBufferDataProvider::StartNextIfNeeded() {
  if ((pos_ << block_size_shift()) < cur_end_) {
    return;
  }
  int64_t start = cur_end_ + 1;
  cur_end_ = start + (request_size_ << block_size_shift()) - 1;

  StartRequest(start);
}

void SegmentationMultiBufferDataProvider::SetDeferred(bool deferred) {
  if (!deferred) {
    StartNextIfNeeded();
  }
}

void SegmentationMultiBufferDataProvider::NotifyLengthSet() {
  if (urlData()->length() == kPositionNotSpecified) {
    return;
  }

  if (cur_end_ >= urlData()->length()) {
    cur_end_ = urlData()->length() - 1;
  }

  for (uint16_t i = read_index_; i <= cur_index_; i++) {
    if (range_vector_[i].second >= urlData()->length()) {
      range_vector_[i].second = urlData()->length() - 1;
    }
  }
}

}  // namespace blink