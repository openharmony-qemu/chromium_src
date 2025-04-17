// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_POLICY_CORE_COMMON_POLICY_LOADER_OHOS_H_
#define COMPONENTS_POLICY_CORE_COMMON_POLICY_LOADER_OHOS_H_

#include "base/task/sequenced_task_runner.h"
#include "cef/libcef/browser/policy/browser_policy_handler.h"
#include "components/policy/core/common/async_policy_loader.h"
#include "components/policy/policy_export.h"
#include "enterprise_device_management_adapter.h"
#include "base/memory/raw_ptr.h"

namespace base {
class Value;
}

namespace policy {

class PolicyChangedEventCallback;

class POLICY_EXPORT PolicyLoaderOhos : public AsyncPolicyLoader {
 public:
  static constexpr int kUseBrowserPolicyMinApiVersion = 16;
  PolicyLoaderOhos(scoped_refptr<base::SequencedTaskRunner> task_runner);
  PolicyLoaderOhos(const PolicyLoaderOhos&) = delete;
  PolicyLoaderOhos& operator=(const PolicyLoaderOhos&) = delete;
  ~PolicyLoaderOhos() override;

  // AsyncPolicyLoader implementation.
  void InitOnBackgroundThread() override;
  PolicyBundle Load() override;

  static bool ParsePolicy(const std::string& json, PolicyBundle* bundle);

 private:
  std::shared_ptr<PolicyChangedEventCallback> event_callback_;
  bool callback_initialized_ = false;
  bool reached_min_api_version_ = false;
  bool use_browser_policy_ = false;
  bool prev_use_browser_policy_ = false;

  std::string ReadTestPolices();
  void InitialReadApiVersionOnce();
  void MaybeSwitchLoadInvoker();
  void DeterminePolicySource();
};

class PolicyChangedEventCallback
    : public policy::BrowserPolicyHandler::Observer,
      public OHOS::NWeb::EdmPolicyChangedEventCallbackAdapter {
 public:
  PolicyChangedEventCallback(PolicyLoaderOhos* loader);

  // policy::BrowserPolicyHandler::Observer overrides
  void OnPolicyChanged() override;

  // OHOS::NWeb::EdmPolicyChangedEventCallbackAdapter overrides
  void Changed() override;

 private:
  void OnPolicyChangedImpl();
  raw_ptr<PolicyLoaderOhos> loader_;
};

}  // namespace policy

#endif  // COMPONENTS_POLICY_CORE_COMMON_POLICY_LOADER_OHOS_H_
