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

#include "web_download_ffi.h"

#include "cj_lambda.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "web_download.pb.h"
#include "web_download_delegate_impl.h"
#include "web_download_item_impl.h"
#include "web_download_manager_impl.h"
#include "web_errors.h"
#include "webview_controller_impl.h"
#include "webview_log.h"
#include "webview_utils.h"

using namespace OHOS::FFI;
using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {
extern "C" {
// WebDownloadItemImpl
int64_t FfiOHOSWebDownloadItemImplConstructor()
{
    auto nativeWebDownloadItemImpl = FFIData::Create<WebDownloadItemImpl>();
    if (nativeWebDownloadItemImpl == nullptr) {
        WEBVIEWLOGE("new web download item failed");
        return -1;
    }
    return nativeWebDownloadItemImpl->GetID();
}

RetDataCString FfiOHOSWebDownloadItemImplGetGuid(int64_t id)
{
    RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        return ret;
    }
    std::string guid = nativeWebDownloadItemImpl->guid;
    ret.code = NWebError::NO_ERROR;
    ret.data = MallocCString(guid);
    return ret;
}

int64_t FfiOHOSWebDownloadItemImplGetCurrentSpeed(int64_t id, int32_t* errCode)
{
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        *errCode = NWebError::INIT_ERROR;
        return -1;
    }
    *errCode = NWebError::NO_ERROR;
    return static_cast<int64_t>(nativeWebDownloadItemImpl->currentSpeed);
}

int64_t FfiOHOSWebDownloadItemImplGetPercentComplete(int64_t id, int32_t* errCode)
{
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        *errCode = NWebError::INIT_ERROR;
        return -1;
    }
    *errCode = NWebError::NO_ERROR;
    return static_cast<int64_t>(nativeWebDownloadItemImpl->percentComplete);
}

int64_t FfiOHOSWebDownloadItemImplGetTotalBytes(int64_t id, int32_t* errCode)
{
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        *errCode = NWebError::INIT_ERROR;
        return -1;
    }
    *errCode = NWebError::NO_ERROR;
    return static_cast<int64_t>(nativeWebDownloadItemImpl->totalBytes);
}

int64_t FfiOHOSWebDownloadItemImplGetReceivedBytes(int64_t id, int32_t* errCode)
{
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        *errCode = NWebError::INIT_ERROR;
        return -1;
    }
    *errCode = NWebError::NO_ERROR;
    return static_cast<int64_t>(nativeWebDownloadItemImpl->receivedBytes);
}

int32_t FfiOHOSWebDownloadItemImplGetState(int64_t id, int32_t* errCode)
{
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        *errCode = NWebError::INIT_ERROR;
        return -1;
    }
    *errCode = NWebError::NO_ERROR;
    return static_cast<int32_t>(nativeWebDownloadItemImpl->state);
}

int32_t FfiOHOSWebDownloadItemImplGetLastErrorCode(int64_t id, int32_t* errCode)
{
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        *errCode = NWebError::INIT_ERROR;
        return -1;
    }
    *errCode = NWebError::NO_ERROR;
    return static_cast<int32_t>(nativeWebDownloadItemImpl->lastErrorCode);
}

RetDataCString FfiOHOSWebDownloadItemImplGetMethod(int64_t id)
{
    RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        return ret;
    }
    std::string methodValue = nativeWebDownloadItemImpl->method;
    ret.code = NWebError::NO_ERROR;
    ret.data = MallocCString(methodValue);
    return ret;
}

RetDataCString FfiOHOSWebDownloadItemImplGetMimeType(int64_t id)
{
    RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        return ret;
    }
    std::string mimeTypeValue = nativeWebDownloadItemImpl->mimeType;
    ret.code = NWebError::NO_ERROR;
    ret.data = MallocCString(mimeTypeValue);
    return ret;
}

RetDataCString FfiOHOSWebDownloadItemImplGetUrl(int64_t id)
{
    RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        return ret;
    }
    std::string urlValue = nativeWebDownloadItemImpl->url;
    ret.code = NWebError::NO_ERROR;
    ret.data = MallocCString(urlValue);
    return ret;
}

