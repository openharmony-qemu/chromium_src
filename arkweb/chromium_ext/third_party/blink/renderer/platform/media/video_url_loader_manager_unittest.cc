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
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/video_url_loader_manager.h"
#undef private

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/public/platform/web_url_response.h"
#include "third_party/blink/public/web/web_associated_url_loader.h"
#include "third_party/blink/public/web/web_associated_url_loader_client.h"
#include "third_party/blink/public/web/web_associated_url_loader_options.h"
#include "third_party/blink/public/web/web_frame.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_video_priority.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"
#include "third_party/blink/renderer/platform/testing/url_test_helpers.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

#define private public
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/media/video_url_loader_impl.h"
#undef private

using blink::test::RunPendingTasks;
using blink::url_test_helpers::ToKURL;
using ::testing::_;

namespace blink {

class MockWebAssociatedURLLoaderClient : public WebAssociatedURLLoaderClient {
 public:
  MockWebAssociatedURLLoaderClient() = default;
  ~MockWebAssociatedURLLoaderClient() override = default;

  MOCK_METHOD0(DidReStart, void());

  base::WeakPtr<WebAssociatedURLLoaderClient> GetWeakPtr() {
    return weak_factory_.GetWeakPtr();
  }

 private:
  base::WeakPtrFactory<MockWebAssociatedURLLoaderClient> weak_factory_{this};
};

class MockVideoURLLoaderImpl {
 public:
  MockVideoURLLoaderImpl() {
    helper_.InitializeAndLoad("about:blank");
    frame_impl_ = helper_.GetWebView()->MainFrameImpl();
    CreateUrlLoader();
  }

  ~MockVideoURLLoaderImpl() { helper_.Reset(); }

  base::WeakPtr<VideoURLLoaderImpl> GetWeakPtr() {
    if (url_loader_) {
      return url_loader_->weak_factory_.GetWeakPtr();
    }
    return base::WeakPtr<VideoURLLoaderImpl>();
  }

  void ResetLoader() { url_loader_.reset(); }

 private:
  void CreateUrlLoader() {
    WebAssociatedURLLoaderOptions options = WebAssociatedURLLoaderOptions();
    std::unique_ptr<WebAssociatedURLLoader> base_url_loader =
        frame_impl_->CreateVideoURLLoader(options);
    VideoURLLoaderImpl* url_loader =
        static_cast<VideoURLLoaderImpl*>(base_url_loader.get());
    if (url_loader) {
      base_url_loader.release();
      url_loader_ = std::unique_ptr<VideoURLLoaderImpl>(url_loader);
    }
  }

  WebLocalFrameImpl* frame_impl_;
  frame_test_helpers::WebViewHelper helper_;
  std::unique_ptr<VideoURLLoaderImpl> url_loader_;
};

class VideoUrlLoaderManagerTest : public ::testing::Test {
 public:
  void SetUp() override {}

  void TearDown() override {}

  void AddUrlLoaderTest(base::WeakPtr<VideoURLLoaderImpl> load,
                        std::string& videoId,
                        int64_t start,
                        const std::string& url_str,
                        base::WeakPtr<WebAssociatedURLLoaderClient> client) {
    KURL url = ToKURL(url_str);
    WebURLRequest request(url);
    PriorityLoader priLoader =
        PriorityLoader(load, videoId, start, std::move(request), client);
    url_loader_manager_.AddUrlLoader(std::move(priLoader));
  }

  void RemoveUrlLoaderTest(base::WeakPtr<VideoURLLoaderImpl> load) {
    url_loader_manager_.RemoveUrlLoader(load);
  }

 protected:
  test::TaskEnvironment task_environment_;
  VideoUrlLoaderManager url_loader_manager_;
};

TEST_F(VideoUrlLoaderManagerTest, VideoOpt_AddUrlLoaderTest) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader_;

