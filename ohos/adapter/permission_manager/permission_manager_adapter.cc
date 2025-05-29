// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
    case OHOSPermissionType::USER_DOWNLOAD_DIR:
      type = "directory_download";
      break;
    case OHOSPermissionType::USER_DESKTOP_DIR:
      type = "directory_desktop";
      break;
    case OHOSPermissionType::USER_DOCUMENT_DIR:
      type = "directory_document";
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
}  // namespace

bool PermissionManagerAdapter::RequestPermission(
    const OHOSPermissionType& type) {
  std::promise<bool> promise;
  std::function<void(bool)> is_permitted = [&promise](bool flag) {
    promise.set_value(flag);
  };
  if (auto jsFunc = ohos::adapter::GetJSFunction(
          "PermissionManagerAdapter.RequestPermissions")) {
    jsFunc->Invoke<void>(ConvertTypeEnumToTypeString(type), is_permitted);
  } else {
    LOGE("RequestPermissions js binding error: function undefined");
    return false;
  }

  return promise.get_future().get();
}

bool PermissionManagerAdapter::CheckPermission(const OHOSPermissionType& type) {
  std::promise<bool> promise;
  std::function<void(bool)> is_permitted = [&promise](bool flag) {
    promise.set_value(flag);
  };
  if (auto jsFunc = ohos::adapter::GetJSFunction(
          "PermissionManagerAdapter.CheckPermissions")) {
    jsFunc->Invoke<void>(ConvertTypeEnumToTypeString(type), is_permitted);
  } else {
    LOGE("CheckPermissions js binding error: function undefined");
    return false;
  }

  return promise.get_future().get();
}

int PermissionManagerAdapter::OpenPermissionConfirm() {
  if (auto getDirFunc = ohos::adapter::GetJSFunction(
          "PermissionManagerAdapter.OpenPermissionConfirm")) {
    std::promise<int> insert_promise;
    std::function<void(int)> callback = [&insert_promise](int ret) {
      insert_promise.set_value(ret);
    };
    getDirFunc->Invoke<void>(callback);
    return insert_promise.get_future().get();
  }
  LOGE("PermissionManagerAdapter OpenPermissionConfirm js function failed.");
  return -1;
}

}  // namespace permission
}  // namespace ohos::adapter
