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

#include "permission_manager_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter {
namespace permission {

namespace {
std::string ConvertTypeEnumToTypeString(
    const OHOSPermissionType& permission_type) {
  std::string type{};
  switch (permission_type) {
    case OHOSPermissionType::LOCATION:
      type = "location";
      break;
    case OHOSPermissionType::CAMERA:
      type = "camera";
      break;
    case OHOSPermissionType::MICROPHONE:
      type = "microphone";
      break;
    case OHOSPermissionType::DESKTOP_CAPTURE:
      type = "capture";
      break;
    case OHOSPermissionType::BLUETOOTH:
      type = "bluetooth";
      break;
    case OHOSPermissionType::PASTEBOARD:
      type = "pasteboard";
      break;
    default:
      break;
  }
  return type;
}

PermissionActivationResult GetPermissionActivationResult(const int32_t& code) {
  switch (code) {
    case 1:
      return PermissionActivationResult::SUCCESS;
    case 0:
      return PermissionActivationResult::JS_BINDING_ERROR;
    case -1:
      return PermissionActivationResult::EMPTY_ARRAY;
    case 201:
      return PermissionActivationResult::PERMISSION_VERIFICATION_FAILED;
    case 401:
      return PermissionActivationResult::PARAMETER_ERROR;
    case 801:
      return PermissionActivationResult::CAPABILITY_NOT_SUPPORTED;
    case 13900001:
      return PermissionActivationResult::OPERATION_NOT_PERMITTED;
    case 13900042:
      LOGE("The interface used for activation returned an unknown error");
      return PermissionActivationResult::UNKNOWN_ERROR;
    default:
      LOGE("Invalid error code: %{public}d", code);
      return PermissionActivationResult::UNKNOWN_ERROR;
  }
}
}  // namespace

bool PermissionManagerAdapter::RequestPermission(
    const OHOSPermissionType& type) {
  int request_permission_result = RequestPermissionCode(type);
  LOGI(
      "PermissionManagerAdapter::RequestPermission request_permission_result "
      "is %{public}d", request_permission_result);
  return (request_permission_result == 0);
}

int PermissionManagerAdapter::RequestPermissionCode(
    const OHOSPermissionType& type) {
  if (auto jsFunc = ohos::adapter::GetJSFunction("PermissionManagerAdapter.RequestPermissionCode")) {
    auto promise = std::make_shared<std::promise<int>>();
    auto future = promise->get_future();
    std::function<void(int)> callback = [promise](int flag) {
      promise->set_value(flag);
    };
    jsFunc->Invoke<void>(ConvertTypeEnumToTypeString(type), callback);
    return future.get();
  }
  LOGE("RequestPermissions js binding error: function undefined");
  return K_REQUEST_PERMISSION_FAIL;
}

bool PermissionManagerAdapter::CheckPermission(
    const OHOSPermissionType& type) {
  std::promise<bool> promise;
  std::function<void(bool)> is_permitted = [&promise](bool flag) {
    promise.set_value(flag);
  };
  if (auto jsFunc = ohos::adapter::GetJSFunction("PermissionManagerAdapter.CheckPermissions")) {
    jsFunc->Invoke<void>(ConvertTypeEnumToTypeString(type), is_permitted);
  } else {
    LOGE("CheckPermissions js binding error: function undefined");
    return false;
  }

  return promise.get_future().get();
}

bool PermissionManagerAdapter::CheckAndRequestPermission(
    const OHOSPermissionType& type) {
// 1. check permission
    if (!CheckPermission(type)) {
// 2. if no permission, request permission
        int request_permisson_result = RequestPermissionCode(type);
        return (request_permisson_result == 0);
    }
    return true;
}

void PermissionManagerAdapter::FileAccessPersist(const std::string &url) {
  std::vector <std::string> urls;
  urls.push_back(url);
  if (auto jsFunc = ohos::adapter::GetJSFunction("PermissionManagerAdapter.FileAccessPersist")) {
    jsFunc->Invoke<void>(std::move(urls));
  } else {
    LOGE("FileAccessPersist js binding error: function undefined");
  }
}

PermissionActivationResult PermissionManagerAdapter::ActivateFileAccessPersist(
    const std::string& url) {
  std::vector<std::string> urls;
  urls.push_back(url);
  std::promise<int32_t> promise;
  std::function<void(int32_t)> result_code = [&promise](int32_t ret) {
      promise.set_value(ret);
  };
  if (auto jsFunc = ohos::adapter::GetJSFunction(
      "PermissionManagerAdapter.ActivateFileAccessPersist")) {
      jsFunc->Invoke<void>(std::move(urls), result_code);
  } else {
      LOGE("ActivateFileAccessPersist js binding error: function undefined");
      return PermissionActivationResult::JS_BINDING_ERROR;
  }
  return GetPermissionActivationResult(promise.get_future().get());
}

int PermissionManagerAdapter::OpenPermissionConfirm(const OHOSPermissionType& type) {
  if (auto getDirFunc = ohos::adapter::GetJSFunction(
      "PermissionManagerAdapter.OpenPermissionConfirm")) {
    auto insert_promise = std::make_shared<std::promise<int>>();
    auto future = insert_promise->get_future();
    std::function<void(int)> callback = [insert_promise](int ret) {
      insert_promise->set_value(ret);
    };
    getDirFunc->Invoke<void>(ConvertTypeEnumToTypeString(type), callback);
    auto status = future.wait_for(std::chrono::seconds(3));
    if (status == std::future_status::timeout) {
      LOGE("PermissionManagerAdapter::OpenPermissionConfirm Wait timeout");
      return K_REQUEST_PERMISSION_FAIL;
    }
    return future.get();
  }
  LOGE("PermissionManagerAdapter OpenPermissionConfirm js function failed.");
  return K_REQUEST_PERMISSION_FAIL;
}

}  // namespace permission
}  // namespace ohos::adapter