  int64_t start1 = 0;
  std::string video_str = "html5_api1";
  std::string url_str = "http://www.test.com/SameOriginSuccess.html";
  AddUrlLoaderTest(url_loader_.GetWeakPtr(), video_str, start1, url_str,
                   loader_client.GetWeakPtr());
  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 0);
  EXPECT_EQ(url_loader_manager_.pending_list_.size(), 1);

  auto& iter_f = url_loader_manager_.pending_list_.front();
  EXPECT_EQ(iter_f.priority_, UINT16_MAX);
}

TEST_F(VideoUrlLoaderManagerTest, VideoOpt_AddUrlLoaderTest2) {
  MockWebAssociatedURLLoaderClient loader_client1;
  MockVideoURLLoaderImpl url_loader_1;

  int64_t startPos1 = 1000;
  std::string video_str = "html5_api1";
  std::string url_str = "http://www.test.com/SameOriginSuccess.html";
  AddUrlLoaderTest(url_loader_1.GetWeakPtr(), video_str, startPos1, url_str,
                   loader_client1.GetWeakPtr());

  MockWebAssociatedURLLoaderClient loader_client2;
  MockVideoURLLoaderImpl url_loader_2;
  int64_t startPos2 = 0;
  AddUrlLoaderTest(url_loader_2.GetWeakPtr(), video_str, startPos2, url_str,
                   loader_client2.GetWeakPtr());

  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 0);
  EXPECT_EQ(url_loader_manager_.pending_list_.size(), 2);

  auto& iter_f = url_loader_manager_.pending_list_.front();
  auto& iter_b = url_loader_manager_.pending_list_.back();

  EXPECT_EQ(iter_f.priority_, UINT16_MAX);
  EXPECT_EQ(iter_b.priority_, UINT16_MAX);

  EXPECT_EQ(iter_f.start_, startPos2);
  EXPECT_EQ(iter_b.start_, startPos1);
}

TEST_F(VideoUrlLoaderManagerTest, VideoOpt_RemoveUrlLoaderTest) {
  MockWebAssociatedURLLoaderClient loader_client1;
  MockVideoURLLoaderImpl url_loader_1;

  int64_t startPos1 = 1000;
  std::string video_str = "html5_api1";
  std::string url_str = "http://www.test.com/SameOriginSuccess.html";
  AddUrlLoaderTest(url_loader_1.GetWeakPtr(), video_str, startPos1, url_str,
                   loader_client1.GetWeakPtr());

  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 0);
  EXPECT_EQ(url_loader_manager_.pending_list_.size(), 1);

  RemoveUrlLoaderTest(url_loader_1.GetWeakPtr());
  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 0);
  EXPECT_EQ(url_loader_manager_.pending_list_.size(), 0);
}

TEST_F(VideoUrlLoaderManagerTest, VideoOpt_RemoveUrlLoaderTest2) {
  MockWebAssociatedURLLoaderClient loader_client1;
  MockVideoURLLoaderImpl url_loader_1;

  int64_t startPos1 = 1000;
  std::string video_str = "html5_api1";
  std::string url_str = "http://www.test.com/SameOriginSuccess.html";
  AddUrlLoaderTest(url_loader_1.GetWeakPtr(), video_str, startPos1, url_str,
                   loader_client1.GetWeakPtr());

  MockWebAssociatedURLLoaderClient loader_client2;
  MockVideoURLLoaderImpl url_loader_2;
  int64_t startPos2 = 0;
  AddUrlLoaderTest(url_loader_2.GetWeakPtr(), video_str, startPos2, url_str,
                   loader_client2.GetWeakPtr());

  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 0);
  EXPECT_EQ(url_loader_manager_.pending_list_.size(), 2);

  RemoveUrlLoaderTest(url_loader_1.GetWeakPtr());
  RemoveUrlLoaderTest(url_loader_2.GetWeakPtr());

  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 0);
  EXPECT_EQ(url_loader_manager_.pending_list_.size(), 0);
}

