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
#include "base/test/task_environment.h"
#include "content/public/browser/native_embed_info.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"
#include "mojo/public/cpp/bindings/pending_associated_receiver.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"
#define private public
#include "content/browser/media/ohos/native_web_contents_observer.h"
#define ARKWEB_TEST_INCLUDE
#include "content/browser/media/ohos/native_web_contents_observer.cc"
#undef private

using ::testing::_;
using ::testing::StrictMock;

namespace content {

class MockNativeBridgeObserver : public media::mojom::NativeBridgeObserver {
 public:
  MockNativeBridgeObserver() = default;
  ~MockNativeBridgeObserver() override = default;

  MOCK_METHOD(void,
              OnCreateNativeSurface,
              (media::mojom::NativeEmbedInfoPtr),
              (override));
  MOCK_METHOD(void,
              OnNativeEmbedFirstFramePaint,
              (int32_t, const std::string&),
              (override));
  MOCK_METHOD(void, OnEmbedRectChange, (const gfx::Rect&), (override));
  MOCK_METHOD(void, OnDestroyNativeSurface, (), (override));
  MOCK_METHOD(void, OnLayerRectVisibilityChange, (bool, int), (override));
  MOCK_METHOD(void,
              OnEmbedObjectParamChange,
              (media::mojom::NativeEmbedParamChangeInfoPtr),
              (override));
};

class NativeWebContentsObserverTest : public RenderViewHostTestHarness {
 public:
  NativeWebContentsObserverTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    web_contents_ = static_cast<WebContentsImpl*>(web_contents());
    ASSERT_NE(web_contents_, nullptr);

    observer_ = std::make_unique<NativeWebContentsObserver>(web_contents_);
    ASSERT_NE(observer_, nullptr);
  }

  void TearDown() override {
    observer_.reset();
    RenderViewHostTestHarness::TearDown();
  }

  void CreateNativeBridgeHostImpl() {
    GlobalRenderFrameHostId frame_id = main_rfh()->GetGlobalId();
    host_impl_ =
        std::make_unique<NativeWebContentsObserver::NativeBridgeHostImpl>(
            frame_id, observer_.get());
    ASSERT_NE(host_impl_, nullptr);
  }

  WebContentsImpl* web_contents_ = nullptr;
  std::unique_ptr<NativeWebContentsObserver> observer_;
  std::unique_ptr<NativeWebContentsObserver::NativeBridgeHostImpl> host_impl_;
};

TEST_F(NativeWebContentsObserverTest, OnNativeBridgeAdded1) {
  CreateNativeBridgeHostImpl();
  host_impl_->native_web_contents_observer_ = nullptr;
  MockNativeBridgeObserver mock_observer;
  mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeObserver> receiver;
  ASSERT_NO_FATAL_FAILURE(
      host_impl_->OnNativeBridgeAdded(std::move(receiver), 123));
  EXPECT_TRUE(observer_->native_bridge_observer_hosts_.empty());
}

TEST_F(NativeWebContentsObserverTest, OnNativeBridgeAdded2) {
  CreateNativeBridgeHostImpl();
  ASSERT_NE(host_impl_->native_web_contents_observer_, nullptr);
  MockNativeBridgeObserver mock_observer;
  mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeObserver> receiver;
  auto remote = receiver.InitWithNewEndpointAndPassRemote();
  EXPECT_TRUE(remote.is_valid());
  ASSERT_NO_FATAL_FAILURE(
      host_impl_->OnNativeBridgeAdded(std::move(receiver), 456));

  MediaPlayerId expected_id(main_rfh()->GetGlobalId(), 456);
  auto it = observer_->native_bridge_observer_hosts_.find(expected_id);
  EXPECT_NE(it, observer_->native_bridge_observer_hosts_.end());
  EXPECT_NE(it->second.get(), nullptr);
}

TEST_F(NativeWebContentsObserverTest, OnCreateNativeSurface1) {
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, nullptr);

  auto embed_info = media::mojom::NativeEmbedInfo::New();
  embed_info->embed_id = 456;
  embed_info->element_id = "test_element";
  embed_info->type = "video";
  embed_info->source = "https://example";
  embed_info->tag = "video";
  embed_info->rect = gfx::Rect(0, 0, 100, 100);
  embed_info->params["param1"] = "value1";

  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnCreateNativeSurface(std::move(embed_info)));
}

