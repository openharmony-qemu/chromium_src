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

PolicyLoaderOhos::PolicyLoaderOhos(
    scoped_refptr<base::SequencedTaskRunner> task_runner)
    : AsyncPolicyLoader(task_runner, /*periodic_updates*/ false) {}

PolicyLoaderOhos::~PolicyLoaderOhos() {}

void PolicyLoaderOhos::InitOnBackgroundThread() {
  // 目前未对鸿蒙policy进行后台扫描监控，暂时为空实现
}

std::unique_ptr<PolicyBundle> PolicyLoaderOhos::Load() {
  std::string policies;
  int32_t error_code = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                           .GetEnterpriseDeviceManagementInstance()
                           .GetPolicies(policies);
  LOG(INFO) << "GetPolicies error_code:" << error_code
            << ", policies:" << policies;
  std::unique_ptr<PolicyBundle> bundle = std::make_unique<PolicyBundle>();
  LoadOhosPolicy(policies, bundle.get());
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

  base::DictionaryValue dictionary_value;
  JSONStringValueDeserializer deserializer(json);
  std::string error_msg;
  std::unique_ptr<base::Value> json_value =
      deserializer.Deserialize(/*error_code=*/nullptr, &error_msg);
  if (!json_value) {
    LOG(WARNING) << "Unable to deserialize json data. error_msg: " << error_msg;
    return;
  }

  if (json_value->type() == base::Value::Type::DICTIONARY) {
    for (auto kv : json_value->DictItems()) {
      std::string key = kv.first;
      if (kv.second.type() != base::Value::Type::DICTIONARY) {
        LOG(WARNING) << "key: " << key << " type is not  DICTIONARY";
        continue;
      }
      base::Value* policy_value = kv.second.FindKey("value");
      if (!policy_value) {
        LOG(WARNING) << "key: " << key << " has no value";
        continue;
      }

      switch (policy_value->type()) {
        case base::Value::Type::BOOLEAN: {
          dictionary_value.SetBoolean(key, policy_value->GetBool());
          break;
        }
        case base::Value::Type::INTEGER: {
          dictionary_value.SetInteger(key, policy_value->GetInt());
          break;
        }
        case base::Value::Type::DOUBLE: {
          dictionary_value.SetDouble(key, policy_value->GetDouble());
          break;
        }
        case base::Value::Type::STRING: {
          dictionary_value.SetString(key, policy_value->GetString());
          break;
        }
        case base::Value::Type::LIST: {
          dictionary_value.SetList(
              key, std::make_unique<base::ListValue>(policy_value->GetList()));
          break;
        }
        default:
          LOG(WARNING) << "unkown type";
      }
    }
  }

  PolicyMap policy_map;
  policy_map.LoadFrom(&dictionary_value, POLICY_LEVEL_MANDATORY,
                      POLICY_SCOPE_MACHINE, POLICY_SOURCE_PLATFORM);
  bundle->Get(PolicyNamespace(POLICY_DOMAIN_CHROME, std::string()))
      .MergeFrom(policy_map);
  return;
}

}  // namespace policy
