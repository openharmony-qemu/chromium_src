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

#ifndef WEBVIEW_MESSAGE_FFI_H
#define WEBVIEW_MESSAGE_FFI_H

#include <cstdint>
#include "ffi_remote_data.h"
#include "webview_utils.h"
#include "cj_common_ffi.h"

extern "C" {
    // WebMessagePort
    FFI_EXPORT void FfiOHOSWebMessagePortPostMessageEvent(int64_t msgPortId, char* stringValue, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessagePortPostMessageEventArr(int64_t msgPortId, CArrUI8 arrBuf, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessagePortPostMessageEventExt(int64_t msgPortId, int64_t msgExtId, int32_t *errCode);
    FFI_EXPORT bool FfiOHOSWebMessagePortIsExtentionType(int64_t msgPortId);
    FFI_EXPORT void FfiOHOSWebMessagePortOnMessageEvent(int64_t msgPortId,
        void (*callback)(OHOS::Webview::RetWebMessage), int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessagePortOnMessageEventExt(int64_t msgPortId,
        void (*callback)(int64_t), int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessagePortClose(int64_t msgPortId, int32_t *errCode);

    // WebMessageExt
    FFI_EXPORT int64_t FfiOHOSWebMessageExtImplConstructor();
    FFI_EXPORT int32_t FfiOHOSWebMessageExtImplGetType(int64_t msgExtId, int32_t *errCode);
    FFI_EXPORT char* FfiOHOSWebMessageExtImplGetString(int64_t msgExtId, int32_t *errCode);
    FFI_EXPORT OHOS::Webview::RetNumber FfiOHOSWebMessageExtImplGetNumber(int64_t msgExtId, int32_t *errCode);
    FFI_EXPORT bool FfiOHOSWebMessageExtImplGetBoolean(int64_t msgExtId, int32_t *errCode);
    FFI_EXPORT CArrUI8 FfiOHOSWebMessageExtImplGetArrayBuffer(int64_t msgExtId, int32_t *errCode);
    FFI_EXPORT OHOS::Webview::CArrValue FfiOHOSWebMessageExtImplGetArray(int64_t msgExtId, int32_t *errCode);
    FFI_EXPORT OHOS::Webview::CError FfiOHOSWebMessageExtImplGetError(int64_t msgExtId, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessageExtImplSetType(int64_t msgExtId, int32_t type, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessageExtImplSetString(int64_t msgExtId, char* message, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessageExtImplSetNumber(int64_t msgExtId, double value, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessageExtImplSetBoolean(int64_t msgExtId, bool value, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessageExtImplSetArrayBuffer(int64_t msgExtId,
        CArrUI8 value, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessageExtImplSetArrayString(int64_t msgExtId,
        CArrString value, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessageExtImplSetArrayInt(int64_t msgExtId,
        CArrI64 value, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessageExtImplSetArrayDouble(int64_t msgExtId,
        OHOS::Webview::CArrDouble value, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessageExtImplSetArrayBoolean(int64_t msgExtId,
        OHOS::Webview::CArrBool value, int32_t *errCode);
    FFI_EXPORT void FfiOHOSWebMessageExtImplSetError(int64_t msgExtId,
        OHOS::Webview::CError value, int32_t *errCode);

    // WebJsMessageExtImpl
    FFI_EXPORT int32_t FfiOHOSJsMessageExtImplGetType(int64_t jsExtId, int32_t *errCode);
    FFI_EXPORT char* FfiOHOSJsMessageExtImplGetString(int64_t jsExtId, int32_t *errCode);
    FFI_EXPORT OHOS::Webview::RetNumber FfiOHOSJsMessageExtImplGetNumber(int64_t jsExtId, int32_t *errCode);
    FFI_EXPORT bool FfiOHOSJsMessageExtImplGetBoolean(int64_t jsExtId, int32_t *errCode);
    FFI_EXPORT CArrUI8 FfiOHOSJsMessageExtImplGetArrayBuffer(int64_t jsExtId, int32_t *errCode);
    FFI_EXPORT OHOS::Webview::CArrValue FfiOHOSJsMessageExtImplGetArray(int64_t jsExtId, int32_t *errCode);
}
#endif // WEBVIEW_MESSAGE_FFI_H