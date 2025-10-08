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

#if BUILDFLAG(ARKWEB_CUSTOM_DNS)
TEST_F(NetworkContextTest, TesExtSetHostIP001) {
  std::unique_ptr<NetworkContext> network_context =
      CreateContextWithParams(CreateNetworkContextParamsForTesting());
  std::string hostName = "";
  std::vector<std::string> address = {};
  uint32_t alive_time = 0;
  ASSERT_NO_FATAL_FAILURE(
      network_context->AsArkWebNetworkContextExt()->SetHostIP(hostName, address,
                                                              alive_time));
}

TEST_F(NetworkContextTest, TesExtSetHostIP002) {
  std::unique_ptr<NetworkContext> network_context =
      CreateContextWithParams(CreateNetworkContextParamsForTesting());
  std::string hostName = "";
  std::vector<std::string> address = {};
  address.push_back("1");
  address.push_back("2");
  uint32_t alive_time = 0;
  ASSERT_NO_FATAL_FAILURE(
      network_context->AsArkWebNetworkContextExt()->SetHostIP(hostName, address,
                                                              alive_time));
}

TEST_F(NetworkContextTest, TesExtClearHostIP001) {
  std::unique_ptr<NetworkContext> network_context =
      CreateContextWithParams(CreateNetworkContextParamsForTesting());
  ASSERT_NO_FATAL_FAILURE(
      network_context->AsArkWebNetworkContextExt()->ClearHostIP(""));
}

TEST_F(NetworkContextTest, TesExtClearHostIP002) {
  std::unique_ptr<NetworkContext> network_context =
      CreateContextWithParams(CreateNetworkContextParamsForTesting());
  std::string hostName = "111";
  std::vector<std::string> address = {};
  uint32_t alive_time = 0;
  ASSERT_NO_FATAL_FAILURE(
      network_context->AsArkWebNetworkContextExt()->ClearHostIP(hostName));
}
#endif  // ARKWEB_CUSTOM_DNS

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
TEST_F(NetworkContextTest, TesExtIsStrictLogModeIP001) {
  std::unique_ptr<NetworkContext> network_context =
      CreateContextWithParams(CreateNetworkContextParamsForTesting());
  network_context->AsArkWebNetworkContextExt()->params_->is_strict_log_mode =
      false;
  bool result = network_context->AsArkWebNetworkContextExt()->IsStrictLogMode();
  ASSERT_FALSE(result);
}

TEST_F(NetworkContextTest, TesExtIsStrictLogModeIP002) {
  std::unique_ptr<NetworkContext> network_context =
      CreateContextWithParams(CreateNetworkContextParamsForTesting());
  network_context->AsArkWebNetworkContextExt()->params_->is_strict_log_mode =
      true;
  network_context->AsArkWebNetworkContextExt()->params_ = nullptr;
  bool result = network_context->AsArkWebNetworkContextExt()->IsStrictLogMode();
  ASSERT_FALSE(result);
}
#endif  // ARKWEB_LOGGER_REPORT