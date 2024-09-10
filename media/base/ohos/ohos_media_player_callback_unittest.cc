/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "base/test/task_environment.h"

#define private public
#include "ohos_media_player_callback.h"
#undef private

using namespace media;
using namespace testing;
using namespace OHOS::NWeb;
using namespace  base;

class OHOSMediaPlayerCallbackTest : public ::testing::Test {
 protected:
  void SetUp() override {
    task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
    media_player_callback_ = std::make_unique<OHOSMediaPlayerCallback>(task_runner,media_player);
  }

  void TearDown() override {
    media_player_callback_.reset();
    task_runner.reset();
    media_player.reset();
  }

  std::unique_ptr<OHOSMediaPlayerCallback> media_player_callback_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner = nullptr;
  base::WeakPtr<OHOSMediaPlayerBridge> media_player = nullptr;
  base::test::TaskEnvironment task_environment_;
};

TEST_F(OHOSMediaPlayerCallbackTest, OnError) {
    media_player_callback_->task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    OHOS::NWeb::PlayerAdapterErrorType errorType = OHOS::NWeb::PlayerAdapterErrorType::INVALID_CODE;
    media_player_callback_->OnError(errorType);
}

TEST_F(OHOSMediaPlayerCallbackTest, TestOnInfompl) {
  OHOS::NWeb::PlayerOnInfoType type = OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_UNSET;
  int32_t extra = 1;
  int32_t value = 0;
  media_player_callback_->OnInfo(type,extra,value);
}