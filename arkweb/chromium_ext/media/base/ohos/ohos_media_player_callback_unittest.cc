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
#include "base/run_loop.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#define protected public
#include "ohos_media_player_bridge.h"
#include "ohos_media_player_callback.h"
#undef protected
#undef private

using namespace media;
using namespace testing;
using namespace OHOS::NWeb;
using namespace base;

namespace media {
using namespace OHOS::NWeb;

class MockClient : public OHOSMediaPlayerBridge::Client {
 public:
  virtual ~MockClient() {}
  MOCK_METHOD(void,
              OnFrameAvailable,
              (int, uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t),
              (override));
  MOCK_METHOD(void, OnMediaDurationChanged, (base::TimeDelta), (override));
  MOCK_METHOD(void, OnPlaybackComplete, (), (override));
  MOCK_METHOD(void, OnError, (int), (override));
  MOCK_METHOD(void, OnVideoSizeChanged, (int, int), (override));
  MOCK_METHOD(void, OnPlayerInterruptEvent, (int32_t), (override));
  MOCK_METHOD(void, OnAudioStateChanged, (bool), (override));
  MOCK_METHOD(void, OnPlayerSeekBack, (base::TimeDelta), (override));
  MOCK_METHOD(OHOSMediaResourceGetter*, GetMediaResourceGetter, (), (override));
};

class MockSingleThreadTaskRunner : public base::SingleThreadTaskRunner {
 public:
  MOCK_METHOD(bool, RunsTasksInCurrentSequence, (), (const, override));
  MOCK_METHOD(bool,
              PostNonNestableDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));
  MOCK_METHOD(bool,
              PostDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));
  MOCK_METHOD2(PostTask, bool(const base::Location&, base::OnceClosure));
};

class OHOSMediaPlayerCallbackTest : public ::testing::Test {
 public:
  OHOSMediaPlayerCallbackTest() = default;
  ~OHOSMediaPlayerCallbackTest() override = default;

  void SetUp() override {
    gurl_ = std::make_unique<GURL>("http://example.com");
    mock_client_ = std::make_unique<MockClient>();

    bridge_ = new OHOSMediaPlayerBridge(
        *gurl_.get(), site_for_cookies_, top_frame_origin_, user_agent_, storage_access_api_status_,
        hide_url_log_, mock_client_.get(), allow_credentials_, is_hls_, headers_, grantMediaFileAccessDirs_);

    mock_task_runner_ = base::MakeRefCounted<MockSingleThreadTaskRunner>();
    EXPECT_CALL(*mock_task_runner_, PostDelayedTask(_, _, _))
        .WillRepeatedly([](const base::Location& from_here, base::OnceClosure task, base::TimeDelta delay) {
            std::move(task).Run();
            return true;
        });
    media_player_callback_ =
        std::make_unique<OHOSMediaPlayerCallback>(std::move(mock_task_runner_), bridge_->weak_factory_.GetWeakPtr());
  }

  void TearDown() override {
    delete bridge_;
    bridge_ = nullptr;
    gurl_.reset();
    mock_client_.reset();
  }

  std::unique_ptr<OHOSMediaPlayerCallback> media_player_callback_;
  OHOSMediaPlayerBridge* bridge_;
  net::SiteForCookies site_for_cookies_;
  url::Origin top_frame_origin_;
  std::string user_agent_{"test"};
  net::StorageAccessApiStatus storage_access_api_status_ = net::StorageAccessApiStatus::kAccessViaAPI;
  bool hide_url_log_ = false;
  bool allow_credentials_ = true;
  bool is_hls_ = false;
  base::flat_map<std::string, std::string> headers_;
  std::vector<std::string> grantMediaFileAccessDirs_;
  std::unique_ptr<GURL> gurl_;
  std::unique_ptr<MockClient> mock_client_;
  scoped_refptr<MockSingleThreadTaskRunner> mock_task_runner_;
};

TEST_F(OHOSMediaPlayerCallbackTest, TestItem1) {
  media_player_callback_->media_player_ = bridge_->weak_factory_.GetWeakPtr();

  EXPECT_CALL(*mock_client_, OnError(OHOSMediaPlayerBridge::MediaErrorType::MEDIA_ERROR_INVALID_CODE)).Times(1);
  media_player_callback_->OnError(OHOS::NWeb::PlayerAdapterErrorType::INVALID_CODE);

  EXPECT_CALL(*mock_client_, OnError(OHOSMediaPlayerBridge::MediaErrorType::MEDIA_ERROR_FORMAT)).Times(1);
  media_player_callback_->OnError(OHOS::NWeb::PlayerAdapterErrorType::UNSUPPORT_TYPE);

  EXPECT_CALL(*mock_client_, OnError(OHOSMediaPlayerBridge::MediaErrorType::MEDIA_ERROR_DECODE)).Times(1);
  media_player_callback_->OnError(OHOS::NWeb::PlayerAdapterErrorType::FATAL_ERROR);
}

TEST_F(OHOSMediaPlayerCallbackTest, TestItem2) {
  constexpr int32_t TEST_EXTRA = 5;
  constexpr int32_t TEST_VALUE = 5;
  media_player_callback_->media_player_ = bridge_->weak_factory_.GetWeakPtr();

  media_player_callback_->OnInfo(OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_UNSET, TEST_EXTRA, TEST_VALUE);
  media_player_callback_->OnInfo(OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_MESSAGE, TEST_EXTRA, TEST_VALUE);

  EXPECT_CALL(*mock_client_, OnPlaybackComplete()).Times(1);
  media_player_callback_->OnInfo(OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_EOS, TEST_EXTRA, TEST_VALUE);

  media_player_callback_->media_player_->player_ = OHOS::NWeb::OhosAdapterHelper::GetInstance().CreatePlayerAdapter();
  media_player_callback_->OnInfo(OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_STATE_CHANGE, TEST_EXTRA, TEST_VALUE);
  ASSERT_EQ(media_player_callback_->media_player_->player_state_, TEST_EXTRA);

  media_player_callback_->OnInfo(OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_POSITION_UPDATE, TEST_EXTRA, TEST_VALUE);
  ASSERT_EQ(media_player_callback_->media_player_->extra_time_, base::Milliseconds(TEST_EXTRA));

  EXPECT_CALL(*mock_client_, OnPlayerInterruptEvent(_)).Times(1);
  media_player_callback_->OnInfo(OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_INTERRUPT_EVENT, TEST_EXTRA, TEST_VALUE);

  media_player_callback_->OnInfo(OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_SEEKDONE, TEST_EXTRA, TEST_VALUE);
  ASSERT_EQ(media_player_callback_->media_player_->seek_complete_, true);
  
  EXPECT_CALL(*mock_client_, OnVideoSizeChanged(_, _)).Times(1);
  media_player_callback_->OnInfo(OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_RESOLUTION_CHANGE, TEST_EXTRA, TEST_VALUE);
}

}  // namespace media
