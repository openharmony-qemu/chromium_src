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

#include "webview_adsblock_manager.h"

#include "nweb_helper.h"
#include "web_errors.h"
#include "webview_utils.h"
#include "webview_log.h"
#include "nweb_adsblock_manager.h"

using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {
    constexpr int MAX_URL_RULES_FILEPATH_LENGTH = 255;

    int32_t AdsBlockManagerImpl::SetAdsBlockRules(const char *rulesFile, bool replace)
    {
        if (rulesFile == nullptr) {
            WEBVIEWLOGE("AdsBlockManagerImpl::SetAdsBlockRules failed: rulesFile is null.");
            return NWebError::PARAM_CHECK_ERROR;
        }
        std::string rulesFileStr(rulesFile);
        if (rulesFileStr.length() > MAX_URL_RULES_FILEPATH_LENGTH) {
            WEBVIEWLOGE("AdsBlockManagerImpl::SetAdsBlockRules failed: rulesFile path too long");
            return NWebError::PARAM_CHECK_ERROR;
        }
        std::shared_ptr<NWebAdsBlockManager> adsBlockManager = NWebHelper::Instance().GetAdsBlockManager();
        if (adsBlockManager != nullptr) {
            adsBlockManager->SetAdsBlockRules(rulesFile, replace);
        }
        return NWebError::NO_ERROR;
    }

    int32_t AdsBlockManagerImpl::AddAdsBlockDisallowedList(CArrString domainSuffixes)
    {
        if (domainSuffixes.size <= 0 || domainSuffixes.head == nullptr) {
            WEBVIEWLOGE("AdsBlockManagerImpl::AddAdsBlockDisallowedList failed: domainSuffixes is null.");
            return NWebError::PARAM_CHECK_ERROR;
        }
        std::shared_ptr<NWebAdsBlockManager> adsBlockManager = NWebHelper::Instance().GetAdsBlockManager();
        if (adsBlockManager != nullptr) {
            adsBlockManager->AddAdsBlockDisallowedList(OHOS::Webview::CArrStringToVector(domainSuffixes));
        }
        return NWebError::NO_ERROR;
    }

    int32_t AdsBlockManagerImpl::RemoveAdsBlockDisallowedList(CArrString domainSuffixes)
    {
        if (domainSuffixes.size <= 0 || domainSuffixes.head == nullptr) {
            WEBVIEWLOGE("AdsBlockManagerImpl::RemoveAdsBlockDisallowedList failed: domainSuffixes is null.");
            return NWebError::PARAM_CHECK_ERROR;
        }
        std::shared_ptr<NWebAdsBlockManager> adsBlockManager = NWebHelper::Instance().GetAdsBlockManager();
        if (adsBlockManager != nullptr) {
            adsBlockManager->RemoveAdsBlockDisallowedList(OHOS::Webview::CArrStringToVector(domainSuffixes));
        }
        return NWebError::NO_ERROR;
    }

    int32_t AdsBlockManagerImpl::AddAdsBlockAllowedList(CArrString domainSuffixes)
    {
        if (domainSuffixes.size <= 0 || domainSuffixes.head == nullptr) {
            WEBVIEWLOGE("AdsBlockManagerImpl::AddAdsBlockAllowedList failed: domainSuffixes is null.");
            return NWebError::PARAM_CHECK_ERROR;
        }
        std::shared_ptr<NWebAdsBlockManager> adsBlockManager = NWebHelper::Instance().GetAdsBlockManager();
        if (adsBlockManager != nullptr) {
            adsBlockManager->AddAdsBlockAllowedList(OHOS::Webview::CArrStringToVector(domainSuffixes));
        }
        return NWebError::NO_ERROR;
    }

    int32_t AdsBlockManagerImpl::RemoveAdsBlockAllowedList(CArrString domainSuffixes)
    {
        if (domainSuffixes.size <= 0 || domainSuffixes.head == nullptr) {
            WEBVIEWLOGE("AdsBlockManagerImpl::RemoveAdsBlockAllowedList failed: domainSuffixes is null.");
            return NWebError::PARAM_CHECK_ERROR;
        }
        std::shared_ptr<NWebAdsBlockManager> adsBlockManager = NWebHelper::Instance().GetAdsBlockManager();
        if (adsBlockManager != nullptr) {
            adsBlockManager->RemoveAdsBlockAllowedList(OHOS::Webview::CArrStringToVector(domainSuffixes));
        }
        return NWebError::NO_ERROR;
    }

    void AdsBlockManagerImpl::ClearAdsBlockAllowedList()
    {
        std::shared_ptr<NWebAdsBlockManager> adsBlockManager = NWebHelper::Instance().GetAdsBlockManager();
        if (adsBlockManager != nullptr) {
            adsBlockManager->ClearAdsBlockAllowedList();
        }
    }

    void AdsBlockManagerImpl::ClearAdsBlockDisallowedList()
    {
        std::shared_ptr<NWebAdsBlockManager> adsBlockManager = NWebHelper::Instance().GetAdsBlockManager();
        if (adsBlockManager != nullptr) {
            adsBlockManager->ClearAdsBlockDisallowedList();
        }
    }
}
}