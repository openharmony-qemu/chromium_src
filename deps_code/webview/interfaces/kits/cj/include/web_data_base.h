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

#ifndef WEB_DATA_BASE_H
#define WEB_DATA_BASE_H

#include <string>
#include <cj_ffi/cj_common_ffi.h>

namespace OHOS {
namespace NWeb {

const int MAX_STRING_LENGTH = 40960;
const int MAX_PWD_LENGTH = 256;
class WebDataBase {
public:
    WebDataBase() {}
    ~WebDataBase() = default;

    static CArrString CJGetHttpAuthCredentials(const std::string &host, const std::string &realm);
    static void CJSaveHttpAuthCredentials(const std::string &host, const std::string &realm,
        const std::string &username, const std::string &password);
    static bool CJExistHttpAuthCredentials();
    static void CJDeleteHttpAuthCredentials();
};
}
}

#endif