RetDataCString FfiOHOSWebDownloadItemImplGetSuggestedFileName(int64_t id)
{
    RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        return ret;
    }
    std::string fileNameValue = nativeWebDownloadItemImpl->suggestedFileName;
    ret.code = NWebError::NO_ERROR;
    ret.data = MallocCString(fileNameValue);
    return ret;
}

RetDataCString FfiOHOSWebDownloadItemImplGetFullPath(int64_t id)
{
    RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        return ret;
    }
    std::string fullPath = nativeWebDownloadItemImpl->fullPath;
    ret.code = NWebError::NO_ERROR;
    ret.data = MallocCString(fullPath);
    return ret;
}

int32_t FfiOHOSWebDownloadItemImplStart(int64_t id, char* downloadPath)
{
    std::string sDownloadPath = downloadPath;
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        return NWebError::INIT_ERROR;
    }
    nativeWebDownloadItemImpl->downloadPath = sDownloadPath;
    WebDownload_Continue(
        nativeWebDownloadItemImpl->before_download_callback, nativeWebDownloadItemImpl->downloadPath.c_str());
    return NWebError::NO_ERROR;
}

int32_t FfiOHOSWebDownloadItemImplCancel(int64_t id)
{
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        return NWebError::INIT_ERROR;
    }
    if (nativeWebDownloadItemImpl->download_item_callback) {
        WebDownload_Cancel(nativeWebDownloadItemImpl->download_item_callback);
    } else if (nativeWebDownloadItemImpl->before_download_callback) {
        WebDownload_CancelBeforeDownload(nativeWebDownloadItemImpl->before_download_callback);
    } else {
        WEBVIEWLOGE("[DOWNLOAD] WebDownloadItem::Cancel failed for callback nullptr");
    }
    return NWebError::NO_ERROR;
}

int32_t FfiOHOSWebDownloadItemImplPause(int64_t id)
{
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        return NWebError::INIT_ERROR;
    }
    NWebDownloadItemState state =
        WebDownload_GetItemState(nativeWebDownloadItemImpl->nwebId, nativeWebDownloadItemImpl->webDownloadId);
    if (state != NWebDownloadItemState::IN_PROGRESS && state != NWebDownloadItemState::PENDING) {
        return NWebError::DOWNLOAD_NOT_START;
    }
    if (nativeWebDownloadItemImpl->download_item_callback) {
        WebDownload_Pause(nativeWebDownloadItemImpl->download_item_callback);
    } else if (nativeWebDownloadItemImpl->before_download_callback) {
        WebDownload_PauseBeforeDownload(nativeWebDownloadItemImpl->before_download_callback);
    } else {
        WEBVIEWLOGE("[DOWNLOAD] WebDownloadItem::Pause failed for callback nullptr");
    }
    return NWebError::NO_ERROR;
}

int32_t FfiOHOSWebDownloadItemImplResume(int64_t id)
{
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        return NWebError::INIT_ERROR;
    }
    NWebDownloadItemState state =
        WebDownload_GetItemState(nativeWebDownloadItemImpl->nwebId, nativeWebDownloadItemImpl->webDownloadId);
    if (state != NWebDownloadItemState::PAUSED) {
        return NWebError::DOWNLOAD_NOT_PAUSED;
    }

    if (nativeWebDownloadItemImpl->download_item_callback) {
        WebDownload_Resume(nativeWebDownloadItemImpl->download_item_callback);
    } else if (nativeWebDownloadItemImpl->before_download_callback) {
        WebDownload_ResumeBeforeDownload(nativeWebDownloadItemImpl->before_download_callback);
    } else {
        WEBVIEWLOGE("[DOWNLOAD] WebDownloadItem::Resume failed for callback nullptr");
    }
    return NWebError::NO_ERROR;
}

