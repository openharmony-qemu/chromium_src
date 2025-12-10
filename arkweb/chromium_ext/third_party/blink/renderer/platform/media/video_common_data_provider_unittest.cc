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
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/video_common_data_provider.h"
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
#include "media/base/seekable_buffer.h"
#include "net/base/net_errors.h"
#include "net/http/http_request_headers.h"
#include "net/http/http_util.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/platform/media/url_index.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/platform/web_url_error.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/public/platform/web_url_response.h"
#include "third_party/blink/renderer/platform/media/testing/mock_resource_fetch_context.h"
#include "third_party/blink/renderer/platform/media/testing/mock_web_associated_url_loader.h"

namespace blink {

class WebAssociatedURLLoader;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Truly;

const int kDataSize = 1024;
const int kHttpOK_VideoOpt = 200;
const int kHttpPartialContent_VideoOpt = 206;

const char kHttpUrl[] = "http://foo.bar.com";
const char kHttpRedirect[] = "http://test/ing";
const char kEtag[] = "\"arglebargle glopy-glyf?\"";
const std::string kVideoId = "html5_api";

// Predicate that checks the Accept-Encoding request header.
static bool CorrectAcceptEncoding(const WebURLRequest& request) {
  std::string value = request
                          .HttpHeaderField(WebString::FromUTF8(
                              net::HttpRequestHeaders::kAcceptEncoding))
                          .Utf8();
  return (base::Contains(value, "identity;q=1")) &&
         (base::Contains(value, "*;q=0"));
}

static bool AcceptURLLoadClient(base::WeakPtr<WebAssociatedURLLoaderClient> client) {
    return client != nullptr;
}

class MockVideoCommonDataProvider : public VideoCommonDataProvider {
public:
  MockVideoCommonDataProvider() = default;

  MockVideoCommonDataProvider(UrlData* url_data,
                              MultiBufferBlockId pos,
                              std::string id,
                              bool is_client_audio_element,
                              scoped_refptr<base::SingleThreadTaskRunner> task_runner) 
    : VideoCommonDataProvider(url_data, pos, id, is_client_audio_element, task_runner) {}

  MockVideoCommonDataProvider(const MockVideoCommonDataProvider&) = default;
  MockVideoCommonDataProvider& operator=(const MockVideoCommonDataProvider&) =
      default;
  ~MockVideoCommonDataProvider() override = default;

  MOCK_CONST_METHOD0(Available, bool());
  MOCK_CONST_METHOD0(AvailableBytes, int64_t());
  MOCK_METHOD0(Read, scoped_refptr<media::DataBuffer>());
  MOCK_METHOD1(SetDeferred, void(bool));
  MOCK_METHOD0(Start, void());
};

class VideoCommonDataProviderTest : public ::testing::Test {
public:
  VideoCommonDataProviderTest() {
    ON_CALL(fetch_context_, CreateUrlLoader(_))
        .WillByDefault(Invoke(
            this, &VideoCommonDataProviderTest::CreateUrlLoader));
  }

  void Initialize(const char* url, MultiBufferBlockId pos,
    UrlData::CorsMode corsMode = UrlData::CORS_UNSPECIFIED) {
    url_ = KURL(url);
    url_data_ = url_index_.GetByUrl(url_, UrlData::CORS_UNSPECIFIED, UrlData::kNormal);
    url_data_->set_etag(kEtag);

    std::string id = "html5_api";
    bool is_client_audio_element = false;
    auto loader = std::make_unique<MockVideoCommonDataProvider>(url_data_.get(), pos, id, 
        is_client_audio_element, task_environment_.GetMainThreadTaskRunner());
    provider_ = loader.get();
    url_data_->multibuffer()->AddProvider(std::move(loader));
  }

  void SetUrlData(UrlData* data) {
    if (provider_) {
      provider_->SetUrlData(data);
    }
  }

  UrlData* GetUrlData() {
    if (provider_) {
      return provider_->urlData();
    }
    return nullptr;
  }
  
