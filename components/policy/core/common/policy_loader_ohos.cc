// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/policy/core/common/policy_loader_ohos.h"

#include <string>

#include "base/json/json_string_value_serializer.h"
#include "base/logging.h"
#include "components/policy/core/common/policy_bundle.h"
#include "components/policy/core/common/policy_types.h"
#include "ohos/adapter/policy/browser_policy_adapter.h"

using namespace ohos::adapter;
namespace policy {
constexpr std::string kPolicyLoaderTag = "[Policy Loader]";

PolicyLoaderOhos::PolicyLoaderOhos(
    scoped_refptr<base::SequencedTaskRunner> task_runner)
    : AsyncPolicyLoader(task_runner, /*periodic_updates*/ false) {}

PolicyLoaderOhos::~PolicyLoaderOhos() {}

void PolicyLoaderOhos::InitOnBackgroundThread() {
  // 目前未对鸿蒙policy进行后台扫描监控，暂时为空实现
}

PolicyBundle PolicyLoaderOhos::Load() {
  // Obtain policies from the MDM application.
  std::string policies = BrowserPolicyAdapter::getManagedBrowserPolicy();
  LOG(INFO) << kPolicyLoaderTag << "policies from MDM:" << policies;

  PolicyBundle bundle;
  LoadOhosPolicy(policies, &bundle);
  return bundle;
}

base::Value::Dict PolicyLoaderOhos::GetDictValue(const std::string& json) {
  base::Value::Dict dictionary_value;
  JSONStringValueDeserializer deserializer(json);
  std::string error_msg;
  std::unique_ptr<base::Value> json_value =
      deserializer.Deserialize(/*error_code=*/nullptr, &error_msg);
  if (!json_value) {
    LOG(WARNING) << kPolicyLoaderTag
                 << "Unable to deserialize json data. error_msg: " << error_msg;
    return dictionary_value;
  }

  if (json_value->type() == base::Value::Type::DICT) {
    for (auto kv : json_value->GetDict()) {
      std::string key = kv.first;
      if (kv.second.type() != base::Value::Type::DICT) {
        LOG(WARNING) << kPolicyLoaderTag << "key: " << key
                     << " type is not  DICTIONARY";
        continue;
      }

      base::Value* policy_value = kv.second.GetDict().Find("value");
      if (!policy_value) {
        LOG(WARNING) << kPolicyLoaderTag << "key: " << key << " has no value";
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
          dictionary_value.Set(key, policy_value->GetList().Clone());
          break;
        }
        case base::Value::Type::DICT: {
          dictionary_value.Set(key, policy_value->GetDict().Clone());
          break;
        }
        default:
          LOG(WARNING)
              << kPolicyLoaderTag << "key: " << key
              << ", unable to recognize value type during policy parsing.";
      }
    }
  }
  return dictionary_value;
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
    LOG(ERROR) << kPolicyLoaderTag << "Bundle is nullptr";
    return;
  }

  base::Value::Dict dictionary_value = GetDictValue(json);
  if (dictionary_value.empty()) {
    // If there is no local MDM application, use the default policy to prevent
    // HTTP access failures.
    LOG(INFO) << kPolicyLoaderTag << "Get nothing from MDM, use default policies";
    ApplyFallbackPolicies(bundle);
    return;
  }
  PolicyMap policy_map;
  policy_map.LoadFrom(dictionary_value, POLICY_LEVEL_MANDATORY,
                      POLICY_SCOPE_MACHINE, POLICY_SOURCE_PLATFORM);
  bundle->Get(PolicyNamespace(POLICY_DOMAIN_CHROME, std::string()))
      .MergeFrom(policy_map);
  return;
}

void PolicyLoaderOhos::ApplyFallbackPolicies(PolicyBundle* bundle) {
  PolicyMap fallback_map;

  fallback_map.Set("InsecurePrivateNetworkRequestsAllowed",
                   POLICY_LEVEL_MANDATORY, POLICY_SCOPE_MACHINE,
                   POLICY_SOURCE_PLATFORM, base::Value(true),
                   /*external_data_fetcher=*/nullptr);

  fallback_map.Set("LegacySameSiteCookieBehaviorEnabled",
                   POLICY_LEVEL_MANDATORY, POLICY_SCOPE_MACHINE,
                   POLICY_SOURCE_PLATFORM, base::Value(1),
                   /*external_data_fetcher=*/nullptr);

  base::Value::List domain_list;
  domain_list.Append("[*.]");
  fallback_map.Set("LegacySameSiteCookieBehaviorEnabledForDomainList",
                   POLICY_LEVEL_MANDATORY, POLICY_SCOPE_MACHINE,
                   POLICY_SOURCE_PLATFORM, base::Value(std::move(domain_list)),
                   /*external_data_fetcher=*/nullptr);

  bundle->Get(PolicyNamespace(POLICY_DOMAIN_CHROME, std::string()))
      .MergeFrom(fallback_map);
}

}  // namespace policy