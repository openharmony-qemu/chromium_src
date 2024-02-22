// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/policy/core/common/policy_loader_ohos.h"

#include <string>

#include "base/json/json_string_value_serializer.h"
#include "base/logging.h"
#include "components/policy/core/common/policy_bundle.h"
#include "components/policy/core/common/policy_load_status.h"
#include "components/policy/core/common/policy_types.h"

#include "ohos_adapter_helper.h"

namespace policy {

PolicyChangedEventCallback::PolicyChangedEventCallback(
    PolicyLoaderOhos* loader) : loader_(loader) {}

void PolicyChangedEventCallback::Changed() {
  LOG(INFO) << "Recv edm policy change event and reload policy.";
  if (loader_) {
    loader_->Reload(true);
  }
}

PolicyLoaderOhos::PolicyLoaderOhos(
    scoped_refptr<base::SequencedTaskRunner> task_runner)
    : AsyncPolicyLoader(task_runner, /*periodic_updates*/ false) {}

PolicyLoaderOhos::~PolicyLoaderOhos() {
    std::ignore = OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetEnterpriseDeviceManagementInstance().StopObservePolicyChange();
}

void PolicyLoaderOhos::InitOnBackgroundThread() {
    event_callback_ = std::make_shared<PolicyChangedEventCallback>(this);

    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetEnterpriseDeviceManagementInstance()
        .RegistPolicyChangeEventCallback(event_callback_);

    std::ignore = OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetEnterpriseDeviceManagementInstance().StartObservePolicyChange();
}

PolicyBundle PolicyLoaderOhos::Load() {
  std::string policies;
  int32_t error_code = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                           .GetEnterpriseDeviceManagementInstance()
                           .GetPolicies(policies);
  LOG(INFO) << "GetPolicies error_code:" << error_code
            << ", policies:" << policies;
  PolicyBundle bundle;
  LoadOhosPolicy(policies, &bundle);
  return bundle;
}

void PolicyLoaderOhos::LoadOhosPolicy(const std::string& json,
                                      PolicyBundle* bundle) {
  /* policy json demo
  "InsecurePrivateNetworkRequestsAllowed": {
    "level": "mandatory",
    "scope": "machine",
    "source": "platform",
    "value": true
  }*/
  if (bundle == nullptr) {
    return;
  }

  base::Value::Dict dictionary_value;
  JSONStringValueDeserializer deserializer(json);
  std::string error_msg;
  std::unique_ptr<base::Value> json_value =
      deserializer.Deserialize(/*error_code=*/nullptr, &error_msg);
  if (!json_value) {
    LOG(WARNING) << "Unable to deserialize json data. error_msg: " << error_msg;
    return;
  }

  if (json_value->type() == base::Value::Type::DICT) {
    for (auto kv : json_value->GetDict()) {
      std::string key = kv.first;
      if (kv.second.type() != base::Value::Type::DICT) {
        LOG(WARNING) << "key: " << key << " type is not  DICTIONARY";
        continue;
      }
      base::Value::Dict& kv_dict = kv.second.GetDict();
      base::Value* policy_value = kv_dict.Find("value");
      if (!policy_value) {
        LOG(WARNING) << "key: " << key << " has no value";
        continue;
      }

      switch (policy_value->type()) {
        case base::Value::Type::BOOLEAN: {
          dictionary_value.Set(key, policy_value->GetBool());
          break;
        }
        case base::Value::Type::INTEGER: {
          dictionary_value.Set(key, policy_value->GetInt());
          break;
        }
        case base::Value::Type::DOUBLE: {
          dictionary_value.Set(key, policy_value->GetDouble());
          break;
        }
        case base::Value::Type::STRING: {
          dictionary_value.Set(key, policy_value->GetString());
          break;
        }
        case base::Value::Type::LIST: {
          dictionary_value.Set(key, std::move(policy_value->GetList()));
          break;
        }
        default:
          LOG(WARNING) << "unkown type";
      }
    }
  }

  PolicyMap policy_map;
  policy_map.LoadFrom(dictionary_value, POLICY_LEVEL_MANDATORY,
                      POLICY_SCOPE_MACHINE, POLICY_SOURCE_PLATFORM);
  bundle->Get(PolicyNamespace(POLICY_DOMAIN_CHROME, std::string()))
      .MergeFrom(policy_map);
  return;
}

}  // namespace policy
