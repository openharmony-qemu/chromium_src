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

#ifndef OHOS_ADAPTER_PERMISSION_MANAGER_PERMISSION_MANAGER_ADAPTER_H
#define OHOS_ADAPTER_PERMISSION_MANAGER_PERMISSION_MANAGER_ADAPTER_H

#include <string>
#include "ohos/adapter/export.h"

namespace ohos::adapter {
namespace permission {

enum class OHOSPermissionType : int32_t {
  NO,
  LOCATION,
  CAMERA,
  MICROPHONE,
  DESKTOP_CAPTURE,
  BLUETOOTH,
  PASTEBOARD
};

enum class PermissionActivationResult : int32_t {
  SUCCESS = 1,
  JS_BINDING_ERROR = 0,
  EMPTY_ARRAY = -1,
  PERMISSION_VERIFICATION_FAILED = 201,
  PARAMETER_ERROR = 401,
  CAPABILITY_NOT_SUPPORTED = 801,
  OPERATION_NOT_PERMITTED = 13900001,
  UNKNOWN_ERROR = 13900042
};

//The request is rejected by the system. The return value is - 1.
const int K_REQUEST_PERMISSION_DENIED = -1;
const int K_OPEN_PERMISSION_CONFIRM_FAIL = -2;
const int K_REQUEST_PERMISSION_FAIL = -1;

class ADAPTER_EXPORT_API PermissionManagerAdapter {
 public:
  static constexpr const char* kOhosPermissionLocation =
    "ohos.permission.LOCATION";
  static constexpr const char* kOhosPermissionApproximatelyLocation =
    "ohos.permission.APPROXIMATELY_LOCATION";
  static constexpr const char* kOhosPermissionMicrophone =
    "ohos.permission.MICROPHONE";
  static constexpr const char* kOhosPermissionCamera =
    "ohos.permission.CAMERA";
  static constexpr const char* kOhosPermissionPastboard =
    "ohos.permission.READ_PASTEBOARD";
  static constexpr const char* kOhosPermissionBluetooth =
    "ohos.permission.ACCESS_BLUETOOTH";
  static constexpr const char* kOhosPermissionDirectoryDownload =
    "ohos.permission.READ_WRITE_DOWNLOAD_DIRECTORY";
  static constexpr const char* kOhosPermissionDirectoryDesktop =
    "ohos.permission.READ_WRITE_DESKTOP_DIRECTORY";
  static constexpr const char* kOhosPermissionDirectoryDocument =
    "ohos.permission.READ_WRITE_DOCUMENTS_DIRECTORY";
  static bool RequestPermission(const OHOSPermissionType& type);
  static bool CheckPermission(const OHOSPermissionType& type);
  static bool CheckAndRequestPermission(const OHOSPermissionType& type_);
  static void FileAccessPersist(const std::string &url);
  static PermissionActivationResult ActivateFileAccessPersist(const std::string &url);
  static int OpenPermissionConfirm(const OHOSPermissionType& type);
  static int RequestPermissionCode(const OHOSPermissionType& type);
};

}  // namespace permission
}  // namespace ohos::adapter

#endif
