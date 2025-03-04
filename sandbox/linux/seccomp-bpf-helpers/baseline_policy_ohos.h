// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SANDBOX_LINUX_SECCOMP_BPF_HELPERS_BASELINE_POLICY_OHOS_H_
#define SANDBOX_LINUX_SECCOMP_BPF_HELPERS_BASELINE_POLICY_OHOS_H_

#include <sys/types.h>

#include "sandbox/linux/seccomp-bpf-helpers/baseline_policy.h"
#include "sandbox/sandbox_export.h"

namespace sandbox {

class SANDBOX_EXPORT BaselinePolicyOhos : public BaselinePolicy {
 public:
  BaselinePolicyOhos();
  explicit BaselinePolicyOhos(bool allow_sched_affinity);

  BaselinePolicyOhos(const BaselinePolicyOhos&) = delete;
  BaselinePolicyOhos& operator=(const BaselinePolicyOhos&) = delete;

  ~BaselinePolicyOhos() override;

  // sandbox::BaselinePolicy:
  sandbox::bpf_dsl::ResultExpr EvaluateSyscall(
      int system_call_number) const override;
};

}  // namespace sandbox
#endif  // SANDBOX_LINUX_SECCOMP_BPF_HELPERS_BASELINE_POLICY_OHOS_H_