  const KURL& GetOrigin() {
    return provider_->Origin();
  }
  
  UrlData::CorsMode GetCorsMode() {
    return provider_->CorsMode();
  }

  MultiBufferBlockId GetTell() {
    return provider_ ? provider_->Tell() : 0;
  }

  void Fallback()  {
    return provider_->Fallback();
  }

  int64_t BlockSize() {
    return provider_->block_size();
  }

  int32_t blockSizeShift() {
    return provider_->block_size_shift();
  }

  WebURLRequest GetRequest(int64_t start, int64_t end) {
    return provider_->PrepareRequest(start, end);
  }

protected:
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
  raw_ptr<MockVideoCommonDataProvider> provider_;
};

class VideoRangeURLLoaderClientTest : public ::testing::Test {
public:
  VideoRangeURLLoaderClientTest() {
    for (int i = 0; i < kDataSize; ++i) {
      data_[i] = i;
    }

    ON_CALL(fetch_context_, CreateVideoUrlLoader(_))
        .WillByDefault(Invoke(
            this, &VideoRangeURLLoaderClientTest::CreateVideoUrlLoader));
    ON_CALL(fetch_context_, CreateUrlLoader(_))
        .WillByDefault(Invoke(
            this, &VideoRangeURLLoaderClientTest::CreateUrlLoader));
  }

  void Initialize(const char* url, int32_t pos, int64_t start, int64_t end,
      UrlData::CorsMode corsMode = UrlData::CORS_UNSPECIFIED) {
    url_ = KURL(url);
    url_index_.SetNewsFeedPageFitted(true);
    url_data_ = url_index_.GetByUrl(url_, corsMode, UrlData::kNormal);

    url_data_->OnRedirect(
        base::BindOnce(&VideoRangeURLLoaderClientTest::RedirectCallback,
                       base::Unretained(this)));

    auto provider = std::make_unique<MockVideoCommonDataProvider>(url_data_.get(), pos, kVideoId,
        false, task_environment_.GetMainThreadTaskRunner());
    provider_ = provider.get();
    url_data_->multibuffer()->AddProvider(std::move(provider));

    loader_client_ = std::make_unique<VideoRangeURLLoaderClient>(provider_, start, end, kVideoId,
        task_environment_.GetMainThreadTaskRunner());
    start_ = start;
  }
  
  std::pair<int64_t, int64_t> GetRange() {
    return loader_client_->Range();
  }

  bool ProviderInRange() {
    return loader_client_->ProviderInThisRange();
  }

  void FullResponse(int64_t instance_size) {
    WebURLResponse response(url_);
    response.SetHttpHeaderField(
        WebString::FromUTF8("Content-Length"),
        WebString::FromUTF8(base::StringPrintf("%" PRId64, instance_size)));
    response.SetExpectedContentLength(instance_size);
    response.SetHttpStatusCode(kHttpOK_VideoOpt);
    loader_client_->DidReceiveResponse(response);
  }


  void PartialResponse(int64_t first_position,
                       int64_t last_position,
                       int64_t instance_size) {
    PartialResponse(first_position, last_position, instance_size, false, true);
  }

  void PartialResponse(int64_t first_position,
                       int64_t last_position,
                       int64_t instance_size,
                       bool chunked,
                       bool accept_ranges) {
    WebURLResponse response(url_);
    response.SetHttpHeaderField(
        WebString::FromUTF8("Content-Range"),
        WebString::FromUTF8(
            base::StringPrintf("bytes "
                               "%" PRId64 "-%" PRId64 "/%" PRId64,
                               first_position, last_position, instance_size)));

    // HTTP 1.1 doesn't permit Content-Length with Transfer-Encoding: chunked.
    int64_t content_length = -1;
    if (chunked) {
      response.SetHttpHeaderField(WebString::FromUTF8("Transfer-Encoding"),
                                  WebString::FromUTF8("chunked"));
    } else {
      content_length = last_position - first_position + 1;
    }
    response.SetExpectedContentLength(content_length);

    // A server isn't required to return Accept-Ranges even though it might.
    if (accept_ranges) {
      response.SetHttpHeaderField(WebString::FromUTF8("Accept-Ranges"),
                                  WebString::FromUTF8("bytes"));
    }

    response.SetHttpStatusCode(kHttpPartialContent_VideoOpt);
    loader_client_->DidReceiveResponse(response);

    EXPECT_EQ(instance_size, url_data_->length());
  }