CArrUI8 FfiOHOSWebDownloadItemImplSerialize(int64_t id, int32_t* errCode)
{
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(id);
    if (!nativeWebDownloadItemImpl) {
        *errCode = NWebError::INIT_ERROR;
        return CArrUI8 { nullptr, 0 };
    }

    browser_service::WebDownload webDownloadPb;
    webDownloadPb.set_web_download_id(nativeWebDownloadItemImpl->webDownloadId);
    webDownloadPb.set_current_speed(nativeWebDownloadItemImpl->currentSpeed);
    webDownloadPb.set_percent_complete(nativeWebDownloadItemImpl->percentComplete);
    webDownloadPb.set_total_bytes(nativeWebDownloadItemImpl->totalBytes);
    webDownloadPb.set_received_bytes(nativeWebDownloadItemImpl->receivedBytes);
    webDownloadPb.set_guid(nativeWebDownloadItemImpl->guid);
    webDownloadPb.set_full_path(nativeWebDownloadItemImpl->fullPath);
    webDownloadPb.set_url(nativeWebDownloadItemImpl->url);
    webDownloadPb.set_etag(nativeWebDownloadItemImpl->etag);
    webDownloadPb.set_original_url(nativeWebDownloadItemImpl->originalUrl);
    webDownloadPb.set_suggested_file_name(nativeWebDownloadItemImpl->suggestedFileName);
    webDownloadPb.set_content_disposition(nativeWebDownloadItemImpl->contentDisposition);
    webDownloadPb.set_mime_type(nativeWebDownloadItemImpl->mimeType);
    webDownloadPb.set_last_modified(nativeWebDownloadItemImpl->lastModified);
    webDownloadPb.set_state(
        static_cast<browser_service::WebDownload::WebDownloadState>(nativeWebDownloadItemImpl->state));
    webDownloadPb.set_method(nativeWebDownloadItemImpl->method);
    webDownloadPb.set_last_error_code(nativeWebDownloadItemImpl->lastErrorCode);
    webDownloadPb.set_received_slices(nativeWebDownloadItemImpl->receivedSlices);
    webDownloadPb.set_download_path(nativeWebDownloadItemImpl->downloadPath);

    std::string webDownloadValue;
    webDownloadPb.SerializeToString(&webDownloadValue);
    uint8_t* result = MallocUInt8(webDownloadValue);
    if (result == nullptr) {
        WEBVIEWLOGE("[DOWNLOAD] malloc failed");
        *errCode = NWebError::NEW_OOM;
        return CArrUI8 { nullptr, 0 };
    }
    *errCode = NWebError::NO_ERROR;
    return CArrUI8 { result, webDownloadValue.length() };
}

int64_t FfiOHOSWebDownloadItemImplDeserialize(CArrUI8 serializedData, int32_t* errCode)
{
    char* buffer = reinterpret_cast<char*>(serializedData.head);
    browser_service::WebDownload webDownloadPb;
    bool result = webDownloadPb.ParseFromArray(buffer, serializedData.size);
    if (!result) {
        *errCode = NWebError::INIT_ERROR;
        return -1;
    }
    WebDownloadItemImpl* webDownloadItem = FFIData::Create<WebDownloadItemImpl>();
    if (webDownloadItem == nullptr) {
        WEBVIEWLOGE("new web download item failed");
        *errCode = NWebError::INIT_ERROR;
        return -1;
    }
    webDownloadItem->webDownloadId = webDownloadPb.web_download_id();
    webDownloadItem->currentSpeed = webDownloadPb.current_speed();
    webDownloadItem->percentComplete = webDownloadPb.percent_complete();
    webDownloadItem->totalBytes = webDownloadPb.total_bytes();
    webDownloadItem->receivedBytes = webDownloadPb.received_bytes();
    webDownloadItem->guid = webDownloadPb.guid();
    webDownloadItem->fullPath = webDownloadPb.full_path();
    webDownloadItem->url = webDownloadPb.url();
    webDownloadItem->etag = webDownloadPb.etag();
    webDownloadItem->originalUrl = webDownloadPb.original_url();
    webDownloadItem->suggestedFileName = webDownloadPb.suggested_file_name();
    webDownloadItem->contentDisposition = webDownloadPb.content_disposition();
    webDownloadItem->mimeType = webDownloadPb.mime_type();
    webDownloadItem->lastModified = webDownloadPb.last_modified();
    webDownloadItem->state = static_cast<NWebDownloadItemState>(webDownloadPb.state());
    webDownloadItem->method = webDownloadPb.method();
    webDownloadItem->lastErrorCode = webDownloadPb.last_error_code();
    webDownloadItem->receivedSlices = webDownloadPb.received_slices();
    webDownloadItem->downloadPath = webDownloadPb.download_path();
    *errCode = NWebError::NO_ERROR;
    return webDownloadItem->GetID();
}

