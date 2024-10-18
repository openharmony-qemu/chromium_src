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

#include "ohos_nweb/src/set_renderer_seccomp_policy.cc"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include "sandbox/linux/seccomp-bpf-helpers/baseline_policy_ohos.h"
#include "sandbox/linux/seccomp-bpf-helpers/seccomp_starter_ohos.h"

// Mock classes for testing
namespace sandbox {
class MockSeccompStarterOhos : public SeccompStarterOhos {
 public:
  MOCK_METHOD1(set_policy, void(std::unique_ptr<BaselinePolicyOhos>));
  MOCK_METHOD0(StartSandbox, void());
  MOCK_METHOD0(status, SeccompSandboxStatus());
};
}  // namespace sandbox

// Test cases
TEST(SetRendererSeccompPolicyTest, SetRendererSeccompPolicy_001) {
#if defined(__arm__) || defined(__aarch64__)
  sandbox::MockSeccompStarterOhos starter;
  ON_CALL(starter, status())
      .WillByDefault(::testing::Return(sandbox::SeccompSandboxStatus::ENGAGED));

  bool result = SetRendererSeccompPolicy();
  EXPECT_TRUE(result);
#else
  bool result = SetRendererSeccompPolicy();
  EXPECT_TRUE(result);
#endif
}

TEST(SetRendererSeccompPolicyTest, SetRendererSeccompPolicy_002) {
#if !(defined(__arm__) || defined(__aarch64__))
  bool result = SetRendererSeccompPolicy();
  EXPECT_TRUE(result);
#endif
}