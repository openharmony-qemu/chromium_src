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

#include "arkweb/chromium_ext/content/browser/renderer_host/ark_web_render_frame_host_manager.h"

#include "content/public/browser/back_forward_cache.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory> 

using ::testing::Return;
using ::testing::_;

namespace content {
class MockDisabledReason : public BackForwardCache {
 public:
  MockDisabledReason() = default;
  virtual ~MockDisabledReason() = default;

  MOCK_METHOD(int, ArkWebGetCacheSize, (), (const, override));
  MOCK_METHOD(int, ArkWebGetTimeToLive, (), (const, override));

  void Flush();
  void Flush(NotRestoredReason reason);

  void Prune(size_t limit);
  void DisableForTesting(DisableForTestingReason reason);

  size_t GetStoredEntriesNumber();
  void SetCacheSize(int size);
  void SetTimeToLive(int timeToLive);
  base::TimeDelta ArkWebGetTimeToLiveInBackForwardCache();
};

void MockDisabledReason::Flush() {
}
void MockDisabledReason::Flush(NotRestoredReason reason) {}

void MockDisabledReason::Prune(size_t limit) {}
void MockDisabledReason::DisableForTesting(DisableForTestingReason reason) {}

size_t MockDisabledReason::GetStoredEntriesNumber() {
  return 0;
}
void MockDisabledReason::SetCacheSize(int size) {}
void MockDisabledReason::SetTimeToLive(int timeToLive) {}
base::TimeDelta MockDisabledReason::ArkWebGetTimeToLiveInBackForwardCache() {
  base::TimeDelta time_delta;
  return time_delta;
}

class ArkWebRenderFrameHostManagerTest : public ::testing::Test {
 protected:
  void SetUp() override { mock_ = std::make_unique<MockDisabledReason>(); }
  void TearDown() override {}

  std::unique_ptr<MockDisabledReason> mock_;
};

TEST_F(ArkWebRenderFrameHostManagerTest, ArkWebUnloadOldFrame001) {
  bool can_store = false;
  std::string reason = "";
  const int expected_caches_size = 1;
  const int expected_time_value = 1;

  EXPECT_CALL(*mock_, ArkWebGetCacheSize())
      .Times(1)
      .WillOnce(Return(expected_caches_size));
  EXPECT_CALL(*mock_, ArkWebGetTimeToLive())
      .Times(1)
      .WillOnce(Return(expected_time_value));
  ArkWebUnloadOldFrame(*mock_, reason, can_store);
}

TEST_F(ArkWebRenderFrameHostManagerTest, ArkWebUnloadOldFrame002) {
  bool can_store = false;
  std::string reason = "";
  const int expected_caches_size = 1;
  const int expected_time_value = -1;

  EXPECT_CALL(*mock_, ArkWebGetCacheSize())
      .Times(1)
      .WillOnce(Return(expected_caches_size));

  EXPECT_CALL(*mock_, ArkWebGetTimeToLive())
      .Times(1)
      .WillOnce(Return(expected_time_value));
  ArkWebUnloadOldFrame(*mock_, reason, can_store);
}

TEST_F(ArkWebRenderFrameHostManagerTest, ArkWebUnloadOldFrame003) {
  bool can_store = false;
  std::string reason = "";
  const int expected_caches_size = -1;
  const int expected_time_value = 1;

  EXPECT_CALL(*mock_, ArkWebGetCacheSize())
      .Times(1)
      .WillOnce(Return(expected_caches_size));

  EXPECT_CALL(*mock_, ArkWebGetTimeToLive())
      .Times(1)
      .WillOnce(Return(expected_time_value));
  ArkWebUnloadOldFrame(*mock_, reason, can_store);
}
}  // namespace content