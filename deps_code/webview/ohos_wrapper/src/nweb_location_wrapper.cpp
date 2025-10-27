/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "nweb_export.h"
#include "nweb_log.h"

#include "locator.h"

using namespace OHOS;
using namespace OHOS::Location;
namespace OHOS::NWeb {
auto g_locatorProxy = Locator::GetInstance();
}

extern "C" OHOS_NWEB_EXPORT bool IsLocationEnable(bool& isEnabled)
{
    if (!NWeb::g_locatorProxy) {
        WVLOG_E("g_locatorProxy is nullptr");
        return false;
    }
    LocationErrCode ret = NWeb::g_locatorProxy->IsLocationEnabledV9(isEnabled);
    if (ret != LocationErrCode::ERRCODE_SUCCESS) {
        WVLOG_E("StartLocating failed, errcode:%{public}d", ret);
        return false;
    }
    return true;
}

extern "C" OHOS_NWEB_EXPORT bool StartLocating(
    std::unique_ptr<RequestConfig>& requestConfig,
    OHOS::sptr<ILocatorCallback>& callback)
{
    if (!NWeb::g_locatorProxy || !requestConfig || !callback) {
        WVLOG_E("g_locatorProxy is nullptr");
        return false;
    }
    LocationErrCode ret = NWeb::g_locatorProxy->StartLocatingV9(
        requestConfig, callback);
    if (ret != LocationErrCode::ERRCODE_SUCCESS) {
        WVLOG_E("StartLocating failed, errcode:%{public}d", ret);
        return false;
    }
    return true;
}

extern "C" OHOS_NWEB_EXPORT bool StopLocating(OHOS::sptr<ILocatorCallback>& callback)
{
    if (!NWeb::g_locatorProxy || !callback) {
        WVLOG_E("g_locatorProxy is nullptr");
        return false;
    }
    LocationErrCode ret = NWeb::g_locatorProxy->StopLocatingV9(callback);
    if (ret != LocationErrCode::ERRCODE_SUCCESS) {
        WVLOG_E("StopLocating failed, errcode:%{public}d", ret);
        return false;
    }
    return true;
}

extern "C" OHOS_NWEB_EXPORT bool EnableAbility(bool enable)
{
    if (!NWeb::g_locatorProxy) {
        WVLOG_E("g_locatorProxy is nullptr");
        return false;
    }
    LocationErrCode ret = NWeb::g_locatorProxy->EnableAbilityV9(enable);
    if (ret != LocationErrCode::ERRCODE_SUCCESS) {
        WVLOG_E("StopLocating failed, errcode:%{public}d", ret);
        return false;
    }
    return true;
}