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

#ifndef NWEB_NAPI_WEB_ADSBLOCK_MANAGER_H
#define NWEB_NAPI_WEB_ADSBLOCK_MANAGER_H

#include <cstddef>
#include <iosfwd>
#include <string>
#include <uv.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nweb_value_callback.h"

namespace OHOS {
namespace NWeb {
const std::string WEB_ADSBLOCK_MANAGER_CLASS_NAME = "AdsBlockManager";

class NapiWebAdsBlockManager {
public:
    NapiWebAdsBlockManager() {}

    ~NapiWebAdsBlockManager() = default;

    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static napi_value JsSetAdsBlockRules(napi_env env, napi_callback_info info);

    static napi_value JsAddAdsBlockDisallowedList(napi_env env, napi_callback_info info);

    static napi_value JsRemoveAdsBlockDisallowedList(napi_env env, napi_callback_info info);

    static napi_value JsClearAdsBlockDisallowedList(napi_env env, napi_callback_info info);

    static napi_value JsAddAdsBlockAllowedList(napi_env env, napi_callback_info info);

    static napi_value JsRemoveAdsBlockAllowedList(napi_env env, napi_callback_info info);

    static napi_value JsClearAdsBlockAllowedList(napi_env env, napi_callback_info info);
};

} // namespace NWeb
} // namespace OHOS

#endif // NWEB_NAPI_WEB_ADSBLOCK_MANAGER_H