// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_POLICY_CORE_COMMON_POLICY_LOADER_OHOS_H_
#define COMPONENTS_POLICY_CORE_COMMON_POLICY_LOADER_OHOS_H_

#include "base/task/sequenced_task_runner.h"
#include "components/policy/core/common/async_policy_loader.h"
#include "components/policy/policy_export.h"
#include "enterprise_device_management_adapter.h"

namespace base {
class Value;
}

namespace policy {

class PolicyChangedEventCallback;

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
  std::string ReadTestPolices();
  
  std::shared_ptr<PolicyChangedEventCallback> event_callback_;
};

class PolicyChangedEventCallback : public OHOS::NWeb::EdmPolicyChangedEventCallbackAdapter {
 public:
  PolicyChangedEventCallback(PolicyLoaderOhos*);

  void Changed() override;

 private:
  PolicyLoaderOhos* loader_;
};

}  // namespace policy

#endif  // COMPONENTS_POLICY_CORE_COMMON_POLICY_LOADER_OHOS_H_
