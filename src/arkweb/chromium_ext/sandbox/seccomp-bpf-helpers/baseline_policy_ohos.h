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