  void Redirect(const char* url) {
    WebURL new_url{KURL(url)};
    WebURLResponse redirect_response(url_);

    loader_client_->WillFollowRedirect(new_url, redirect_response);

    base::RunLoop().RunUntilIdle();
  }

  void StopWhenLoad() {
    provider_ = nullptr;
    loader_client_ = nullptr;
    url_data_ = nullptr;
  }

  void WriteLoader(int position, int size) {
    loader_client_->DidReceiveData(
        base::as_chars(base::span(data_).subspan(position, size)));
  }

  void WriteData(int size) {
    auto data = base::HeapArray<char>::Uninit(size);
    loader_client_->DidReceiveData(data);
  }

  MOCK_METHOD1(RedirectCallback, void(const scoped_refptr<UrlData>&));

  // Verifies that data in buffer[0...size] is equal to data_[pos...pos+size].
  void VerifyBuffer(uint8_t* buffer, int pos, int size) {
    EXPECT_EQ(0, memcmp(buffer, data_ + pos, size));
  }

  void SetUrlData(const scoped_refptr<UrlData>& new_url_data) {
    url_data_ = new_url_data;
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
  
  raw_ptr<MockVideoCommonDataProvider> provider_;
  std::unique_ptr<VideoRangeURLLoaderClient> loader_client_;

  int64_t start_ = 0;
  uint8_t data_[kDataSize];
};

// VideoCommonDataProviderTest
TEST_F(VideoCommonDataProviderTest, VideoOpt_UrlDataTest) {
  Initialize(kHttpUrl, 0);

  UrlData* urlDataPtr = GetUrlData();
  EXPECT_NE(urlDataPtr, nullptr);

  KURL url1("http://foo.bar.com");
  UrlIndex url_index_local{nullptr, task_environment_.GetMainThreadTaskRunner()};
  scoped_refptr<UrlData> url_data_local =
    url_index_local.GetByUrl(url1, UrlData::CORS_UNSPECIFIED, UrlData::kNormal);
  SetUrlData(url_data_local.get());
  EXPECT_NE(url_data_local.get(), nullptr);
}

TEST_F(VideoCommonDataProviderTest, VideoOpt_OriginCorsModeTellTest) {
  Initialize(kHttpUrl, 0);

  const KURL& urlOrigin = GetOrigin();
  UrlData::CorsMode corsMode = GetCorsMode();
  MultiBufferBlockId blockId = GetTell();

  KURL kurl("http://foo.bar.com");

  EXPECT_EQ(corsMode, UrlData::CORS_UNSPECIFIED);
  EXPECT_EQ(blockId, 0);
}

TEST_F(VideoCommonDataProviderTest, VideoOpt_BlockSizeTest) {
  Initialize(kHttpUrl, 0);

  UrlData* urlDataPtr = GetUrlData();
  EXPECT_NE(urlDataPtr, nullptr);

  ResourceMultiBuffer* multiBufferPtr = urlDataPtr->multibuffer();
  EXPECT_NE(multiBufferPtr, nullptr);

  int64_t block_size  = BlockSize();
  int32_t block_size_shift = blockSizeShift();

  int64_t expect_block_size = 32768;
  int32_t expect_block_size_shift = 15;

  EXPECT_EQ(block_size, expect_block_size);
  EXPECT_EQ(block_size_shift, expect_block_size_shift);
}

TEST_F(VideoCommonDataProviderTest, VideoOpt_FallbackTest) {
  Initialize(kHttpUrl, 0);

  UrlData* urlData = GetUrlData();
  if (urlData) {
    bool is_need_fallback = urlData->IsNeedFallback();
    EXPECT_EQ(is_need_fallback, false);

    Fallback();
    is_need_fallback = urlData->IsNeedFallback();
    EXPECT_EQ(is_need_fallback, true);
  }
}

TEST_F(VideoCommonDataProviderTest, VideoOpt_StartRequestTest) {
  Initialize(kHttpUrl, 0);

  WebURLRequest endRequest = GetRequest(0, -1); // end
  WebString rangeStr = WebString::FromUTF8(net::HttpRequestHeaders::kRange);
  WebString endRangeStr = endRequest.HttpHeaderField(rangeStr);
  EXPECT_EQ(endRangeStr.length(), 8);

  WebURLRequest blockRequest = GetRequest(0, 10); // block
  WebString blockRangeStr = blockRequest.HttpHeaderField(rangeStr);
  EXPECT_EQ(blockRangeStr.length(), 10);
}

TEST_F(VideoCommonDataProviderTest, VideoOpt_StartRequest002Test) {
  Initialize(kHttpUrl, 0, UrlData::CORS_UNSPECIFIED);

  WebString rangeStr = WebString::FromUTF8(net::HttpRequestHeaders::kRange);
  WebURLRequest blockRequest = GetRequest(0, 10); // block
  WebString blockRangeStr = blockRequest.HttpHeaderField(rangeStr);
  EXPECT_EQ(blockRangeStr.length(), 10);
}

TEST_F(VideoCommonDataProviderTest, VideoOpt_StartRequest003Test) {
  Initialize(kHttpUrl, 0, UrlData::CORS_USE_CREDENTIALS);

  WebString rangeStr = WebString::FromUTF8(net::HttpRequestHeaders::kRange);
  WebURLRequest blockRequest = GetRequest(0, 10); // block
  WebString blockRangeStr = blockRequest.HttpHeaderField(rangeStr);
  EXPECT_EQ(blockRangeStr.length(), 10);
}

// VideoRangeURLLoaderClientTest
TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_RangeTest) {
  Initialize(kHttpUrl, 0,  0, 100);
  std::pair<int64_t, int64_t> pairData = GetRange();
  EXPECT_EQ(pairData.second, 100);
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_ProviderInRangeTest1) {
  Initialize(kHttpUrl, 0, 0, 100);
  bool isInRange = ProviderInRange();
  EXPECT_TRUE(isInRange);
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_ProviderInRangeTest2) {
  Initialize(kHttpUrl, 0, 2, 100);
  bool isInRange = ProviderInRange();
  EXPECT_FALSE(isInRange);
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_ProviderInRangeTest3) {
  Initialize(kHttpUrl, 2, 2, -1);
  bool isInRange = ProviderInRange();
  EXPECT_TRUE(isInRange);
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_ProviderInRangeTest4) {
  Initialize(kHttpUrl, 101, 2, 100);
  bool isInRange = ProviderInRange();
  EXPECT_FALSE(isInRange);
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_StartStop) {
  Initialize(kHttpUrl, 0, 0, 100);
  loader_client_->Start();
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_PartialResponse) {
  Initialize(kHttpUrl, 0, 0, 100);
  loader_client_->Start();

  PartialResponse(100, 200, 1024);
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_PartialResponse_Chunked) {
  Initialize(kHttpUrl, 0, 0, 100);
  loader_client_->Start();

  PartialResponse(100, 200, 1024, true, true);
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_InvalidPartialResponse) {
  Initialize(kHttpUrl, 0, 0, 100);
  loader_client_->Start();

  WebURLResponse response(url_);
  response.SetHttpHeaderField(
      WebString::FromUTF8("Content-Range"),
      WebString::FromUTF8(base::StringPrintf("bytes "
                                             "%d-%d/%d",
                                             1, 10, 1024)));
  response.SetExpectedContentLength(10);
  response.SetHttpStatusCode(kHttpPartialContent_VideoOpt);
  loader_client_->DidReceiveResponse(response);
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_InvalidPartialResponse002) {
  Initialize(kHttpUrl, 10, 0, 100);
  loader_client_->Start();

  PartialResponse(0, 8, 100, true, true);
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_TestRedirects) {
  // Test redirect.
  Initialize(kHttpUrl, 0, 0, 100);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);

