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

#include "web_data_base.h"
#include "nweb_data_base.h"
#include "nweb_helper.h"
#include "web_errors.h"
#include "webview_utils.h"
#include "webview_log.h"
#include "securec.h"
#include <cstring>

namespace OHOS {
namespace NWeb {
const int DEFAULT_AUTH_LENGTH = 2;
const int HTTP_AUTH_INIT_LENGTH = -1;

CArrString WebDataBase::CJGetHttpAuthCredentials(const std::string &host, const std::string &realm)
{
    CArrString ret = {nullptr, HTTP_AUTH_INIT_LENGTH};
    std::string username_s;
    char password[MAX_PWD_LENGTH + 1] = {0};
    std::shared_ptr<NWebDataBase> database = NWebHelper::Instance().GetDataBase();
    if (database != nullptr) {
        database->GetHttpAuthCredentials(host, realm, username_s, password, MAX_PWD_LENGTH + 1);
    }

    if (username_s.empty() || strlen(password) == 0) {
        ret.size = 0;
        return ret;
    }

    char** result = static_cast<char**>(malloc(sizeof(char*) * DEFAULT_AUTH_LENGTH));
    if (result == nullptr) {
        WEBVIEWLOGI("Webdatabase getHttpAuthCredentials malloc result failed!");
        (void)memset_s(password, MAX_PWD_LENGTH + 1, 0, MAX_PWD_LENGTH + 1);
        return ret;
    }

    result[0] = OHOS::Webview::MallocCString(username_s);
    if (result[0] == nullptr) {
        WEBVIEWLOGI("Webdatabase getHttpAuthCredentials transfer username_s failed!");
        free(result);
        (void)memset_s(password, MAX_PWD_LENGTH + 1, 0, MAX_PWD_LENGTH + 1);
        return ret;
    }

    result[1] = static_cast<char*>(malloc(sizeof(char) * (MAX_PWD_LENGTH + 1)));
    if (result[1] == nullptr) {
        WEBVIEWLOGI("Webdatabase getHttpAuthCredentials malloc password failed!");
        free(result[0]);
        free(result);
        (void)memset_s(password, MAX_PWD_LENGTH + 1, 0, MAX_PWD_LENGTH + 1);
        return ret;
    }
    result[1] = std::char_traits<char>::copy(result[1], password, MAX_PWD_LENGTH);
    (void)memset_s(password, MAX_PWD_LENGTH + 1, 0, MAX_PWD_LENGTH + 1);
    ret.head = result;
    ret.size = DEFAULT_AUTH_LENGTH;
    return ret;
}

void WebDataBase::CJSaveHttpAuthCredentials(const std::string &host, const std::string &realm,
    const std::string &username, const std::string &password)
{
    // get web database instance;
    std::shared_ptr<NWebDataBase> database = NWebHelper::Instance().GetDataBase();
    if (database != nullptr) {
        database->SaveHttpAuthCredentials(host, realm, username, password.c_str());
    }
}

bool WebDataBase::CJExistHttpAuthCredentials()
{
    bool isExist = false;
    // get web database instance;
    std::shared_ptr<NWebDataBase> database = NWebHelper::Instance().GetDataBase();
    if (database != nullptr) {
        isExist = database->ExistHttpAuthCredentials();
    }
    return isExist;
}

void WebDataBase::CJDeleteHttpAuthCredentials()
{
    // get web database instance;
    std::shared_ptr<NWebDataBase> database = NWebHelper::Instance().GetDataBase();
    if (database != nullptr) {
        database->DeleteHttpAuthCredentials();
    }
}
}
}
