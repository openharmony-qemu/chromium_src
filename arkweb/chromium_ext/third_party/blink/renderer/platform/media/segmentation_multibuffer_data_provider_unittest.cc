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

#include "ohos_sdk/openharmony/native/llvm/bin/../include/libcxx-ohos/include/c++/v1/__ranges/lazy_split_view.h"
#define private public
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/segmentation_multibuffer_data_provider.h"
#undef private

#include "build/build_config.h"
#include "base/logging.h"
#include "base/memory/weak_ptr.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/task_environment.h"
#include "media/base/media_log.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/platform/media/url_index.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/renderer/platform/media/testing/mock_resource_fetch_context.h"
#include "third_party/blink/renderer/platform/media/testing/mock_web_associated_url_loader.h"

namespace blink {

using ::testing::_;
using ::testing::NiceMock;

const int kDataSize = 1024;
const char kHttpUrl[] = "http://foo.bar.com";
const int64_t kBlockShift = 32768;

class SegmentationMultiBufferDataProviderTest : public ::testing::Test {
public:
  SegmentationMultiBufferDataProviderTest() {
    for (int i = 0; i < kDataSize; ++i) {
      data_[i] = i;
    }

    ON_CALL(fetch_context_, CreateVideoUrlLoader(_))
        .WillByDefault(Invoke(
            this, &SegmentationMultiBufferDataProviderTest::CreateVideoUrlLoader));
    ON_CALL(fetch_context_, CreateUrlLoader(_))
        .WillByDefault(Invoke(
            this, &SegmentationMultiBufferDataProviderTest::CreateUrlLoader));
  }

  void Initialize(const char* url, int32_t pos, int32_t preload_size, int32_t request_size, uint16_t byte_rate) {
    url_ = KURL(url);
    url_index_.SetNewsFeedPageFitted(true);
    url_data_ = url_index_.GetByUrl(url_, UrlData::CORS_UNSPECIFIED, UrlData::kNormal);
    std::string video_id = "html5_api";

    segment_provider_ = std::make_unique<SegmentationMultiBufferDataProvider>(url_data_.get(), pos, preload_size, 
        request_size, byte_rate, video_id, false, task_environment_.GetMainThreadTaskRunner());
  }

protected:
  std::unique_ptr<WebAssociatedURLLoader> CreateVideoUrlLoader(
      const WebAssociatedURLLoaderOptions& options) {
    auto url_loader = std::make_unique<NiceMock<MockWebAssociatedURLLoader>>();
    return url_loader;
  }

  std::unique_ptr<WebAssociatedURLLoader> CreateUrlLoader(
      const WebAssociatedURLLoaderOptions& options) {
    auto url_loader = std::make_unique<NiceMock<MockWebAssociatedURLLoader>>();
    return url_loader;
  }

  base::test::SingleThreadTaskEnvironment task_environment_;
  KURL url_;
  NiceMock<MockResourceFetchContext> fetch_context_;
  UrlIndex url_index_{&fetch_context_, task_environment_.GetMainThreadTaskRunner()};
  scoped_refptr<UrlData> url_data_;
  uint8_t data_[kDataSize];