TEST_F(VideoUrlLoaderManagerTest, VideoOpt_SetVideoPlayingTest) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader_;

  int64_t start1 = 0;
  std::string video_str = "html5_api1";
  std::string url_str = "http://www.test.com/SameOriginSuccess.html";
  AddUrlLoaderTest(url_loader_.GetWeakPtr(), video_str, start1, url_str,
                   loader_client.GetWeakPtr());
  EXPECT_EQ(url_loader_manager_.pending_list_.size(), 1);
  auto& iter_f = url_loader_manager_.pending_list_.front();

  url_loader_manager_.SetVideoIsPlaying(video_str, false);
  EXPECT_EQ(iter_f.priority_, UINT16_MAX);
}

TEST_F(VideoUrlLoaderManagerTest, VideoOpt_SetVideoPriorityTest) {
  MockWebAssociatedURLLoaderClient loader_client1;
  ON_CALL(loader_client1, DidReStart()).WillByDefault(testing::Return());
  MockVideoURLLoaderImpl url_loader_1;

  int64_t startPos1 = 0;
  std::string video_str1 = "html5_api1";
  std::string url_str = "http://www.test.com/SameOriginSuccess.html";
  AddUrlLoaderTest(url_loader_1.GetWeakPtr(), video_str1, startPos1, url_str,
                   loader_client1.GetWeakPtr());

  MockWebAssociatedURLLoaderClient loader_client2;
  ON_CALL(loader_client2, DidReStart()).WillByDefault(testing::Return());
  MockVideoURLLoaderImpl url_loader_2;
  int64_t startPos2 = 0;
  std::string video_str2 = "html5_api2";
  AddUrlLoaderTest(url_loader_2.GetWeakPtr(), video_str2, startPos2, url_str,
                   loader_client2.GetWeakPtr());

  HeapVector<Member<VideoPriority>> videoVec;
  auto* videoPri1 = MakeGarbageCollected<VideoPriority>();
  videoPri1->setId("html5_api1");
  videoPri1->setPriority(UINT16_MAX);

  auto* videoPri2 = MakeGarbageCollected<VideoPriority>();
  videoPri2->setId("html5_api2");
  videoPri2->setPriority(UINT16_MAX);

  videoVec.push_back(videoPri1);
  videoVec.push_back(videoPri2);

  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 0);
  EXPECT_EQ(url_loader_manager_.pending_list_.size(), 2);
}

TEST_F(VideoUrlLoaderManagerTest, PriorityLoader_AsHumanReadableString_Test) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader;

  int64_t start = 1000;
  std::string video_id = "test_video";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request(url);

  PriorityLoader pri_loader(url_loader.GetWeakPtr(), video_id, start,
                            std::move(request), loader_client.GetWeakPtr());
  pri_loader.priority_ = 12345;

  std::string readable = pri_loader.AsHumanReadableString();
  EXPECT_TRUE(readable.find("pri=12345") != std::string::npos);
  EXPECT_TRUE(readable.find("start=1000") != std::string::npos);
  EXPECT_TRUE(readable.find("hash") != std::string::npos);
}

TEST_F(VideoUrlLoaderManagerTest,
       PriorityLoader_OperatorLess_PriorityComparison_Test) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader;

  int64_t start1 = 0, start2 = 0;
  std::string video_id1 = "video1", video_id2 = "video2";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request1(url);
  WebURLRequest request2(url);

  PriorityLoader pri_loader1(url_loader.GetWeakPtr(), video_id1, start1,
                             std::move(request1), loader_client.GetWeakPtr());
  PriorityLoader pri_loader2(url_loader.GetWeakPtr(), video_id2, start2,
                             std::move(request2), loader_client.GetWeakPtr());

  pri_loader1.priority_ = 100;
  pri_loader2.priority_ = 200;

  EXPECT_TRUE(pri_loader1 < pri_loader2);
  EXPECT_FALSE(pri_loader2 < pri_loader1);
}

