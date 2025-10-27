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

#include "web_data_base_ffi.h"

#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "web_data_base.h"
#include "web_errors.h"
#include "webview_log.h"
#include "webview_utils.h"

using namespace OHOS::FFI;
using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {
extern "C" {
// web data base;
RetDataCArrString FfiOHOSDBGetHttpAuthCredentials(const char* host, const char* realm)
{
    std::string host_s = std::string(host);
    std::string realm_s = std::string(realm);

    CArrString result = OHOS::NWeb::WebDataBase::CJGetHttpAuthCredentials(host_s, realm_s);
    RetDataCArrString ret;

    if (result.size == -1) {
        ret.code = NWebError::HTTP_AUTH_MALLOC_FAILED;
    } else {
        ret.code = NWebError::NO_ERROR;
    }

    ret.data = result;
    return ret;
}

void FfiOHOSDBSaveHttpAuthCredentials(const char* host, const char* realm, const char* username, const char* password)
{
    std::string host_s = std::string(host);
    std::string realm_s = std::string(realm);
    std::string username_s = std::string(username);
    std::string password_s = std::string(password);

    OHOS::NWeb::WebDataBase::CJSaveHttpAuthCredentials(host_s, realm_s, username_s, password_s);
}

bool FfiOHOSDBExistHttpAuthCredentials()
{
    return OHOS::NWeb::WebDataBase::CJExistHttpAuthCredentials();
}

void FfiOHOSDBDeleteHttpAuthCredentials()
{
    OHOS::NWeb::WebDataBase::CJDeleteHttpAuthCredentials();
}
}
} // namespace Webview
} // namespace OHOS