  std::unique_ptr<SegmentationMultiBufferDataProvider> segment_provider_;
};

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Initialize001) {
  int32_t pos = 0;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;
  
  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);
  int32_t range_size =  segment_provider_->range_size_;
  EXPECT_EQ(range_size, 8);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Initialize002) {
  int32_t pos = 0;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 200;
  
  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);
  int32_t range_size =  segment_provider_->range_size_;
  EXPECT_EQ(range_size, 16);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Initialize003) {
  int32_t pos = 0;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 300;
  
  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);
  int32_t range_size =  segment_provider_->range_size_;
  EXPECT_EQ(range_size, 24);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Initialize004) {
  int32_t pos = 0;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;
  
  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);
  int32_t request_size_value =  segment_provider_->request_size_;
  EXPECT_EQ(request_size_value, 24);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Initialize005) {
  int32_t pos = 0;
  int32_t preload_size = 8;
  int32_t request_size = 26;
  uint16_t byte_rate = 120;
  
  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);
  int32_t request_size_value =  segment_provider_->request_size_;
  EXPECT_EQ(request_size_value, 32);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Start001) {
  int32_t pos = 0;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;
  
  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);
  segment_provider_->Start();
  int64_t cur_end = segment_provider_->cur_end_;
  int64_t expect_value = preload_size * kBlockShift - 1;
  EXPECT_EQ(cur_end, expect_value);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Start002) {
  int32_t pos = 1;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;
  
  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);
  segment_provider_->Start();
  int64_t cur_end = segment_provider_->cur_end_;
  int64_t expect_value = (pos + request_size)  * kBlockShift - 1;
  EXPECT_EQ(cur_end, expect_value);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_StartRequest001) {
  int32_t pos = 110;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);

  int64_t urldata_length = 3276810;
  url_data_->set_length(urldata_length);

  segment_provider_->Start();
  int64_t cur_end = segment_provider_->cur_end_;
  EXPECT_GT(cur_end, urldata_length);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_StartRequest002) {
  int32_t pos = 80;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);

  int64_t urldata_length = 3276810;
  url_data_->set_length(urldata_length);

  segment_provider_->Start();
  int64_t cur_end = segment_provider_->cur_end_;
  int64_t expect_value = urldata_length - 1;
  EXPECT_EQ(cur_end, expect_value);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_StartRequest003) {
  int32_t pos = 0;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);

  int64_t urldata_length = 3276810;
  url_data_->set_length(urldata_length);

  segment_provider_->Start();
  size_t request_length = segment_provider_->client_vector_.size();
  EXPECT_EQ(request_length, 1);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_StartRequest004) {
  int32_t pos = 10;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);

  int64_t urldata_length = 3276810;
  url_data_->set_length(urldata_length);

  segment_provider_->Start();
  size_t request_length = segment_provider_->client_vector_.size();
  EXPECT_EQ(request_length, 3);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_AvaialbleBytes001) {
  int32_t pos = 0;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);

  int64_t urldata_length = 3276810;
  url_data_->set_length(urldata_length);

  int64_t block_size = segment_provider_->block_size();
  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  request_buffers.push_back(base::MakeRefCounted<media::DataBuffer>(static_cast<int>(block_size)));
  request_buffers.push_back(media::DataBuffer::CreateEOSBuffer());
  segment_provider_->fifo_vector_.push_back(request_buffers);

  int64_t bytes = segment_provider_->AvailableBytes();
  EXPECT_EQ(bytes, block_size);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Available001) {
  int32_t pos = 0;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);
  bool is_available = segment_provider_->Available();
  EXPECT_FALSE(is_available);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Available002) {
  int32_t pos = 0;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);

  int64_t urldata_length = 3276810;
  url_data_->set_length(urldata_length);

  int64_t block_size = segment_provider_->block_size();
  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  request_buffers.push_back(base::MakeRefCounted<media::DataBuffer>(static_cast<int>(block_size)));
  request_buffers.push_back(base::MakeRefCounted<media::DataBuffer>(static_cast<int>(block_size)));
  request_buffers.push_back(media::DataBuffer::CreateEOSBuffer());
  segment_provider_->fifo_vector_.push_back(request_buffers);

  int64_t start = 96 * kBlockShift;
  int64_t end = 104 * kBlockShift - 1;
  segment_provider_->range_vector_.push_back(std::make_pair(start, end));
  segment_provider_->cur_index_ = 0;

  bool is_available = segment_provider_->Available();
  EXPECT_TRUE(is_available);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Available003) {
  int32_t pos = 10;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);

  int64_t urldata_length = 3276810;
  url_data_->set_length(urldata_length);

  int64_t block_size = segment_provider_->block_size();
  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  request_buffers.push_back(base::MakeRefCounted<media::DataBuffer>(static_cast<int>(block_size)));
  request_buffers.push_back(base::MakeRefCounted<media::DataBuffer>(static_cast<int>(block_size)));
  request_buffers.push_back(media::DataBuffer::CreateEOSBuffer());
  segment_provider_->fifo_vector_.push_back(request_buffers);

  int64_t start = 10 * kBlockShift;
  int64_t end = 18 * kBlockShift - 1;
  segment_provider_->range_vector_.push_back(std::make_pair(start, end));
  segment_provider_->cur_index_ = 0;

  bool is_available = segment_provider_->Available();
  EXPECT_TRUE(is_available);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Available004) {
  int32_t pos = 10;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);

  int64_t urldata_length = 3276810;
  url_data_->set_length(urldata_length);

  int64_t block_size = segment_provider_->block_size();
  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  request_buffers.push_back(media::DataBuffer::CreateEOSBuffer());
  segment_provider_->fifo_vector_.push_back(request_buffers);

  int64_t start = 10 * kBlockShift;
  int64_t end = 18 * kBlockShift - 1;
  segment_provider_->range_vector_.push_back(std::make_pair(start, end));

  bool is_available = segment_provider_->Available();
  EXPECT_FALSE(is_available);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_Read001) {
  int32_t pos = 10;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);

  int64_t urldata_length = 3276810;
  url_data_->set_length(urldata_length);

  int64_t block_size = segment_provider_->block_size();
  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  request_buffers.push_back(base::MakeRefCounted<media::DataBuffer>(static_cast<int>(block_size)));
  request_buffers.push_back(base::MakeRefCounted<media::DataBuffer>(static_cast<int>(block_size)));
  request_buffers.push_back(media::DataBuffer::CreateEOSBuffer());
  segment_provider_->fifo_vector_.push_back(request_buffers);

  int64_t start = 10 * kBlockShift;
  int64_t end = 18 * kBlockShift - 1;
  segment_provider_->range_vector_.push_back(std::make_pair(start, end));

  scoped_refptr<media::DataBuffer> data_buffer = segment_provider_->Read();
  EXPECT_TRUE(data_buffer->data_size() == block_size);
  EXPECT_FALSE(data_buffer->end_of_stream());
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_SetDeferred001) {
  int32_t pos = 10;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);

  int64_t urldata_length = 3276810;
  url_data_->set_length(urldata_length);

  segment_provider_->Start();
  segment_provider_->SetDeferred(false);
  EXPECT_NE(url_data_->length(), kPositionNotSpecified);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_NotifyLengthSet001) {
  int32_t pos = 0;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);
  segment_provider_->NotifyLengthSet();
  EXPECT_EQ(url_data_->length(), kPositionNotSpecified);
}

TEST_F(SegmentationMultiBufferDataProviderTest, VideoOpt_NotifyLengthSet002) {
  int32_t pos = 80;
  int32_t preload_size = 8;
  int32_t request_size = 24;
  uint16_t byte_rate = 120;

  Initialize(kHttpUrl, pos, preload_size, request_size, byte_rate);
  segment_provider_->Start();

  int64_t urldata_length = 3276810;
  url_data_->set_length(urldata_length);

  segment_provider_->NotifyLengthSet();
  int64_t cur_end = segment_provider_->cur_end_;
  EXPECT_EQ(cur_end, urldata_length - 1);
}
}