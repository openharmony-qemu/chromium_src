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
#include "base/test/task_environment.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#define private public
#include "ohos_media_player_listener.h"
#undef private

using namespace media;
using namespace testing;
using namespace OHOS::NWeb;
using namespace  base;

class MoxkSurfaceBufferAdapter :public SurfaceBufferAdapter{
public:
  MOCK_METHOD(int32_t, GetFileDescriptor, (), ());
  MOCK_METHOD(int32_t, GetWidth, (), ());
  MOCK_METHOD(int32_t, GetHeight, (), ());
  MOCK_METHOD(int32_t, GetStride, (), ());
  MOCK_METHOD(int32_t, GetFormat, (), ());
  MOCK_METHOD(uint32_t, GetSize, (), ());
  MOCK_METHOD(void* , GetVirAddr, (), ());
};

class OHOSMediaPlayerListenerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    listener_ = std::make_unique<OHOSMediaPlayerListener>(task_runner_, media_player_);
  }

  void TearDown() override {
  }

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_ = nullptr;
  base::WeakPtr<OHOSMediaPlayerBridge> media_player_= nullptr;
  std::unique_ptr<OHOSMediaPlayerListener> listener_;
  base::test::TaskEnvironment task_environment_;
};

TEST_F(OHOSMediaPlayerListenerTest, OnBufferAvailableImpl) {
  std::shared_ptr<MoxkSurfaceBufferAdapter> mock_surface_buffer = std::make_shared<MoxkSurfaceBufferAdapter>();
  listener_->task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  EXPECT_CALL(*mock_surface_buffer,GetWidth())
    .WillRepeatedly(Return(1));
  listener_->OnBufferAvailable(mock_surface_buffer);
}
