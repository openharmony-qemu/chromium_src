/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include "seccomp_starter_ohos.h"

#include "base/logging.h"
#include "sandbox/linux/seccomp-bpf/sandbox_bpf.h"

namespace sandbox {

SeccompStarterOhos::SeccompStarterOhos() = default;

SeccompStarterOhos::~SeccompStarterOhos() = default;

bool SeccompStarterOhos::StartSandbox() {
  // Do run-time detection to ensure that support is present.
  if (!SandboxBPF::SupportsSeccompSandbox(
          SandboxBPF::SeccompLevel::MULTI_THREADED)) {
    status_ = SeccompSandboxStatus::DETECTION_FAILED;
    LOG(WARNING) << "Seccomp support should be present, but detection "
                 << "failed. Continuing without Seccomp-BPF.";
    return false;
  }
  SandboxBPF sandbox(std::move(policy_));
  CHECK(sandbox.StartSandbox(SandboxBPF::SeccompLevel::MULTI_THREADED));
  status_ = SeccompSandboxStatus::ENGAGED;
  return true;
}

}  // namespace sandbox