TEST_F(VideoUrlLoaderManagerTest,
       PriorityLoader_OperatorLess_SamePriorityDifferentId_Test) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader;

  int64_t start1 = 0, start2 = 0;
  std::string video_id1 = "video1", video_id2 = "video2";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request1(url);
  WebURLRequest request2(url);

  PriorityLoader pri_loader1(url_loader.GetWeakPtr(), video_id1, start1,
                             std::move(request1), loader_client.GetWeakPtr());
  PriorityLoader pri_loader2(url_loader.GetWeakPtr(), video_id2, start2,
                             std::move(request2), loader_client.GetWeakPtr());

  pri_loader1.priority_ = 100;
  pri_loader2.priority_ = 100;

  EXPECT_FALSE(pri_loader1 < pri_loader2);
  EXPECT_FALSE(pri_loader2 < pri_loader1);
}

TEST_F(VideoUrlLoaderManagerTest,
       PriorityLoader_OperatorLess_SameIdStartComparison_Test) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader;

  int64_t start1 = 500, start2 = 1000;
  std::string video_id = "same_video";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request1(url);
  WebURLRequest request2(url);

  PriorityLoader pri_loader1(url_loader.GetWeakPtr(), video_id, start1,
                             std::move(request1), loader_client.GetWeakPtr());
  PriorityLoader pri_loader2(url_loader.GetWeakPtr(), video_id, start2,
                             std::move(request2), loader_client.GetWeakPtr());

  pri_loader1.priority_ = 100;
  pri_loader2.priority_ = 100;

  EXPECT_TRUE(pri_loader1 < pri_loader2);
  EXPECT_FALSE(pri_loader2 < pri_loader1);
}

TEST_F(VideoUrlLoaderManagerTest,
       UpdatePriority4EnsurePlaying_PriorityMismatchInExecuting_Test) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader;

  int64_t start = 0;
  std::string video_id = "test_video";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request(url);

  PriorityLoader pri_loader(url_loader.GetWeakPtr(), video_id, start,
                            std::move(request), loader_client.GetWeakPtr());
  pri_loader.priority_ = 500;

  url_loader_manager_.executing_list_.push_back(std::move(pri_loader));

  size_t size_before = url_loader_manager_.executing_list_.size();
  url_loader_manager_.UpdatePriority4EnsurePlaying(video_id, UINT16_MAX, 0);
  EXPECT_EQ(url_loader_manager_.executing_list_.size(), size_before);
}

TEST_F(VideoUrlLoaderManagerTest,
       UpdatePriority4EnsurePlaying_PriorityMismatchInPending_Test) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader;

  int64_t start = 0;
  std::string video_id = "test_video";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request(url);

  PriorityLoader pri_loader(url_loader.GetWeakPtr(), video_id, start,
                            std::move(request), loader_client.GetWeakPtr());
  pri_loader.priority_ = 500;

  url_loader_manager_.pending_list_.push_back(std::move(pri_loader));

  size_t size_before = url_loader_manager_.pending_list_.size();
  url_loader_manager_.UpdatePriority4EnsurePlaying(video_id, UINT16_MAX, 0);
  EXPECT_EQ(url_loader_manager_.pending_list_.size(), size_before);
}

TEST_F(VideoUrlLoaderManagerTest,
       UpdatePriority4EnsurePlaying_NullLoaderInExecutingList_Test) {
  std::string video_id = "test_video";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request(url);
  base::WeakPtr<WebAssociatedURLLoaderClient> null_client;

  PriorityLoader pri_loader(base::WeakPtr<VideoURLLoaderImpl>(), video_id, 0,
                            std::move(request), null_client);
  pri_loader.priority_ = UINT16_MAX;

  url_loader_manager_.executing_list_.push_back(std::move(pri_loader));

  url_loader_manager_.UpdatePriority4EnsurePlaying(video_id, UINT16_MAX, 0);
  EXPECT_TRUE(url_loader_manager_.executing_list_.empty());
}