TEST_F(NativeWebContentsObserverTest, OnCreateNativeSurface2) {
  auto empty_observer = std::make_unique<NativeWebContentsObserver>(nullptr);
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, empty_observer.get());

  auto embed_info = media::mojom::NativeEmbedInfo::New();
  embed_info->embed_id = 456;
  embed_info->element_id = "test_element";
  embed_info->type = "video";
  embed_info->source = "https://example";
  embed_info->tag = "video";
  embed_info->rect = gfx::Rect(0, 0, 100, 100);
  embed_info->params["param1"] = "value1";

  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnCreateNativeSurface(std::move(embed_info)));
  EXPECT_TRUE(empty_observer->bridge_info_map_.empty());
}

TEST_F(NativeWebContentsObserverTest, OnCreateNativeSurface3) {
  CreateNativeBridgeHostImpl();
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, observer_.get());

  auto embed_info = media::mojom::NativeEmbedInfo::New();
  embed_info->embed_id = 456;
  embed_info->element_id = "test_element";
  embed_info->type = "video";
  embed_info->source = "https://example";
  embed_info->tag = "video";
  embed_info->rect = gfx::Rect(0, 0, 100, 100);

  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnCreateNativeSurface(std::move(embed_info)));
  auto it = observer_->bridge_info_map_.find(test_id);
  EXPECT_NE(it, observer_->bridge_info_map_.end());
  EXPECT_NE(it->second.get(), nullptr);

  const auto& native_embed_info = it->second->native_embed_info();
  EXPECT_EQ(native_embed_info.native_embed_id, 456);
  EXPECT_EQ(native_embed_info.embed_element_id, "test_element");
  EXPECT_EQ(native_embed_info.native_type, "video");
  EXPECT_EQ(native_embed_info.native_source, "https://example");
  EXPECT_EQ(native_embed_info.tag, "video");
  EXPECT_EQ(native_embed_info.rect, gfx::Rect(0, 0, 100, 100));
  EXPECT_TRUE(native_embed_info.params.empty());
}

TEST_F(NativeWebContentsObserverTest, OnCreateNativeSurface4) {
  CreateNativeBridgeHostImpl();

  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, observer_.get());

  auto embed_info = media::mojom::NativeEmbedInfo::New();
  embed_info->embed_id = 789;
  embed_info->element_id = "test_element2";
  embed_info->type = "audio";
  embed_info->source = "https://example";
  embed_info->tag = "audio";
  embed_info->rect = gfx::Rect(10, 10, 200, 50);

  embed_info->params["autoplay"] = "true";
  embed_info->params["controls"] = "false";
  embed_info->params["loop"] = "true";
  embed_info->params["muted"] = "false";

  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnCreateNativeSurface(std::move(embed_info)));
  auto it = observer_->bridge_info_map_.find(test_id);
  EXPECT_NE(it, observer_->bridge_info_map_.end());
  EXPECT_NE(it->second.get(), nullptr);
  const auto& native_embed_info = it->second->native_embed_info();
  EXPECT_EQ(native_embed_info.native_embed_id, 789);
  EXPECT_EQ(native_embed_info.embed_element_id, "test_element2");
  EXPECT_EQ(native_embed_info.native_type, "audio");
  EXPECT_EQ(native_embed_info.native_source, "https://example");
  EXPECT_EQ(native_embed_info.tag, "audio");
  EXPECT_EQ(native_embed_info.rect, gfx::Rect(10, 10, 200, 50));
}

TEST_F(NativeWebContentsObserverTest, OnNativeEmbedFirstFramePaint1) {
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, nullptr);
  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnNativeEmbedFirstFramePaint(456, "test_attribute"));
}

TEST_F(NativeWebContentsObserverTest, OnNativeEmbedFirstFramePaint2) {
  auto empty_observer = std::make_unique<NativeWebContentsObserver>(nullptr);
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, empty_observer.get());
  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnNativeEmbedFirstFramePaint(456, "test_attribute"));
}

TEST_F(NativeWebContentsObserverTest, OnNativeEmbedFirstFramePaint3) {
  CreateNativeBridgeHostImpl();
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, observer_.get());
  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnNativeEmbedFirstFramePaint(456, "test_attribute"));
}

TEST_F(NativeWebContentsObserverTest, OnDestroyNativeSurface1) {
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, nullptr);
  ASSERT_NO_FATAL_FAILURE(observer_host->OnDestroyNativeSurface());
}

TEST_F(NativeWebContentsObserverTest, OnDestroyNativeSurface2) {
  auto empty_observer = std::make_unique<NativeWebContentsObserver>(nullptr);
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, empty_observer.get());
  ASSERT_NO_FATAL_FAILURE(observer_host->OnDestroyNativeSurface());
}