// WebDownloadDelegateImpl
int64_t FfiOHOSWebDownloadDelegateImplConstructor()
{
    auto nativeWebDownloadDelegateImpl = FFIData::Create<WebDownloadDelegateImpl>();
    if (nativeWebDownloadDelegateImpl == nullptr) {
        WEBVIEWLOGE("new web download delegate failed");
        return -1;
    }
    return nativeWebDownloadDelegateImpl->GetID();
}

void FfiOHOSWebDownloadDelegateImplOnBeforeDownload(int64_t id, void (*callback)(int64_t))
{
    auto nativeWebDownloadDelegateImpl = FFIData::GetData<WebDownloadDelegateImpl>(id);
    if (!nativeWebDownloadDelegateImpl) {
        WEBVIEWLOGE("[DOWNLOAD] webDownloadDelegate is null");
        return;
    }
    nativeWebDownloadDelegateImpl->PutDownloadBeforeStart(CJLambda::Create(callback));
}

void FfiOHOSWebDownloadDelegateImplOnDownloadUpdated(int64_t id, void (*callback)(int64_t))
{
    auto nativeWebDownloadDelegateImpl = FFIData::GetData<WebDownloadDelegateImpl>(id);
    if (!nativeWebDownloadDelegateImpl) {
        WEBVIEWLOGE("[DOWNLOAD] webDownloadDelegate is null");
        return;
    }
    nativeWebDownloadDelegateImpl->PutDownloadDidUpdate(CJLambda::Create(callback));
}

void FfiOHOSWebDownloadDelegateImplOnDownloadFinish(int64_t id, void (*callback)(int64_t))
{
    auto nativeWebDownloadDelegateImpl = FFIData::GetData<WebDownloadDelegateImpl>(id);
    if (!nativeWebDownloadDelegateImpl) {
        WEBVIEWLOGE("[DOWNLOAD] webDownloadDelegate is null");
        return;
    }
    nativeWebDownloadDelegateImpl->PutDownloadDidFinish(CJLambda::Create(callback));
}

void FfiOHOSWebDownloadDelegateImplOnDownloadFailed(int64_t id, void (*callback)(int64_t))
{
    auto nativeWebDownloadDelegateImpl = FFIData::GetData<WebDownloadDelegateImpl>(id);
    if (!nativeWebDownloadDelegateImpl) {
        WEBVIEWLOGE("[DOWNLOAD] webDownloadDelegate is null");
        return;
    }
    nativeWebDownloadDelegateImpl->PutDownloadDidFail(CJLambda::Create(callback));
}

// WebDownloadManagerImpl
void FfiOHOSWebDownloadManagerImplSetDownloadDelegate(int64_t delegateId)
{
    auto nativeWebDownloadDelegateImpl = FFIData::GetData<WebDownloadDelegateImpl>(delegateId);
    if (!nativeWebDownloadDelegateImpl) {
        WEBVIEWLOGE("[DOWNLOAD] webDownloadDelegate is null");
        return;
    }
    WebDownloadManagerImpl::SetDownloadDelegate(nativeWebDownloadDelegateImpl);
}

int32_t FfiOHOSWebDownloadManagerImplResumeDownload(int64_t itemId)
{
    if (!WebDownloadManagerImpl::HasValidDelegate()) {
        return NWebError::NO_DOWNLOAD_DELEGATE_SET;
    }
    auto nativeWebDownloadItemImpl = FFIData::GetData<WebDownloadItemImpl>(itemId);
    if (!nativeWebDownloadItemImpl) {
        WEBVIEWLOGE("[DOWNLOAD] webDownloadDelegate is null");
        return NWebError::INIT_ERROR;
    }
    WebDownloadManagerImpl::ResumeDownload(nativeWebDownloadItemImpl);
    return NWebError::NO_ERROR;
}
}
} // namespace Webview
} // namespace OHOS