TEST_F(VideoUrlLoaderManagerTest,
       UpdatePriority4EnsurePlaying_NullLoaderInPendingList_Test) {
  std::string video_id = "test_video";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request(url);
  base::WeakPtr<WebAssociatedURLLoaderClient> null_client;

  PriorityLoader pri_loader(base::WeakPtr<VideoURLLoaderImpl>(), video_id, 0,
                            std::move(request), null_client);
  pri_loader.priority_ = UINT16_MAX;

  url_loader_manager_.pending_list_.push_back(std::move(pri_loader));

  url_loader_manager_.UpdatePriority4EnsurePlaying(video_id, UINT16_MAX, 0);
  EXPECT_TRUE(url_loader_manager_.pending_list_.empty());
}

TEST_F(VideoUrlLoaderManagerTest, SetVideoPriority_SkipPlayingPriority_Test) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader;

  int64_t start = 0;
  std::string video_id = "playing_video";
  std::string url_str = "http://www.test.com/video.mp4";
  AddUrlLoaderTest(url_loader.GetWeakPtr(), video_id, start, url_str,
                   loader_client.GetWeakPtr());

  HeapVector<Member<VideoPriority>> videoVec;
  auto* videoPri = MakeGarbageCollected<VideoPriority>();
  videoPri->setId("playing_video");
  videoPri->setPriority(0);

  videoVec.push_back(videoPri);

  url_loader_manager_.SetVideoPriority(videoVec);
  EXPECT_FALSE(url_loader_manager_.loader_priority_map_.count(video_id));
}

TEST_F(VideoUrlLoaderManagerTest,
       StartPendingLoaderIfNeeded_InvalidLoaderCleanup_Test) {
  std::string video_id = "test_video";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request(url);
  MockWebAssociatedURLLoaderClient loader_client;

  PriorityLoader pri_loader(base::WeakPtr<VideoURLLoaderImpl>(), video_id, 0,
                            std::move(request), loader_client.GetWeakPtr());
  pri_loader.priority_ = 100;

  url_loader_manager_.pending_list_.push_back(std::move(pri_loader));

  url_loader_manager_.StartPendingLoaderIfNeeded();
  EXPECT_TRUE(url_loader_manager_.pending_list_.empty());
}

TEST_F(VideoUrlLoaderManagerTest,
       StartPendingLoaderIfNeeded_InvalidClientCleanup_Test) {
  MockVideoURLLoaderImpl url_loader;
  std::string video_id = "test_video";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request(url);
  base::WeakPtr<WebAssociatedURLLoaderClient> null_client;

  PriorityLoader pri_loader(url_loader.GetWeakPtr(), video_id, 0,
                            std::move(request), null_client);
  pri_loader.priority_ = 100;

  url_loader_manager_.pending_list_.push_back(std::move(pri_loader));

  url_loader_manager_.StartPendingLoaderIfNeeded();
  EXPECT_TRUE(url_loader_manager_.pending_list_.empty());
}

TEST_F(VideoUrlLoaderManagerTest,
       StartPendingLoaderIfNeeded_DefaultPriorityReturn_Test) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader;

  int64_t start = 0;
  std::string video_id = "test_video";
  std::string url_str = "http://www.test.com/video.mp4";
  AddUrlLoaderTest(url_loader.GetWeakPtr(), video_id, start, url_str,
                   loader_client.GetWeakPtr());

  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 0);
  url_loader_manager_.StartPendingLoaderIfNeeded();
  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 0);
}

