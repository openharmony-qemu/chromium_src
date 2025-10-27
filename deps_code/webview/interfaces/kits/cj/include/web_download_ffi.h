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

#ifndef WEB_DOWNLOAD_FFI_H
#define WEB_DOWNLOAD_FFI_H

#include <cstdint>

#include "cj_common_ffi.h"
#include "ffi_remote_data.h"
#include "webview_utils.h"

extern "C" {
// WebDownloadItemImpl
FFI_EXPORT int64_t FfiOHOSWebDownloadItemImplConstructor();
FFI_EXPORT RetDataCString FfiOHOSWebDownloadItemImplGetGuid(int64_t id);
FFI_EXPORT int64_t FfiOHOSWebDownloadItemImplGetCurrentSpeed(int64_t id, int32_t* errCode);
FFI_EXPORT int64_t FfiOHOSWebDownloadItemImplGetPercentComplete(int64_t id, int32_t* errCode);
FFI_EXPORT int64_t FfiOHOSWebDownloadItemImplGetTotalBytes(int64_t id, int32_t* errCode);
FFI_EXPORT int64_t FfiOHOSWebDownloadItemImplGetReceivedBytes(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebDownloadItemImplGetState(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebDownloadItemImplGetLastErrorCode(int64_t id, int32_t* errCode);
FFI_EXPORT RetDataCString FfiOHOSWebDownloadItemImplGetMethod(int64_t id);
FFI_EXPORT RetDataCString FfiOHOSWebDownloadItemImplGetMimeType(int64_t id);
FFI_EXPORT RetDataCString FfiOHOSWebDownloadItemImplGetUrl(int64_t id);
FFI_EXPORT RetDataCString FfiOHOSWebDownloadItemImplGetSuggestedFileName(int64_t id);
FFI_EXPORT RetDataCString FfiOHOSWebDownloadItemImplGetFullPath(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebDownloadItemImplStart(int64_t id, char* downloadPath);
FFI_EXPORT int32_t FfiOHOSWebDownloadItemImplCancel(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebDownloadItemImplPause(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebDownloadItemImplResume(int64_t id);
FFI_EXPORT CArrUI8 FfiOHOSWebDownloadItemImplSerialize(int64_t id, int32_t* errCode);
FFI_EXPORT int64_t FfiOHOSWebDownloadItemImplDeserialize(CArrUI8 serializedData, int32_t* errCode);

// WebDownloadDelegateImpl
FFI_EXPORT int64_t FfiOHOSWebDownloadDelegateImplConstructor();
FFI_EXPORT void FfiOHOSWebDownloadDelegateImplOnBeforeDownload(int64_t id, void (*callback)(int64_t));
FFI_EXPORT void FfiOHOSWebDownloadDelegateImplOnDownloadUpdated(int64_t id, void (*callback)(int64_t));
FFI_EXPORT void FfiOHOSWebDownloadDelegateImplOnDownloadFinish(int64_t id, void (*callback)(int64_t));
FFI_EXPORT void FfiOHOSWebDownloadDelegateImplOnDownloadFailed(int64_t id, void (*callback)(int64_t));

// WebDownloadManagerImpl
FFI_EXPORT void FfiOHOSWebDownloadManagerImplSetDownloadDelegate(int64_t delegateId);
FFI_EXPORT int32_t FfiOHOSWebDownloadManagerImplResumeDownload(int64_t itemId);
}

#endif // WEB_DOWNLOAD_FFI_H