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

#ifndef WEB_DOWNLOAD_ITEM_IMPL_H
#define WEB_DOWNLOAD_ITEM_IMPL_H

#include <string>

#include "ffi_remote_data.h"
#include "nweb_c_api.h"

namespace OHOS::Webview {
    class __attribute__((visibility("default"))) WebDownloadItemImpl : public OHOS::FFI::FFIData {
        DECL_TYPE(WebDownloadItemImpl, OHOS::FFI::FFIData)
    public:
        explicit WebDownloadItemImpl();
        WebDownloadItemImpl(NWebDownloadItem *downloadItem);
        ~WebDownloadItemImpl();

        long webDownloadId;
        int currentSpeed;
        int percentComplete;
        int64_t totalBytes;
        int64_t receivedBytes;
        int lastErrorCode;

        std::string guid;
        std::string fullPath;
        std::string url;
        std::string etag;
        std::string originalUrl;
        std::string suggestedFileName;
        std::string contentDisposition;
        std::string mimeType;
        std::string lastModified;

        NWebDownloadItemState state = NWebDownloadItemState::MAX_DOWNLOAD_STATE;
        std::string method;
        std::string receivedSlices;

        std::string downloadPath;
        WebBeforeDownloadCallbackWrapper *before_download_callback;
        WebDownloadItemCallbackWrapper *download_item_callback;

        int32_t nwebId;
    };
} // namespace OHOS::Webview

#endif // WEB_DOWNLOAD_ITEM_IMPL_H
