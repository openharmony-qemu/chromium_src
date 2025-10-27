/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WEB_SCHEME_HANDLER_RESPONSE_FFI_H
#define WEB_SCHEME_HANDLER_RESPONSE_FFI_H

#include <cstdint>

#include "cj_common_ffi.h"
#include "ffi_remote_data.h"
#include "webview_utils.h"

extern "C" {
// WebSchemeHandlerResponse
FFI_EXPORT int64_t FfiWebSchemeHandlerResponseConstructor();
FFI_EXPORT RetDataCString FfiWebSchemeHandlerResponseGetUrl(int64_t id);
FFI_EXPORT RetDataCString FfiWebSchemeHandlerResponseGetStatusText(int64_t id);
FFI_EXPORT RetDataCString FfiWebSchemeHandlerResponseGetMimeType(int64_t id);
FFI_EXPORT RetDataCString FfiWebSchemeHandlerResponseGetEncoding(int64_t id);
FFI_EXPORT RetDataCString FfiWebSchemeHandlerResponseGetHeaderByName(int64_t id, const char* name);
FFI_EXPORT int32_t FfiWebSchemeHandlerResponseGetNetErrorCode(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiWebSchemeHandlerResponseGetStatus(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiWebSchemeHandlerResponseSetStatus(int64_t id, int32_t status);
FFI_EXPORT int32_t FfiWebSchemeHandlerResponseSetStatusText(int64_t id, const char* statusText);
FFI_EXPORT int32_t FfiWebSchemeHandlerResponseSetUrl(int64_t id, const char* url);
FFI_EXPORT int32_t FfiWebSchemeHandlerResponseSetMimeType(int64_t id, const char* mimeType);
FFI_EXPORT int32_t FfiWebSchemeHandlerResponseSetEncoding(int64_t id, const char* encoding);
FFI_EXPORT int32_t FfiWebSchemeHandlerResponseSetHeaderByName(
    int64_t id, const char* name, const char* value, bool overwrite);
FFI_EXPORT int32_t FfiWebSchemeHandlerResponseSetErrorCode(int64_t id, int32_t code);
}

#endif // WEB_SCHEME_HANDLER_RESPONSE_FFI_H