  loader_client_->Start();
  Redirect(kHttpRedirect);
  FullResponse(1024);
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_SetFifoTest) {
  Initialize(kHttpUrl, 0,  0, 100);
  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_Start001) {
  Initialize(kHttpUrl, 0, 100, 120);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
  loader_client_->Start();

  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_Start002) {
  Initialize(kHttpUrl, 0, 100, 0, UrlData::CORS_ANONYMOUS);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
  loader_client_->Start();

  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_ReceiveData001) {
  int64_t kDataSize_test = 3276800;
  Initialize(kHttpUrl, 0, 0, kDataSize_test -1);
  url_data_->set_length(kDataSize_test);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
  loader_client_->Start();

  std::vector<char> buffer(kDataSize_test, 0);
  base::span<const char> data_span(buffer.data(), buffer.size());
  loader_client_->DidReceiveData(data_span);
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_ReceiveData002) {
  int64_t kDataSize_test = 3276800;
  Initialize(kHttpUrl, 0, 0, kDataSize_test -1);
  url_data_->set_length(kDataSize_test);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
  loader_client_->Start();

  std::vector<char> buffer(0, 0);
  base::span<const char> data_span(buffer.data(), buffer.size());

  loader_client_->bytes_to_discard_ = 10;
  loader_client_->DidReceiveData(data_span);
  EXPECT_EQ(loader_client_->bytes_to_discard_, 10);
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_Terminate001) {
  Initialize(kHttpUrl, 0, 0, 100);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
  loader_client_->Start();

  loader_client_->Terminate();
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_DidFinishLoading001) {
  Initialize(kHttpUrl, 10, 0, 100);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
  loader_client_->Start();

  loader_client_->DidFinishLoading();
  EXPECT_EQ(request_buffers.size(), 1);
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_DidFinishLoading002) {
  int64_t kDataSize_test = 3276800;
  Initialize(kHttpUrl, 0, 0, kDataSize_test -1);
  url_data_->set_length(kDataSize_test);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
  loader_client_->Start();

  loader_client_->DidFinishLoading();
  EXPECT_GT(loader_client_->retries_, 0);
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_DidFinishLoading003) {
  int64_t kDataSize_test = 3276800;
  Initialize(kHttpUrl, 0, 0, kDataSize_test -1);
  url_data_->set_length(kDataSize_test);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
  loader_client_->Start();

  loader_client_->retries_ = 50;
  loader_client_->DidFinishLoading();
  EXPECT_EQ(request_buffers.size(), 0);
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_DidFail001) {
  Initialize(kHttpUrl, 10, 0, 100);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
  loader_client_->Start();

  WebURL web_url(url_);
  blink::WebURLError error(net::ERR_FAILED, web_url);
  loader_client_->DidFail(error);
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_DidFail002) {
  Initialize(kHttpUrl, 10, 0, 100);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
  loader_client_->Start();

  WebURL web_url(url_);
  blink::WebURLError error(net::ERR_FAILED, web_url);
  loader_client_->retries_  = 5;
  loader_client_->DidFail(error);
  StopWhenLoad();
}

TEST_F(VideoRangeURLLoaderClientTest, VideoOpt_DidReStart001) {
  Initialize(kHttpUrl, 0, 0, 100);

  std::list<scoped_refptr<media::DataBuffer>> request_buffers;
  loader_client_->SetFifo(&request_buffers);
  loader_client_->Start();

  loader_client_->DidReStart();
  StopWhenLoad();
}

}