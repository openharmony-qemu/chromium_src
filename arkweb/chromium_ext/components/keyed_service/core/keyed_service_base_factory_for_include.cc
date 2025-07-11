/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "base/logging.h"
#include "third_party/angle/third_party/rapidjson/src/include/rapidjson/document.h"
#include "third_party/angle/third_party/rapidjson/src/include/rapidjson/error/en.h"
#include "base/files/file_util.h"

const std::unordered_set<std::string>& KeyedServiceBaseFactory::GetDisableServices() {
  if (dependency_manager_->disabledServicesInitialized) {
    return dependency_manager_->disabledServices;
  }

  std::unordered_set<std::string> services;
  base::FilePath file_path = base::FilePath::FromUTF8Unsafe("/data/storage/el1/bundle/arkwebcore/etc/disabled_services.json");

  // 读取文件内容
  std::string json_content;
  if (!base::ReadFileToString(file_path, &json_content)) {
    LOG(WARNING) << "WARNING: read file empty: " << file_path.value();
    dependency_manager_->disabledServicesInitialized = true;
    return dependency_manager_->disabledServices;
  }

  // 解析 JSON 内容
  rapidjson::Document doc;
  if (doc.Parse(json_content.c_str()).HasParseError()) {
    LOG(ERROR) << "ERROR: Failed to parse JSON content: " << rapidjson::GetParseError_En(doc.GetParseError());
    dependency_manager_->disabledServicesInitialized = true;
    return dependency_manager_->disabledServices;
  }

  // 检查解析结果是否为数组类型
  if (doc.IsArray()) {
    for (const auto& s : doc.GetArray()) {
      services.insert(s.GetString());
    }
  } else {
    LOG(WARNING) << "WARNING: JSON content is not an array: " << file_path.value();
  }

  dependency_manager_->disabledServicesInitialized = true;
  dependency_manager_->disabledServices = services;
  return dependency_manager_->disabledServices;
}

bool KeyedServiceBaseFactory::KeyedServiceDisabled(const char* service_name) {
  const std::unordered_set<std::string>& desired_strings = GetDisableServices();
  std::string service_name_input = std::string(service_name);
  if (desired_strings.count(service_name_input) > 0) {
    return true;
  }
  return false;
}
