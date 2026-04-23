/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef COMPONENTS_POLICY_CORE_COMMON_POLICY_LOADER_OHOS_H_
#define COMPONENTS_POLICY_CORE_COMMON_POLICY_LOADER_OHOS_H_

#include "base/task/sequenced_task_runner.h"
#include "components/policy/core/common/async_policy_loader.h"
#include "components/policy/policy_export.h"

namespace base {
class Value;
}

namespace policy {

class POLICY_EXPORT PolicyLoaderOhos : public AsyncPolicyLoader {
 public:
  PolicyLoaderOhos(scoped_refptr<base::SequencedTaskRunner> task_runner);
  PolicyLoaderOhos(const PolicyLoaderOhos&) = delete;
  PolicyLoaderOhos& operator=(const PolicyLoaderOhos&) = delete;
  ~PolicyLoaderOhos() override;

  // AsyncPolicyLoader implementation.
  void InitOnBackgroundThread() override;
  PolicyBundle Load() override;

 private:
  void LoadOhosPolicy(const std::string& json, PolicyBundle* bundle);

  void ApplyFallbackPolicies(policy::PolicyBundle* bundle);

  base::Value::Dict GetDictValue(const std::string& json);
};

}  // namespace policy

#endif  // COMPONENTS_POLICY_CORE_COMMON_POLICY_LOADER_OHOS_H_
