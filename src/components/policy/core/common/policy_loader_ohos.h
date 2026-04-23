// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