TEST_F(VideoUrlLoaderManagerTest,
       StartPendingLoaderIfNeeded_ExecutingHigherPriority_Test) {
  MockWebAssociatedURLLoaderClient loader_client1;
  MockVideoURLLoaderImpl url_loader1;
  int64_t start1 = 0;
  std::string video_id1 = "high_priority";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request1(url);
  WebURLRequest request2(url);

  PriorityLoader pri_loader1(url_loader1.GetWeakPtr(), video_id1, start1,
                             std::move(request1), loader_client1.GetWeakPtr());
  pri_loader1.priority_ = 50;
  url_loader_manager_.executing_list_.push_back(std::move(pri_loader1));

  MockWebAssociatedURLLoaderClient loader_client2;
  MockVideoURLLoaderImpl url_loader2;
  int64_t start2 = 0;
  std::string video_id2 = "medium_priority";
  PriorityLoader pri_loader2(url_loader2.GetWeakPtr(), video_id2, start2,
                             std::move(request2), loader_client2.GetWeakPtr());
  pri_loader2.priority_ = 100;
  url_loader_manager_.pending_list_.push_back(std::move(pri_loader2));

  url_loader_manager_.StartPendingLoaderIfNeeded();
  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 1);
  EXPECT_EQ(url_loader_manager_.executing_list_.front().priority_, 50);
  EXPECT_EQ(url_loader_manager_.pending_list_.size(), 1);
}

TEST_F(VideoUrlLoaderManagerTest, RemoveLoaderFromList_NullLoader_Test) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader;

  int64_t start = 0;
  std::string video_id = "test_video";
  std::string url_str = "http://www.test.com/video.mp4";
  AddUrlLoaderTest(url_loader.GetWeakPtr(), video_id, start, url_str,
                   loader_client.GetWeakPtr());

  size_t size_before = url_loader_manager_.pending_list_.size();
  bool removed = url_loader_manager_.RemoveLoaderFromList(
      &url_loader_manager_.pending_list_, base::WeakPtr<VideoURLLoaderImpl>());

  EXPECT_FALSE(removed);
  EXPECT_EQ(url_loader_manager_.pending_list_.size(), size_before);
}

TEST_F(VideoUrlLoaderManagerTest,
       RemoveUrlLoader_FromExecutingList_PostsTask_Test) {
  MockWebAssociatedURLLoaderClient loader_client;
  MockVideoURLLoaderImpl url_loader;

  int64_t start = 0;
  std::string video_id = "test_video";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request(url);

  PriorityLoader pri_loader(url_loader.GetWeakPtr(), video_id, start,
                            std::move(request), loader_client.GetWeakPtr());
  pri_loader.priority_ = 100;
  url_loader_manager_.executing_list_.push_back(std::move(pri_loader));

  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 1);
  url_loader_manager_.RemoveUrlLoader(url_loader.GetWeakPtr());

  RunPendingTasks();
  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 0);
}

TEST_F(VideoUrlLoaderManagerTest, UpdateUrlLoader_NullLoaderCleanup_Test) {
  std::string video_id = "test_video";
  std::string url_str = "http://www.test.com/video.mp4";
  KURL url = ToKURL(url_str);
  WebURLRequest request(url);
  MockWebAssociatedURLLoaderClient loader_client;

  PriorityLoader pri_loader1(base::WeakPtr<VideoURLLoaderImpl>(), video_id, 0,
                             std::move(request), loader_client.GetWeakPtr());
  pri_loader1.priority_ = 100;

  MockVideoURLLoaderImpl url_loader2;
  PriorityLoader pri_loader2(url_loader2.GetWeakPtr(), video_id, 0,
                             WebURLRequest(ToKURL(url_str)),
                             loader_client.GetWeakPtr());
  pri_loader2.priority_ = 200;

  url_loader_manager_.executing_list_.push_back(std::move(pri_loader1));
  url_loader_manager_.executing_list_.push_back(std::move(pri_loader2));

  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 2);
  url_loader_manager_.UpdateUrlLoader();
  EXPECT_EQ(url_loader_manager_.executing_list_.size(), 1);
}
}  // namespace blink