TEST_F(NativeWebContentsObserverTest, OnDestroyNativeSurface3) {
  CreateNativeBridgeHostImpl();
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, observer_.get());
  EXPECT_TRUE(observer_->bridge_info_map_.empty());
  ASSERT_NO_FATAL_FAILURE(observer_host->OnDestroyNativeSurface());
  EXPECT_TRUE(observer_->bridge_info_map_.empty());
}

TEST_F(NativeWebContentsObserverTest, OnDestroyNativeSurface4) {
  CreateNativeBridgeHostImpl();
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, observer_.get());
  auto embed_info = media::mojom::NativeEmbedInfo::New();
  embed_info->embed_id = 456;
  embed_info->element_id = "test_element";
  embed_info->type = "video";
  embed_info->source = "https://example";
  embed_info->tag = "video";
  embed_info->rect = gfx::Rect(0, 0, 100, 100);
  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnCreateNativeSurface(std::move(embed_info)));
  EXPECT_EQ(observer_->bridge_info_map_.size(), 1u);
  ASSERT_NO_FATAL_FAILURE(observer_host->OnDestroyNativeSurface());
  EXPECT_TRUE(observer_->bridge_info_map_.empty());
}

TEST_F(NativeWebContentsObserverTest, OnLayerRectVisibilityChange1) {
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, nullptr);
  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnLayerRectVisibilityChange(true, 456));
}

TEST_F(NativeWebContentsObserverTest, OnLayerRectVisibilityChange2) {
  auto empty_observer = std::make_unique<NativeWebContentsObserver>(nullptr);
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, empty_observer.get());
  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnLayerRectVisibilityChange(false, 789));
}

TEST_F(NativeWebContentsObserverTest, OnLayerRectVisibilityChange3) {
  CreateNativeBridgeHostImpl();
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, observer_.get());
  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnLayerRectVisibilityChange(true, 456));
}

TEST_F(NativeWebContentsObserverTest, OnEmbedRectChange1) {
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, nullptr);
  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnEmbedRectChange(gfx::Rect(10, 10, 100, 100)));
}

TEST_F(NativeWebContentsObserverTest, OnEmbedRectChange2) {
  auto empty_observer = std::make_unique<NativeWebContentsObserver>(nullptr);
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, empty_observer.get());
  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnEmbedRectChange(gfx::Rect(20, 20, 200, 200)));
}

TEST_F(NativeWebContentsObserverTest, OnEmbedRectChange3) {
  CreateNativeBridgeHostImpl();
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, observer_.get());
  EXPECT_TRUE(observer_->bridge_info_map_.empty());
  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnEmbedRectChange(gfx::Rect(30, 30, 300, 300)));
  EXPECT_TRUE(observer_->bridge_info_map_.empty());
}

TEST_F(NativeWebContentsObserverTest, OnEmbedRectChange4) {
  CreateNativeBridgeHostImpl();
  MediaPlayerId test_id(main_rfh()->GetGlobalId(), 123);
  auto observer_host =
      std::make_unique<NativeWebContentsObserver::NativeBridgeObserverHostImpl>(
          test_id, observer_.get());
  auto embed_info = media::mojom::NativeEmbedInfo::New();
  embed_info->embed_id = 456;
  embed_info->element_id = "test_element";
  embed_info->type = "video";
  embed_info->source = "https://example";
  embed_info->tag = "video";
  embed_info->rect = gfx::Rect(0, 0, 100, 100);
  ASSERT_NO_FATAL_FAILURE(
      observer_host->OnCreateNativeSurface(std::move(embed_info)));
  EXPECT_EQ(observer_->bridge_info_map_.size(), 1u);
  auto it = observer_->bridge_info_map_.find(test_id);
  EXPECT_NE(it, observer_->bridge_info_map_.end());
  EXPECT_NE(it->second.get(), nullptr);
  gfx::Rect new_rect(50, 50, 200, 200);
  observer_host->OnEmbedRectChange(new_rect);
  EXPECT_EQ(observer_->bridge_info_map_.size(), 1u);
  it = observer_->bridge_info_map_.find(test_id);
  EXPECT_NE(it, observer_->bridge_info_map_.end());
  EXPECT_NE(it->second.get(), nullptr);
}

