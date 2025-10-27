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

#include "webview_adsblock_ffi.h"

#include "webview_utils.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "web_errors.h"
#include "webview_log.h"
#include "parameters.h"
#include "webview_adsblock_manager.h"

using namespace OHOS::FFI;
using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {
extern "C" {
    // AdsBlockManager
    int32_t FfiAdsBlockManagerSetAdsBlockRules(const char *rulesFile, bool replace)
    {
        return AdsBlockManagerImpl::SetAdsBlockRules(rulesFile, replace);
    }

    int32_t FfiAdsBlockManagerAddAdsBlockDisallowedList(CArrString domainSuffixes)
    {
        return AdsBlockManagerImpl::AddAdsBlockDisallowedList(domainSuffixes);
    }

    int32_t FfiAdsBlockManagerRemoveAdsBlockDisallowedList(CArrString domainSuffixes)
    {
        return AdsBlockManagerImpl::RemoveAdsBlockDisallowedList(domainSuffixes);
    }

    int32_t FfiAdsBlockManagerAddAdsBlockAllowedList(CArrString domainSuffixes)
    {
        return AdsBlockManagerImpl::AddAdsBlockAllowedList(domainSuffixes);
    }

    int32_t FfiAdsBlockManagerRemoveAdsBlockAllowedList(CArrString domainSuffixes)
    {
        return AdsBlockManagerImpl::RemoveAdsBlockAllowedList(domainSuffixes);
    }

    void FfiAdsBlockManagerClearAdsBlockAllowedList()
    {
        AdsBlockManagerImpl::ClearAdsBlockAllowedList();
    }

    void FfiAdsBlockManagerClearAdsBlockDisallowedList()
    {
        AdsBlockManagerImpl::ClearAdsBlockDisallowedList();
    }
}
}
}