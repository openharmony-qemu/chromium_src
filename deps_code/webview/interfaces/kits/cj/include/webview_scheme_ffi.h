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

#ifndef WEBVIEW_SCHEME_FFI_H
#define WEBVIEW_SCHEME_FFI_H

#include "ffi_remote_data.h"
#include "webview_utils.h"
#include "cj_common_ffi.h"

extern "C" {
    // WebHttpBodyStream
    FFI_EXPORT int64_t FfiWebHttpBodyStreamConstructor();
    FFI_EXPORT void FfiWebHttpBodyStreaminitialize(int64_t id, int32_t *errCode);
    FFI_EXPORT int32_t FfiWebHttpBodyStreamRead(int32_t bufLen, int64_t id);
    FFI_EXPORT bool FfiWebHttpBodyStreamIsChunked(int64_t id, int32_t *errCode);
    FFI_EXPORT bool FfiWebHttpBodyStreamIsEof(int64_t id, int32_t *errCode);
    FFI_EXPORT bool FfiWebHttpBodyStreamIsInMemory(int64_t id, int32_t *errCode);
    FFI_EXPORT uint64_t FfiWebHttpBodyStreamGetPostion(int64_t id, int32_t *errCode);
    FFI_EXPORT uint64_t FfiWebHttpBodyStreamGetSize(int64_t id, int32_t *errCode);

    // WebSchemeHandlerRequest
    FFI_EXPORT int64_t FfiWebSchemeHandlerRequestConstructor();
    FFI_EXPORT int32_t FfiWebSchemeHandlerRequestGetRequestResourceType(int64_t id, int32_t *errCode);
    FFI_EXPORT bool FfiWebSchemeHandlerRequestHasGesture(int64_t id, int32_t *errCode);
    FFI_EXPORT bool FfiWebSchemeHandlerRequestIsMainFrame(int64_t id, int32_t *errCode);
    FFI_EXPORT bool FfiWebSchemeHandlerRequestIsRedirect(int64_t id, int32_t *errCode);
    FFI_EXPORT RetDataCString FfiWebSchemeHandlerRequestGetRequestUrl(int64_t id);
    FFI_EXPORT RetDataCString FfiWebSchemeHandlerRequestGetMethod(int64_t id);
    FFI_EXPORT RetDataCString FfiWebSchemeHandlerRequestGetReferrer(int64_t id);
    FFI_EXPORT RetDataCString FfiWebSchemeHandlerRequestGetFrameUrl(int64_t id);
    FFI_EXPORT OHOS::Webview::ArrWebHeader FfiWebSchemeHandlerRequestGetHeader(int64_t id, int32_t *errCode);
    FFI_EXPORT int64_t FfiWebSchemeHandlerRequestGetHttpBodyStream(int64_t id, int32_t *errCode);

    // WebResourceHandler
    FFI_EXPORT int64_t FfiWebResourceHandlerConstructor();
    FFI_EXPORT void FfiWebResourceHandlerDidReceiveResponse(int64_t id, int32_t *errCode, int64_t responceid);
    FFI_EXPORT void FfiWebResourceHandlerDidReceiveResponseBody(
        int64_t id, int32_t *errCode, CArrUI8 buffer, int64_t buflen);
    FFI_EXPORT void FfiWebResourceHandlerDidFinish(int64_t id, int32_t *errCode);
    FFI_EXPORT void FfiWebResourceHandlerDidFail(int64_t id, int32_t *errCode, int32_t errorcode);

    // WebSchemeHandler
    FFI_EXPORT int64_t FfiWebSchemeHandlerConstructor();
    FFI_EXPORT void FfiWebSchemeHandlerOnRequestStart(int64_t id, bool (*callback)(int64_t, int64_t));
    FFI_EXPORT void FfiWebSchemeHandlerOnRequestStop(int64_t id, void (*callback)(int64_t));
}

#endif // WEBVIEW_SCHEME_FFI_H