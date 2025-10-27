/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "napi_web_download_manager.h"

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>
#include <securec.h>
#include <cstring>

#include "business_error.h"
#include "nweb_c_api.h"
#include "nweb_log.h"
#include "web_download_delegate.h"
#include "web_download_manager.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;

// static
napi_value NapiWebDownloadManager::JS_SetDownloadDelegate(napi_env env, napi_callback_info info)
{
    WVLOG_D("[DOWNLOAD] NapiWebDownloadManager::JS_SetDownloadDelegate");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    napi_value obj = argv[0];
    // check download delegate is object type
    napi_valuetype objType = napi_undefined;
    napi_typeof(env, argv[0], &objType);

    WebDownloadDelegate *delegate = nullptr;
    napi_unwrap(env, obj, (void **)&delegate);
    if (!delegate) {
        WVLOG_E("[DOWNLOAD] NapiWebDownloadManager::JS_SetDownloadDelegate delegate is null");
        return nullptr;
    }
    napi_create_reference(env, obj, 1, &delegate->delegate_);
    WebDownloadManager::SetDownloadDelegate(delegate);
    return nullptr;
}

// static
napi_value NapiWebDownloadManager::JS_ResumeDownload(napi_env env, napi_callback_info info)
{
    WVLOG_D("[DOWNLOAD] NapiWebDownloadManager::JS_ResumeDownload");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    if (!WebDownloadManager::HasValidDelegate()) {
        BusinessError::ThrowErrorByErrcode(env, NO_DOWNLOAD_DELEGATE_SET);
        return nullptr;
    }

    // check web download is object type
    napi_valuetype objType = napi_undefined;
    napi_typeof(env, argv[0], &objType);

    WebDownloadItem *webDownloadItem = nullptr;
    napi_status status = napi_unwrap(env, argv[0], (void **)&webDownloadItem);
    if (status != napi_ok || webDownloadItem == nullptr) {
        WVLOG_E("[DOWNLOAD] unwrap webDownloadItem failed.");
    }

    WebDownloadManager::ResumeDownload(webDownloadItem);
    return nullptr;
}

napi_value NapiWebDownloadManager::JS_Constructor(napi_env env, napi_callback_info info)
{
    WVLOG_D("[DOWNLOAD] NapiWebDownloadManager::JS_Constructor");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    return thisVar;
}

napi_value NapiWebDownloadManager::Init(napi_env env, napi_value exports)
{
    WVLOG_D("[DOWNLOAD] NapiWebDownloadManager::Init");
    napi_property_descriptor properties[] = {
        { "setDownloadDelegate", nullptr, JS_SetDownloadDelegate, nullptr, nullptr, nullptr, napi_static, nullptr },
        { "resumeDownload", nullptr, JS_ResumeDownload, nullptr, nullptr, nullptr, napi_static, nullptr },
    };
    napi_value webDownloadManagerClass = nullptr;
    napi_define_class(env, WEB_DOWNLOAD_MANAGER.c_str(), WEB_DOWNLOAD_MANAGER.length(), JS_Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &webDownloadManagerClass);
    napi_set_named_property(env, exports, WEB_DOWNLOAD_MANAGER.c_str(), webDownloadManagerClass);

    return exports;
}
} // namespace NWeb
} // namespace OHOS
