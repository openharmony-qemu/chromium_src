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

#ifndef NWEB_WEB_DOWNLOAD_MANAGER_IMPL_H
#define NWEB_WEB_DOWNLOAD_MANAGER_IMPL_H

#include <string>

#include "web_download_delegate_impl.h"
#include "web_download_item_impl.h"

namespace OHOS::Webview {
    class WebDownloadDelegateImpl;

    class WebDownloadManagerImpl {
    public:
        static void SetDownloadDelegate(WebDownloadDelegateImpl *delegate);
        static void ResumeDownload(const WebDownloadItemImpl *webDownload);

        static void AddDownloadDelegateForWeb(int32_t nwebId, WebDownloadDelegateImpl *delegate);
        static void RemoveDownloadDelegate(WebDownloadDelegateImpl *delegate);

        static bool HasValidDelegate();

        WebDownloadManagerImpl() = default;
        ~WebDownloadManagerImpl() = default;

    private:
        static void RegisterDownloadCallback();
    };
} // namespace OHOS::Webview

#endif // NWEB_WEB_DOWNLOAD_MANAGER_IMPL_H