TEST_F(NativeWebContentsObserverTest, BindNativeBridgeHost1) {
  GlobalRenderFrameHostId new_frame_id = main_rfh()->GetGlobalId();
  EXPECT_TRUE(observer_->native_bridge_hosts_.empty());
  mojo::AssociatedRemote<media::mojom::NativeBridgeHost> remote;
  auto receiver = remote.BindNewEndpointAndPassReceiver();
  ASSERT_NO_FATAL_FAILURE(
      observer_->BindNativeBridgeHost(new_frame_id, std::move(receiver)));
  EXPECT_EQ(observer_->native_bridge_hosts_.size(), 1u);
  auto it = observer_->native_bridge_hosts_.find(new_frame_id);
  EXPECT_NE(it, observer_->native_bridge_hosts_.end());
  EXPECT_NE(it->second.get(), nullptr);
}

TEST_F(NativeWebContentsObserverTest, BindNativeBridgeHost2) {
  GlobalRenderFrameHostId frame_id = main_rfh()->GetGlobalId();
  observer_->native_bridge_hosts_[frame_id] =
      std::make_unique<NativeWebContentsObserver::NativeBridgeHostImpl>(
          frame_id, observer_.get());
  EXPECT_EQ(observer_->native_bridge_hosts_.size(), 1u);
  mojo::AssociatedRemote<media::mojom::NativeBridgeHost> remote;
  auto receiver = remote.BindNewEndpointAndPassReceiver();
  ASSERT_NO_FATAL_FAILURE(
      observer_->BindNativeBridgeHost(frame_id, std::move(receiver)));
  EXPECT_EQ(observer_->native_bridge_hosts_.size(), 1u);
}

TEST_F(NativeWebContentsObserverTest, BindNativeBridgeHost3) {
  GlobalRenderFrameHostId id1 = main_rfh()->GetGlobalId();
  GlobalRenderFrameHostId id2(1, 2);
  GlobalRenderFrameHostId id3(3, 4);
  mojo::AssociatedRemote<media::mojom::NativeBridgeHost> r1;
  mojo::AssociatedRemote<media::mojom::NativeBridgeHost> r2;
  mojo::AssociatedRemote<media::mojom::NativeBridgeHost> r3;
  auto rec1 = r1.BindNewEndpointAndPassReceiver();
  ASSERT_NO_FATAL_FAILURE(
      observer_->BindNativeBridgeHost(id1, std::move(rec1)));
  auto rec2 = r2.BindNewEndpointAndPassReceiver();
  ASSERT_NO_FATAL_FAILURE(
      observer_->BindNativeBridgeHost(id2, std::move(rec2)));
  auto rec3 = r3.BindNewEndpointAndPassReceiver();
  ASSERT_NO_FATAL_FAILURE(
      observer_->BindNativeBridgeHost(id3, std::move(rec3)));
  EXPECT_EQ(observer_->native_bridge_hosts_.size(), 3u);
}

TEST_F(NativeWebContentsObserverTest, OnRenderFrameHostEnterBackForwardCache1) {
  GlobalRenderFrameHostId frame_id = main_rfh()->GetGlobalId();
  EXPECT_TRUE(observer_->bridge_info_map_.empty());
  ASSERT_NO_FATAL_FAILURE(
      observer_->OnRenderFrameHostEnterBackForwardCache(frame_id));
}

TEST_F(NativeWebContentsObserverTest, OnRenderFrameHostEnterBackForwardCache2) {
  GlobalRenderFrameHostId frame_id = main_rfh()->GetGlobalId();
  MediaPlayerId test_id1(frame_id, 123);
  MediaPlayerId test_id2(frame_id, 456);
  MediaPlayerId test_id3(GlobalRenderFrameHostId(999, 999), 789);
  int id1 = 111;
  GURL url1("https://example1");
  std::string element1 = "element1";
  std::string type1 = "video";
  std::string source1 = "source1";
  std::string tag1 = "tag1";
  gfx::Rect rect1(0, 0, 100, 100);
  std::map<std::string, std::string> params1;
  content::NativeEmbedInfo embed_info1(id1, url1, element1, type1, source1,
                                       tag1, rect1, params1);
  int id2 = 222;
  GURL url2("https://example2");
  std::string element2 = "element2";
  std::string type2 = "audio";
  std::string source2 = "source2";
  std::string tag2 = "tag2";
  gfx::Rect rect2(10, 10, 200, 200);
  std::map<std::string, std::string> params2;

  content::NativeEmbedInfo embed_info2(id2, url2, element2, type2, source2,
                                       tag2, rect2, params2);
  observer_->bridge_info_map_[test_id1] =
      std::make_unique<NativeWebContentsObserver::BridgeInfo>(
          test_id1, embed_info1, observer_.get());
  observer_->bridge_info_map_[test_id2] =
      std::make_unique<NativeWebContentsObserver::BridgeInfo>(
          test_id2, embed_info2, observer_.get());
  EXPECT_EQ(observer_->bridge_info_map_.size(), 2u);
  ASSERT_NO_FATAL_FAILURE(
      observer_->OnRenderFrameHostEnterBackForwardCache(frame_id));
  EXPECT_EQ(observer_->bridge_info_map_.size(), 2u);
}

