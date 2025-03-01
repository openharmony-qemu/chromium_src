// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/policy/core/common/policy_loader_ohos.h"

#include <string>
#include <vector>

#include "base/base_paths_ohos.h"
#include "base/files/file.h"
#include "base/json/json_string_value_serializer.h"
#include "base/logging.h"
#include "base/ohos/sys_info_utils.h"
#include "base/path_service.h"
#include "components/policy/core/common/policy_bundle.h"
#include "components/policy/core/common/policy_load_status.h"
#include "components/policy/core/common/policy_types.h"

#include "ohos_adapter_helper.h"

namespace policy {

namespace {
constexpr int kApiMinApiVersion = 16;
constexpr bool kUseTestPolicies = false;
}  // namespace

PolicyChangedEventCallback::PolicyChangedEventCallback(PolicyLoaderOhos* loader)
    : loader_(loader) {}

void PolicyChangedEventCallback::OnPolicyChanged() {
  OnPolicyChangedImpl();
}

void PolicyChangedEventCallback::Changed() {
  OnPolicyChangedImpl();
}

void PolicyChangedEventCallback::OnPolicyChangedImpl() {
  LOG(INFO) << "Recv edm policy change event and reload policy.";
  if (loader_) {
    loader_->Reload(true);
  }
}

PolicyLoaderOhos::PolicyLoaderOhos(
    scoped_refptr<base::SequencedTaskRunner> task_runner)
    : AsyncPolicyLoader(task_runner, /*periodic_updates*/ false) {}

PolicyLoaderOhos::~PolicyLoaderOhos() {
  if (use_browser_policy_) {
    policy::BrowserPolicyHandler::GetInstance()->RemoveObserver(
        event_callback_.get());
  } else {
    std::ignore = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                      .GetEnterpriseDeviceManagementInstance()
                      .StopObservePolicyChange();
  }
}

void PolicyLoaderOhos::TryChoosePolicySource() {
  if (policy_source_choosed_) {
    return;
  }

  int api_version = base::ohos::ApplicationApiVersion();
  LOG(INFO) << "PolicyLoaderOhos Init api version: " << api_version;
  if (api_version < 0) {
    LOG(ERROR) << "PolicyLoaderOhos choose source failed: invalid api_version";
  }

  policy_source_choosed_ = true;
  if (api_version >= kApiMinApiVersion) {
    use_browser_policy_ = true;
  } else {
    use_browser_policy_ = false;
  }
}

void PolicyLoaderOhos::InitOnBackgroundThread() {
  event_callback_ = std::make_shared<PolicyChangedEventCallback>(this);

  TryChoosePolicySource();

  if (use_browser_policy_) {
    BrowserPolicyHandler::GetInstance()->AddObserver(event_callback_.get());
  } else {
    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetEnterpriseDeviceManagementInstance()
        .RegistPolicyChangeEventCallback(event_callback_);

    std::ignore = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                      .GetEnterpriseDeviceManagementInstance()
                      .StartObservePolicyChange();
  }
}

PolicyBundle PolicyLoaderOhos::Load() {
  TryChoosePolicySource();

  if (use_browser_policy_) {
    return policy::BrowserPolicyHandler::GetInstance()->GetPolicyBundle();
  } else {
    std::string policies;
    int32_t error_code = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                             .GetEnterpriseDeviceManagementInstance()
                             .GetPolicies(policies);
    LOG(INFO) << "GetPolicies error_code:" << error_code
              << ", policies:" << policies;

    if (kUseTestPolicies) {
      policies = ReadTestPolices();
      LOG(INFO) << "ReadTestPolices policies:" << policies;
    }

    PolicyBundle bundle;
    std::ignore = ParsePolicy(policies, &bundle);
    return bundle;
  }
}

std::string PolicyLoaderOhos::ReadTestPolices() {
  base::FilePath data_path;
  base::PathService::Get(base::DIR_CACHE, &data_path);
  data_path = data_path.Append("test_polices.json");
  LOG(INFO) << "Try to read test_polices.json from " << data_path.value();

  base::File tfile(data_path, base::File::FLAG_OPEN | base::File::FLAG_READ);
  if (!tfile.IsValid()) {
    LOG(INFO) << "test_polices.json is invalid or not exist.";
    return "";
  }

  std::vector<char> buffer(tfile.GetLength());
  int bytes_read = tfile.Read(0, buffer.data(), buffer.size());
  if (bytes_read == -1) {
    LOG(INFO) << "Read test_polices.json failed.";
    return "";
  }

  auto buffer_str = std::string_view(buffer.data(), buffer.size());
  auto json =
      base::JSONReader::Read(buffer_str, base::JSON_ALLOW_TRAILING_COMMAS);
  if (!json.has_value()) {
    LOG(INFO) << "Read test_polices.json failed as invalid json format.";
    return "";
  }

  return std::string(buffer_str);
}

// static
bool PolicyLoaderOhos::ParsePolicy(const std::string& json,
                                   PolicyBundle* bundle) {
  /* policy json demo
  "InsecurePrivateNetworkRequestsAllowed": {
    "level": "mandatory",
    "scope": "machine",
    "source": "platform",
    "value": true
  }*/
  if (bundle == nullptr) {
    LOG(WARNING) << "Null bundle given, parse failed";
    return false;
  }

  base::Value::Dict dictionary_value;
  JSONStringValueDeserializer deserializer(json);
  std::string error_msg;
  std::unique_ptr<base::Value> json_value =
      deserializer.Deserialize(/*error_code=*/nullptr, &error_msg);
  if (!json_value) {
    LOG(WARNING) << "Unable to deserialize json data. error_msg: " << error_msg;
    return false;
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
  return true;
}

}  // namespace policy
