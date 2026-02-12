/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "nweb_fallback_proxy.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arkweb/build/features/features.h"

using namespace testing;
using namespace OHOS::NWeb;

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
TEST(NwebFallbackProxyTest, OnUpdateProxyToken) {
  std::string param_token = "param_token";

  testing::internal::CaptureStderr();
  NwebFallbackProxy::OnUpdateProxyToken(param_token);
  std::string output_callback_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(output_callback_err.find("GetProxyClientCallback is null"),
            std::string::npos);

  auto callback_invalid = std::make_shared<NWebProxyClientCallback>();
  NwebFallbackProxy::PutProxyClientCallback(callback_invalid);
  testing::internal::CaptureStderr();
  NwebFallbackProxy::OnUpdateProxyToken(param_token);
  std::string output_func_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(output_func_err.find("GetProxyClientCallback is null"),
            std::string::npos);

  static std::string captured_token;
  auto callback_valid = std::make_shared<NWebProxyClientCallback>();
  callback_valid->onUpdateProxyToken = [](const char* old_token) {
    captured_token = old_token ? old_token : "";
  };
  NwebFallbackProxy::PutProxyClientCallback(callback_valid);
  NwebFallbackProxy::OnUpdateProxyToken(param_token);

  EXPECT_EQ(captured_token, param_token);
}

TEST(NwebFallbackProxyTest, UpdateProxyToken) {
  std::string token = "token";
  std::string token_info = "token_info";

  testing::internal::CaptureStderr();
  NwebFallbackProxy::UpdateProxyToken(nullptr, token_info.c_str());
  std::string output_token_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(output_token_err.find("token or token_info is null"),
            std::string::npos);

  testing::internal::CaptureStderr();
  NwebFallbackProxy::UpdateProxyToken(token.c_str(), nullptr);
  std::string output_info_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(output_info_err.find("token or token_info is null"),
            std::string::npos);
}

TEST(NwebFallbackProxyTest, SetGlobalListConfigPath) {
  std::string path = "file_path";
  std::string version = "version";
  NwebFallbackProxy::SetGlobalListConfigPath(path.c_str(), version.c_str());

  testing::internal::CaptureStderr();
  NwebFallbackProxy::SetGlobalListConfigPath(nullptr, version.c_str());
  std::string output_path_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(output_path_err.find("file_path or version is null"),
            std::string::npos);

  testing::internal::CaptureStderr();
  NwebFallbackProxy::SetGlobalListConfigPath(path.c_str(), nullptr);
  std::string output_version_err = testing::internal::GetCapturedStderr();
  EXPECT_NE(output_version_err.find("file_path or version is null"),
            std::string::npos);
}
#endif
