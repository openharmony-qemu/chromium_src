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
#include "arkweb/chromium_ext/content/browser/ohos/content_view_statics_ohos.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace content {
class ContentViewStaticsOHOSTest : public ::testing::Test {};

TEST_F(ContentViewStaticsOHOSTest,
       PauseWebKitShardTimersFromOHOS_EmptyProcessList) {
  auto watcher = SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance();
  EXPECT_NO_FATAL_FAILURE(watcher->PauseWebKitShardTimersFromOHOS());
  SUCCEED();
}

TEST_F(ContentViewStaticsOHOSTest, PauseWebKitShardTimersFromOHOS_NullHost) {
  auto watcher = SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance();
  EXPECT_NO_FATAL_FAILURE(watcher->PauseWebKitShardTimersFromOHOS());
  SUCCEED();
}

TEST_F(ContentViewStaticsOHOSTest,
       ResumeWebKitShardTimersFromOHOS_EmptyPauseList) {
  auto watcher = SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance();
  EXPECT_NO_FATAL_FAILURE(watcher->ResumeWebKitShardTimersFromOHOS());
  SUCCEED();
}

TEST_F(ContentViewStaticsOHOSTest, ResumeWebKitShardTimersFromOHOS_NullHost) {
  auto watcher = SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance();
  EXPECT_NO_FATAL_FAILURE(watcher->PauseWebKitShardTimersFromOHOS());
  SUCCEED();
}

TEST_F(ContentViewStaticsOHOSTest, PauseAndResumeCompleteFlow) {
  auto watcher = SuspendedProcessWatcherOHOS::GetShradWebKitTimersInstance();
  EXPECT_NO_FATAL_FAILURE(watcher->PauseWebKitShardTimersFromOHOS());
  EXPECT_NO_FATAL_FAILURE(watcher->ResumeWebKitShardTimersFromOHOS());
  EXPECT_NO_FATAL_FAILURE(watcher->PauseWebKitShardTimersFromOHOS());
  EXPECT_NO_FATAL_FAILURE(watcher->ResumeWebKitShardTimersFromOHOS());
  SUCCEED();
}
}  // namespace content