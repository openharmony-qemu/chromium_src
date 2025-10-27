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

#ifndef WEB_STORAGE_H
#define WEB_STORAGE_H

#include <string>
#include "webview_utils.h"

namespace OHOS {
namespace NWeb {

class WebStorage {
public:
    WebStorage() {}
    ~WebStorage() = default;
    static int32_t CJdeleteOrigin(const std::string &origin);
    static void CJdeleteAllData(bool incognito = false);
    static int64_t CjGetOriginUsageOrQuota(const std::string &origin, int32_t *errCode, bool isQuata);
    static Webview::CArrWebStorageOrigin CjGetOrigins(int32_t *errCode);
};
}
}

#endif