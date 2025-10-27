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

#include "web_download_delegate_impl.h"

#include <cstring>

#include "nweb_c_api.h"
#include "webview_log.h"
#include "web_download_manager_impl.h"

namespace OHOS::Webview {
    WebDownloadDelegateImpl::WebDownloadDelegateImpl()
        : download_before_start_callback_(nullptr),
        download_did_update_callback_(nullptr),
        download_did_finish_callback_(nullptr),
        download_did_fail_callback_(nullptr)
    {
        WEBVIEWLOGD("WebDownloadDelegate::WebDownloadDelegate");
    }

    WebDownloadDelegateImpl::~WebDownloadDelegateImpl()
    {
        WEBVIEWLOGI("[DOWNLOAD] WebDownloadDelegate::~WebDownloadDelegate");
        WebDownloadManagerImpl::RemoveDownloadDelegate(this);
    }

    void WebDownloadDelegateImpl::DownloadBeforeStart(WebDownloadItemImpl *webDownloadItemImpl)
    {
        WEBVIEWLOGI("[DOWNLOAD] WebDownloadDelegate::DownloadBeforeStart");
        if (!download_before_start_callback_) {
            WEBVIEWLOGE("[DOWNLOAD] downloadBeforeStart not exists.");
            return;
        }
        download_before_start_callback_(webDownloadItemImpl->GetID());
    }

    void WebDownloadDelegateImpl::DownloadDidUpdate(WebDownloadItemImpl *webDownloadItemImpl)
    {
        WEBVIEWLOGI("[DOWNLOAD] WebDownloadDelegate::DownloadDidUpdate");
        if (!download_did_update_callback_) {
            WEBVIEWLOGE("[DOWNLOAD] downloadBeforeStart not exists.");
            return;
        }
        download_did_update_callback_(webDownloadItemImpl->GetID());
    }

    void WebDownloadDelegateImpl::DownloadDidFail(WebDownloadItemImpl *webDownloadItemImpl)
    {
        WEBVIEWLOGI("WebDownloadDelegate::DownloadDidFail");
        if (!download_did_fail_callback_) {
            WEBVIEWLOGE("[DOWNLOAD] downloadDidFail not exists.");
            return;
        }
        download_did_fail_callback_(webDownloadItemImpl->GetID());
    }

    void WebDownloadDelegateImpl::DownloadDidFinish(WebDownloadItemImpl *webDownloadItemImpl)
    {
        WEBVIEWLOGI("WebDownloadDelegate::DownloadDidFinish");
        if (!download_did_finish_callback_) {
            WEBVIEWLOGE("[DOWNLOAD] downloadBeforeStart not exists.");
            return;
        }
        download_did_finish_callback_(webDownloadItemImpl->GetID());
    }

    void WebDownloadDelegateImpl::PutDownloadBeforeStart(std::function<void(int64_t)> callback)
    {
        WEBVIEWLOGD("WebDownloadDelegate::PutDownloadBeforeStart");
        download_before_start_callback_ = callback;
    }

    void WebDownloadDelegateImpl::PutDownloadDidUpdate(std::function<void(int64_t)> callback)
    {
        WEBVIEWLOGI("[DOWNLOAD] WebDownloadDelegate::PutDownloadDidUpdate");
        download_did_update_callback_ = callback;
    }

    void WebDownloadDelegateImpl::PutDownloadDidFinish(std::function<void(int64_t)> callback)
    {
        WEBVIEWLOGD("WebDownloadDelegate::PutDownloadDidFinish");
        download_did_finish_callback_ = callback;
    }

    void WebDownloadDelegateImpl::PutDownloadDidFail(std::function<void(int64_t)> callback)
    {
        WEBVIEWLOGD("WebDownloadDelegate::PutDownloadDidFail");
        download_did_fail_callback_ = callback;
    }

    int32_t WebDownloadDelegateImpl::GetNWebId()
    {
        return nwebId_;
    }

    void WebDownloadDelegateImpl::SetNWebId(int32_t nwebId)
    {
        nwebId_ = nwebId;
    }

} // namespace OHOS:WebView
