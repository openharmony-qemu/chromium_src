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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <map>
#include <string>
#include <thread>

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "nweb_engine_impl.h"
#include "nweb.h"
#include "cef/ohos_cef_ext/libcef/browser/devtools/devtools_manager_delegate.h"

using namespace testing;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
class NWebEngineImplTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp(void);
  void TearDown(void);
  std::shared_ptr<NWebEngineImpl> nweb_engine_impl_;
};

void NWebEngineImplTest::SetUpTestCase(void) {}

void NWebEngineImplTest::TearDownTestCase(void) {}

void NWebEngineImplTest::SetUp() {
  nweb_engine_impl_ = std::make_shared<NWebEngineImpl>();
  ASSERT_NE(nweb_engine_impl_, nullptr);
}

void NWebEngineImplTest::TearDown() {
  nweb_engine_impl_ = nullptr;
}

TEST_F(NWebEngineImplTest, SetWebDebuggingAccess001) {
  bool isEnableDebug = false;
  OHOS::NWeb::SetWebDebuggingEnabled(false);
  OHOS::NWeb::SetWebDebuggingPort(0);
  nweb_engine_impl_->SetWebDebuggingAccess(isEnableDebug);
  EXPECT_NE(nweb_engine_impl_, nullptr);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingEnabled(), false);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingPort(), 0);
}

TEST_F(NWebEngineImplTest, SetWebDebuggingAccess002) {
  bool isEnableDebug = false;
  OHOS::NWeb::SetWebDebuggingEnabled(false);
  OHOS::NWeb::SetWebDebuggingPort(1);
  nweb_engine_impl_->SetWebDebuggingAccess(isEnableDebug);
  EXPECT_NE(nweb_engine_impl_, nullptr);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingEnabled(), false);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingPort(), 0);
}

TEST_F(NWebEngineImplTest, SetWebDebuggingAccess003) {
  bool isEnableDebug = false;
  OHOS::NWeb::SetWebDebuggingEnabled(true);
  OHOS::NWeb::SetWebDebuggingPort(0);
  nweb_engine_impl_->SetWebDebuggingAccess(isEnableDebug);
  EXPECT_NE(nweb_engine_impl_, nullptr);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingEnabled(), false);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingPort(), 0);
}

TEST_F(NWebEngineImplTest, SetWebDebuggingAccessAndPort001) {
  bool isEnableDebug = false;
  int32_t port = 0;
  OHOS::NWeb::SetWebDebuggingEnabled(false);
  OHOS::NWeb::SetWebDebuggingPort(0);
  nweb_engine_impl_->SetWebDebuggingAccessAndPort(isEnableDebug, port);
  EXPECT_NE(nweb_engine_impl_, nullptr);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingEnabled(), false);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingPort(), 0);
}

TEST_F(NWebEngineImplTest, SetWebDebuggingAccessAndPort002) {
  bool isEnableDebug = false;
  int32_t port = 1;
  OHOS::NWeb::SetWebDebuggingEnabled(false);
  OHOS::NWeb::SetWebDebuggingPort(0);
  nweb_engine_impl_->SetWebDebuggingAccessAndPort(isEnableDebug, port);
  EXPECT_NE(nweb_engine_impl_, nullptr);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingEnabled(), false);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingPort(), 1);
}

TEST_F(NWebEngineImplTest, SetWebDebuggingAccessAndPort003) {
  bool isEnableDebug = true;
  int32_t port = 0;
  OHOS::NWeb::SetWebDebuggingEnabled(false);
  OHOS::NWeb::SetWebDebuggingPort(0);
  nweb_engine_impl_->SetWebDebuggingAccessAndPort(isEnableDebug, port);
  EXPECT_NE(nweb_engine_impl_, nullptr);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingEnabled(), true);
  EXPECT_EQ(OHOS::NWeb::GetWebDebuggingPort(), 0);
}
}