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

#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_audio_system_manager_adapter.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#include "base/logging.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "arkweb/chromium_ext/content/browser/media/session/audio_focus_delegate_ohos.h"
#undef private

using ::testing::_;
using ::testing::Return;
using ::testing::NotNull;

namespace content {

class AudioFocusDelegateOHOSTest : public RenderViewHostTestHarness {
 public:
  AudioFocusDelegateOHOSTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

  ~AudioFocusDelegateOHOSTest() override = default;

  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    media_session_ = MediaSessionImpl::Get(web_contents());
    delegate_ = std::make_unique<AudioFocusDelegateOHOS>(media_session_);
  }

  void TearDown() override {
    delegate_.reset();
    RenderViewHostTestHarness::TearDown();
  }
  raw_ptr<MediaSessionImpl> media_session_;
  std::unique_ptr<AudioFocusDelegateOHOS> delegate_;
};

TEST_F(AudioFocusDelegateOHOSTest, RequestAudioFocus1) {
  auto result =
      delegate_->RequestAudioFocus(media_session::mojom::AudioFocusType::kGain);
  EXPECT_EQ(result, AudioFocusDelegate::AudioFocusResult::kSuccess);
  result =
      delegate_->RequestAudioFocus(media_session::mojom::AudioFocusType::kGain);
  EXPECT_EQ(result, AudioFocusDelegate::AudioFocusResult::kSuccess);
}

TEST_F(AudioFocusDelegateOHOSTest, RequestAudioFocus2) {
  OHOS::NWeb::MockOhosAdapterHelper* instance =
      new OHOS::NWeb::MockOhosAdapterHelper();
  OHOS::NWeb::OhosAdapterHelper::SetInstance(instance);
  MockAudioSystemManagerAdapter adapter;

  EXPECT_CALL(*instance, GetAudioSystemManager())
      .WillRepeatedly(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, RequestAudioFocus(NotNull()))
      .WillRepeatedly(Return(OHOS::NWeb::AudioAdapterCode::AUDIO_ERROR));

  auto result =
      delegate_->RequestAudioFocus(media_session::mojom::AudioFocusType::kGain);
  EXPECT_EQ(result, AudioFocusDelegate::AudioFocusResult::kFailed);
  OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(AudioFocusDelegateOHOSTest, RequestAudioFocus3) {
  OHOS::NWeb::MockOhosAdapterHelper* instance =
      new OHOS::NWeb::MockOhosAdapterHelper();
  OHOS::NWeb::OhosAdapterHelper::SetInstance(instance);
  MockAudioSystemManagerAdapter adapter;

  EXPECT_CALL(*instance, GetAudioSystemManager())
      .WillRepeatedly(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, SetAudioManagerInterruptCallback(NotNull()))
      .WillRepeatedly(Return(OHOS::NWeb::AudioAdapterCode::AUDIO_ERROR));

  auto result =
      delegate_->RequestAudioFocus(media_session::mojom::AudioFocusType::kGain);
  EXPECT_EQ(result, AudioFocusDelegate::AudioFocusResult::kFailed);
  OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

}  // namespace content