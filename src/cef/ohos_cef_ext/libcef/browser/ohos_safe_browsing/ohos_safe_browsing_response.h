// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_SAFE_BROWSING_SAFE_BROWSING_RESPONSE_H_
#define CEF_LIBCEF_BROWSER_SAFE_BROWSING_SAFE_BROWSING_RESPONSE_H_

#include <string>

#include "base/json/json_value_converter.h"
#include "base/values.h"

namespace {
const int kCloudControlVectorMaxSize = 10000;
}

namespace ohos_safe_browsing {
static bool ParseString(const base::Value* value, std::string* field) {
  if (!value->is_string()) {
    *field = "";
  } else {
    *field = value->GetString();
  }

  return true;
}

static bool ParseInteger(const base::Value* value, int* field) {
  if (!value->is_int()) {
    *field = -1;
  } else {
    *field = value->GetInt();
  }

  return true;
}

static bool ParseStringVector(const base::Value* value,
                              std::vector<std::string>* result) {
  if (result && value && value->is_list()) {
    result->clear();
    for (size_t i = 0; i < value->GetList().size(); i++) {
      const auto& element = value->GetList()[i];
      if (!element.is_string()) {
        LOG(WARNING) << "ParseStringVector, value of the list is not a string.";
        return false;
      }
      result->emplace_back(element.GetString());
      if (result->size() >= kCloudControlVectorMaxSize) {
        LOG(WARNING) << "ParseStringVector, only use the first "
                     << kCloudControlVectorMaxSize
                     << " items, the last element is " << element.GetString();
        break;
      }
    }
  }

  return true;
}

struct VendorInfo {
  std::string vendor_id;
  std::string code;
  int hw_code;

  VendorInfo() {
    vendor_id = "";
    code = "";
    hw_code = -1;
  }
  ~VendorInfo() = default;

  static void RegisterJSONConverter(
      base::JSONValueConverter<VendorInfo>* converter) {
    converter->RegisterCustomValueField<std::string>(
        "vendorID", &VendorInfo::vendor_id, &ParseString);
    converter->RegisterCustomValueField<std::string>("code", &VendorInfo::code,
                                                     &ParseString);
    converter->RegisterCustomValueField<int>("hwCode", &VendorInfo::hw_code,
                                             &ParseInteger);
  }
};

template <typename T>
static bool ParseCustomValueResult(const base::Value* value, T* result) {
  if (!value->is_none()) {
    base::JSONValueConverter<T> converter;
    if (!converter.Convert(*value, result)) {
      return false;
    }
  }

  return true;
}

struct SafeBrowsingUrlCheckResult {
  std::string url;
  int policy = -1;
  std::string notification;
  VendorInfo vendor_info;
  std::vector<std::string> jump_url;
  std::string mapping_type;

  SafeBrowsingUrlCheckResult() { policy = -1; }
  ~SafeBrowsingUrlCheckResult() = default;

  static void RegisterJSONConverter(
      base::JSONValueConverter<SafeBrowsingUrlCheckResult>* converter) {
    converter->RegisterCustomValueField<std::string>(
        "url", &SafeBrowsingUrlCheckResult::url, &ParseString);
    converter->RegisterCustomValueField<int>(
        "policy", &SafeBrowsingUrlCheckResult::policy, &ParseInteger);
    converter->RegisterCustomValueField<std::string>(
        "notification", &SafeBrowsingUrlCheckResult::notification,
        &ParseString);
    converter->RegisterCustomValueField<VendorInfo>(
        "vendorInfo", &SafeBrowsingUrlCheckResult::vendor_info,
        &ParseCustomValueResult);
    converter->RegisterCustomValueField<std::vector<std::string>>(
        "jumpUrls", &SafeBrowsingUrlCheckResult::jump_url, &ParseStringVector);
    converter->RegisterCustomValueField<std::string>(
        "mappingType", &SafeBrowsingUrlCheckResult::mapping_type, &ParseString);
  }
};

struct SingleHashResult {
  std::string url_hash;
  std::string hw_code;
  int policy = -1;
  std::vector<std::string> jump_url;

  SingleHashResult() = default;
  ~SingleHashResult() = default;

  static void RegisterJSONConverter(
      base::JSONValueConverter<SingleHashResult>* converter) {
    converter->RegisterCustomValueField<std::string>(
        "urlHash", &SingleHashResult::url_hash, &ParseString);
    converter->RegisterCustomValueField<std::string>(
        "hwCode", &SingleHashResult::hw_code, &ParseString);
    converter->RegisterCustomValueField<int>(
        "policy", &SingleHashResult::policy, &ParseInteger);
    converter->RegisterCustomValueField<std::vector<std::string>>(
        "jumpUrls", &SingleHashResult::jump_url, &ParseStringVector);
  }
};

template <typename T>
static bool ParseCustomValueVector(const base::Value* value,
                                   std::vector<T>* result) {
  if (result && value && value->is_list()) {
    result->clear();
    for (size_t i = 0; i < value->GetList().size(); i++) {
      const auto& element = value->GetList()[i];
      base::JSONValueConverter<T> converter;
      T single_item;
      if (!converter.Convert(element, &single_item)) {
        return false;
      }

      result->emplace_back(single_item);
      if (result->size() >= kCloudControlVectorMaxSize) {
        LOG(WARNING) << "ParseConfigVector reach the limit size.";
        break;
      }
    }
  }

  return true;
}

struct SafeBrowsingHashCheckResult {
  std::string mapping_type;
  std::vector<SingleHashResult> hash_result_list;

  SafeBrowsingHashCheckResult() = default;
  ~SafeBrowsingHashCheckResult() = default;

  static void RegisterJSONConverter(
      base::JSONValueConverter<SafeBrowsingHashCheckResult>* converter) {
    converter->RegisterCustomValueField(
        "mappingType", &SafeBrowsingHashCheckResult::mapping_type,
        &ParseString);
    converter->RegisterCustomValueField<std::vector<SingleHashResult>>(
        "fullUrlHashlist", &SafeBrowsingHashCheckResult::hash_result_list,
        &ParseCustomValueVector);
  }
};

struct SafeBrowsingResponse {
  std::string status;
  int code = 0;
  std::string reason;
  SafeBrowsingUrlCheckResult url_check_result;
  std::vector<SafeBrowsingHashCheckResult> hash_check_result;

  SafeBrowsingResponse() = default;
  ~SafeBrowsingResponse() = default;

  static void RegisterJSONConverter(
      base::JSONValueConverter<SafeBrowsingResponse>* converter) {
    converter->RegisterCustomValueField<std::string>(
        "status", &SafeBrowsingResponse::status, &ParseString);
    converter->RegisterCustomValueField<int>(
        "code", &SafeBrowsingResponse::code, &ParseInteger);
    converter->RegisterCustomValueField<std::string>(
        "reason", &SafeBrowsingResponse::reason, &ParseString);
    converter->RegisterCustomValueField<SafeBrowsingUrlCheckResult>(
        "urlCheckResult", &SafeBrowsingResponse::url_check_result,
        &ParseCustomValueResult);
    converter
        ->RegisterCustomValueField<std::vector<SafeBrowsingHashCheckResult>>(
            "hashCheckResult", &SafeBrowsingResponse::hash_check_result,
            &ParseCustomValueVector);
  }
};

}  // namespace ohos_safe_browsing

#endif  // CEF_LIBCEF_BROWSER_SAFE_BROWSING_SAFE_BROWSING_RESPONSE_H_
