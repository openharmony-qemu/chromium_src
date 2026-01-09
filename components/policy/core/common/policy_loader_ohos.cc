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

<<<<<<< HEAD:components/policy/core/common/policy_loader_ohos.cc
}  // namespace policy
=======
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
>>>>>>> 【功能-BUGFIX】解决无法获取value为dict的策略问题:src/components/policy/core/common/policy_loader_ohos.cc
