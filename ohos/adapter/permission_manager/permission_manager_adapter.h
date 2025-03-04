// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_PERMISSION_MANAGER_PERMISSION_MANAGER_ADAPTER_H
#define OHOS_ADAPTER_PERMISSION_MANAGER_PERMISSION_MANAGER_ADAPTER_H

#include <string>

#include "ohos/adapter/export.h"

namespace ohos::adapter {
namespace permission {

constexpr int CONFIRM_INDEX = 0;

enum class OHOSPermissionType : int32_t {
  NO,
  LOCATION,
  CAMERA,
  MICROPHONE,
  DESKTOP_CAPTURE,
  USER_DOWNLOAD_DIR,
  USER_DESKTOP_DIR,
  USER_DOCUMENT_DIR,
  BLUETOOTH,
  PASTEBOARD
};

class ADAPTER_EXPORT_API PermissionManagerAdapter {
 public:
  static bool RequestPermission(const OHOSPermissionType& type);
  static bool CheckPermission(const OHOSPermissionType& type);
  static int OpenPermissionConfirm();
};

}  // namespace permission
}  // namespace ohos::adapter

#endif
