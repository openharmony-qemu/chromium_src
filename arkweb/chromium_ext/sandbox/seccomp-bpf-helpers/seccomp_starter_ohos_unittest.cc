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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include "base/logging.h"
#include "sandbox/linux/bpf_dsl/bpf_dsl.h"
#include "sandbox/linux/seccomp-bpf/bpf_tests.h"
#define private public
#include "seccomp_starter_ohos.h"
#include "sandbox/linux/seccomp-bpf/sandbox_bpf.h"
#undef private

using namespace sandbox;
using ::testing::Return;
using sandbox::bpf_dsl::Allow;
using sandbox::bpf_dsl::ResultExpr;
using sandbox::bpf_dsl::Trap;

class MockPolicy : public  sandbox::bpf_dsl::Policy {
 public:
  explicit MockPolicy(int* counter_ptr)
      : counter_ptr_(counter_ptr) {}

  MockPolicy(const MockPolicy&) = delete;
  MockPolicy& operator=(const MockPolicy&) = delete;

  ~MockPolicy() override = default;

  ResultExpr EvaluateSyscall(int sysno) const override {
    DCHECK(SandboxBPF::IsValidSyscallNumber(sysno));
    return Allow();
  }

 private:
  const raw_ptr<int> counter_ptr_;
};

class MockSandboxBPF : public SandboxBPF {
public:
  explicit MockSandboxBPF(int* counter_ptr) : SandboxBPF(std::make_unique<MockPolicy>(counter_ptr)) {}
  MOCK_METHOD(bool, StartSandbox, (SeccompLevel level), ());
};

static bool (*g_supports_func)(SandboxBPF::SeccompLevel) = nullptr;

class SeccompStarterOhosTest : public ::testing::Test {
protected:
    void SetUp() override {
        int counter = 0;
        mock_bpf_ = std::make_unique<MockSandboxBPF>(&counter);
    }
    void TearDown() override {
        mock_bpf_ = nullptr;
        g_supports_func = nullptr;
    }
    std::unique_ptr<MockSandboxBPF> mock_bpf_;
};

TEST_F(SeccompStarterOhosTest, StartSandbox_Unsupported) {
    int counter = 0;
    g_supports_func = [](SandboxBPF::SeccompLevel) { return false; };
    mock_bpf_ = std::make_unique<MockSandboxBPF>(&counter);
    SeccompStarterOhos starter;
    starter.set_policy(std::move(mock_bpf_->policy_));
    bool result = starter.StartSandbox();

    EXPECT_TRUE(result);
    EXPECT_EQ(starter.status_, SeccompSandboxStatus::ENGAGED);
}

TEST_F(SeccompStarterOhosTest, StartSandbox_SupportedAndStartOk) {
    int counter = 0;
    g_supports_func = [](SandboxBPF::SeccompLevel) { return true; };
    mock_bpf_ = std::make_unique<MockSandboxBPF>(&counter);
    SeccompStarterOhos starter;
    starter.set_policy(std::move(mock_bpf_->policy_));
    bool result = starter.StartSandbox();

    EXPECT_TRUE(result);
    EXPECT_EQ(starter.status_, SeccompSandboxStatus::ENGAGED);
}

TEST_F(SeccompStarterOhosTest, StartSandbox_SupportedButStartFails) {
  int counter = 0;
  g_supports_func = [](SandboxBPF::SeccompLevel) { return true; };
  mock_bpf_ = std::make_unique<MockSandboxBPF>(&counter);
  SeccompStarterOhos starter;
  starter.set_policy(std::move(mock_bpf_->policy_));
  bool result = starter.StartSandbox();
  EXPECT_EQ(starter.status_, SeccompSandboxStatus::ENGAGED);
}