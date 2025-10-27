/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARK_WEB_DOWNLOAD_MANAGER_WRAPPER_H_
#define ARK_WEB_DOWNLOAD_MANAGER_WRAPPER_H_
#pragma once

#include "include/nweb_download_manager.h"
#include "ohos_nweb/include/ark_web_download_manager.h"

namespace OHOS::ArkWeb {

class ArkWebDownloadManagerWrapper : public OHOS::NWeb::NWebDownloadManager {
public:
    ArkWebDownloadManagerWrapper(ArkWebRefPtr<ArkWebDownloadManager> ark_web_download_manager);
    ~ArkWebDownloadManagerWrapper() = default;

    void SetHttpDns(std::shared_ptr<OHOS::NWeb::NWebDOHConfig> config) override;

    void SetConnectionTimeout(const int& timeout) override;

private:
    ArkWebRefPtr<ArkWebDownloadManager> ark_web_download_manager_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_DOWNLOAD_MANAGER_WRAPPER_H_