TEST_F(NativeWebContentsObserverTest, OnRenderFrameHostEnterBackForwardCache3) {
  auto empty_observer = std::make_unique<NativeWebContentsObserver>(nullptr);
  GlobalRenderFrameHostId frame_id = GlobalRenderFrameHostId(1, 2);
  MediaPlayerId test_id(frame_id, 123);
  int id1 = 111;
  GURL url1("https://example1");
  std::string element1 = "element1";
  std::string type1 = "video";
  std::string source1 = "source1";
  std::string tag1 = "tag1";
  gfx::Rect rect1(0, 0, 100, 100);
  std::map<std::string, std::string> params1;
  content::NativeEmbedInfo embed_info1(id1, url1, element1, type1, source1,
                                       tag1, rect1, params1);
  empty_observer->bridge_info_map_[test_id] =
      std::make_unique<NativeWebContentsObserver::BridgeInfo>(
          test_id, embed_info1, empty_observer.get());
  EXPECT_EQ(empty_observer->bridge_info_map_.size(), 1u);
  ASSERT_NO_FATAL_FAILURE(
      empty_observer->OnRenderFrameHostEnterBackForwardCache(frame_id));
  EXPECT_EQ(empty_observer->bridge_info_map_.size(), 1u);
}

TEST_F(NativeWebContentsObserverTest, OnRenderFrameHostEnterBackForwardCache4) {
  GlobalRenderFrameHostId frame_id = main_rfh()->GetGlobalId();
  GlobalRenderFrameHostId different_frame_id =
      GlobalRenderFrameHostId(999, 999);
  MediaPlayerId test_id(different_frame_id, 123);
  int id1 = 111;
  GURL url1("https://example1");
  std::string element1 = "element1";
  std::string type1 = "video";
  std::string source1 = "source1";
  std::string tag1 = "tag1";
  gfx::Rect rect1(0, 0, 100, 100);
  std::map<std::string, std::string> params1;
  content::NativeEmbedInfo embed_info1(id1, url1, element1, type1, source1,
                                       tag1, rect1, params1);

  observer_->bridge_info_map_[test_id] =
      std::make_unique<NativeWebContentsObserver::BridgeInfo>(
          test_id, embed_info1, observer_.get());
  EXPECT_EQ(observer_->bridge_info_map_.size(), 1u);
  ASSERT_NO_FATAL_FAILURE(
      observer_->OnRenderFrameHostEnterBackForwardCache(frame_id));
  EXPECT_EQ(observer_->bridge_info_map_.size(), 1u);
}

TEST_F(NativeWebContentsObserverTest, OnRenderFrameHostEnterBackForwardCache5) {
  auto empty_observer = std::make_unique<NativeWebContentsObserver>(nullptr);
  GlobalRenderFrameHostId frame_id = GlobalRenderFrameHostId(1, 2);
  GlobalRenderFrameHostId different_frame_id =
      GlobalRenderFrameHostId(999, 999);
  MediaPlayerId test_id(different_frame_id, 123);
  int id1 = 111;
  GURL url1("https://example1");
  std::string element1 = "element1";
  std::string type1 = "video";
  std::string source1 = "source1";
  std::string tag1 = "tag1";
  gfx::Rect rect1(0, 0, 100, 100);
  std::map<std::string, std::string> params1;
  content::NativeEmbedInfo embed_info1(id1, url1, element1, type1, source1,
                                       tag1, rect1, params1);
  empty_observer->bridge_info_map_[test_id] =
      std::make_unique<NativeWebContentsObserver::BridgeInfo>(
          test_id, embed_info1, empty_observer.get());
  EXPECT_EQ(empty_observer->bridge_info_map_.size(), 1u);
  ASSERT_NO_FATAL_FAILURE(
      empty_observer->OnRenderFrameHostEnterBackForwardCache(frame_id));
  EXPECT_EQ(empty_observer->bridge_info_map_.size(), 1u);
}

}  // namespace content
