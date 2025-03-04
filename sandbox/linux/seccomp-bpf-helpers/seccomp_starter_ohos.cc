// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sandbox/linux/seccomp-bpf-helpers/seccomp_starter_ohos.h"

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
                       
