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

#ifndef WEB_DOWNLOAD_DELEGATE_IMPL_H
#define WEB_DOWNLOAD_DELEGATE_IMPL_H

#include "ffi_remote_data.h"
#include "web_download_item_impl.h"
#include "web_errors.h"

namespace OHOS::Webview {
    class __attribute__((visibility("default"))) WebDownloadDelegateImpl : public OHOS::FFI::FFIData {
        DECL_TYPE(WebDownloadDelegateImpl, OHOS::FFI::FFIData)
    public:
        explicit WebDownloadDelegateImpl();
        ~WebDownloadDelegateImpl();

        void DownloadBeforeStart(WebDownloadItemImpl *webDownloadItemImpl);
        void DownloadDidUpdate(WebDownloadItemImpl *webDownloadItemImpl);
        void DownloadDidFail(WebDownloadItemImpl *webDownloadItemImpl);
        void DownloadDidFinish(WebDownloadItemImpl *webDownloadItemImpl);

        void PutDownloadBeforeStart(std::function<void(int64_t)> callback);
        void PutDownloadDidUpdate(std::function<void(int64_t)> callback);
        void PutDownloadDidFinish(std::function<void(int64_t)> callback);
        void PutDownloadDidFail(std::function<void(int64_t)> callback);

        int32_t GetNWebId();
        void SetNWebId(int32_t nwebId);

    private:
        int32_t nwebId_ = -1;

        std::function<void(int64_t)> download_before_start_callback_;
        std::function<void(int64_t)> download_did_update_callback_;
        std::function<void(int64_t)> download_did_finish_callback_;
        std::function<void(int64_t)> download_did_fail_callback_;
    };
}
#endif // WEB_DOWNLOAD_DELEGATE_IMPL_H