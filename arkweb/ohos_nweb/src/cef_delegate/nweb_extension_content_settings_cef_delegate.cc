/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "nweb_extension_content_settings_cef_delegate.h"

#include <atomic>
#include <map>
#include <mutex>

#include "base/logging.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "content/public/browser/browser_context.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_content_settings_dispatcher.h"
#endif

namespace OHOS::NWeb {

namespace {

static std::map<int, ContentSettingsGetCallback> g_content_settings_get_callback_map_;
std::mutex g_content_settings_get_callback_map_mutex;

static std::map<int, ContentSettingsSetCallback> g_content_settings_set_callback_map_;
std::mutex g_content_settings_set_callback_map_mutex;

static std::map<int, ContentSettingsClearCallback> g_content_settings_clear_callback_map_;
std::mutex g_content_settings_clear_callback_map_mutex;

content::BrowserContext *GetBrowserContext()
{
    CefRequestContextImpl *request_context =
        static_cast<CefRequestContextImpl *>(CefRequestContext::GetGlobalContext().get());
    if (!request_context) {
        LOG(ERROR) << "request_context is null";
        return nullptr;
    }
    CefBrowserContext *cef_browser_context = request_context->GetBrowserContext();
    if (!cef_browser_context) {
        LOG(ERROR) << "cef_browser_context is null";
        return nullptr;
    }      
    return cef_browser_context->AsBrowserContext();
}

}  // namespace

// static
NWebExtensionContentSettingsCefDelegate& NWebExtensionContentSettingsCefDelegate::GetInstance()
{
    static NWebExtensionContentSettingsCefDelegate instance;
    return instance;
}

bool NWebExtensionContentSettingsCefDelegate::HasOnGet() {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
    return false;
#else
    return NWebExtensionContentSettingsDispatcher::GetInstance().HasOnGet();
#endif
}

bool NWebExtensionContentSettingsCefDelegate::HasOnSet() {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
    return false;
#else
    return NWebExtensionContentSettingsDispatcher::GetInstance().HasOnSet();
#endif
}

bool NWebExtensionContentSettingsCefDelegate::HasOnClear() {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
    return false;
#else
    return NWebExtensionContentSettingsDispatcher::GetInstance().HasOnClear();
#endif
}

bool NWebExtensionContentSettingsCefDelegate::OnGet(
    const NWebExtensionContentSettingsGetParam* getParam, ContentSettingsGetCallback callback) 
{
    LOG(INFO) << "OnGet NWebExtensionContentSettingsCefDelegate is call";
#if !BUILDFLAG(ARKWEB_NWEB_EX)
    return false;
#else
    static std::atomic<int> requestId(0);
    int currentRequestId;
    {
        std::lock_guard<std::mutex> lock(g_content_settings_get_callback_map_mutex);
        currentRequestId = ++requestId;
        g_content_settings_get_callback_map_[currentRequestId] = std::move(callback);
    }
    bool result = NWebExtensionContentSettingsDispatcher::GetInstance().OnGet(currentRequestId, getParam);
    if (!result) {
        std::lock_guard<std::mutex> lock(g_content_settings_get_callback_map_mutex);
        g_content_settings_get_callback_map_.erase(currentRequestId);
    }
    return result;
#endif
}

void NWebExtensionContentSettingsCefDelegate::OnGetCallback(const NWebExtensionContentSettingsCallbackResult* result)
{
    LOG(INFO) << "NWebExtensionContentSettingsCefDelegate::OnGetCallback";
    if (!result) {
        LOG(ERROR) << "OnGetCallback result is null";
        return;
    }

    if (result->detailParam != nullptr && result->detailParam->contentSetting != nullptr) {
        LOG(INFO) << "NWebExtensionContentSettingsCefDelegate contentSetting: "
                  << std::string(result->detailParam->contentSetting);
    }

    ContentSettingsGetCallback callback;
    {
        std::lock_guard<std::mutex> lock(g_content_settings_get_callback_map_mutex);
        auto it = g_content_settings_get_callback_map_.find(result->requestId);
        if (it == g_content_settings_get_callback_map_.end()) {
            LOG(ERROR) << "OnGetCallback requestId not found: " << result->requestId;
            return;
        }

        callback = std::move(it->second);
        g_content_settings_get_callback_map_.erase(it);
    }
    LOG(INFO) << "ContentSettingsGetCallback end";
    std::move(callback).Run(result->detailParam, result->error);
}

bool NWebExtensionContentSettingsCefDelegate::OnSet(
    const NWebExtensionContentSettingsSetParam* setParam, ContentSettingsSetCallback callback) 
{
    LOG(INFO) << "OnSet NWebExtensionContentSettingsCefDelegate is call";
#if !BUILDFLAG(ARKWEB_NWEB_EX)
    return false;
#else
    static std::atomic<int> requestId(0);
    int currentRequestId;
    {
        std::lock_guard<std::mutex> lock(g_content_settings_set_callback_map_mutex);
        currentRequestId = ++requestId;
        g_content_settings_set_callback_map_[currentRequestId] = std::move(callback);
    }
    bool result = NWebExtensionContentSettingsDispatcher::GetInstance().OnSet(currentRequestId, setParam);
    if (!result) {
        std::lock_guard<std::mutex> lock(g_content_settings_set_callback_map_mutex);
        g_content_settings_set_callback_map_.erase(currentRequestId);
    }
    return result;
#endif
}

void NWebExtensionContentSettingsCefDelegate::OnSetCallback(const NWebExtensionContentSettingsCallbackSetResult* result)
{
    if (!result) {
        LOG(ERROR) << "OnSetCallback result is null";
        return;
    }
    ContentSettingsSetCallback callback;
    {
        std::lock_guard<std::mutex> lock(g_content_settings_set_callback_map_mutex);
        auto it = g_content_settings_set_callback_map_.find(result->requestId);
        if (it == g_content_settings_set_callback_map_.end()) {
            return;
        }

        callback = std::move(it->second);
        g_content_settings_set_callback_map_.erase(it);
    }
    std::move(callback).Run(result->error);
}

bool NWebExtensionContentSettingsCefDelegate::OnClear(
    const NWebExtensionContentSettingsClearParam* clearParam, ContentSettingsClearCallback callback) 
{
    LOG(INFO) << "OnClear NWebExtensionContentSettingsCefDelegate is call";
#if !BUILDFLAG(ARKWEB_NWEB_EX)
    return false;
#else
    static std::atomic<int> requestId(0);
    int currentRequestId;
    {
        std::lock_guard<std::mutex> lock(g_content_settings_clear_callback_map_mutex);
        currentRequestId = ++requestId;
        g_content_settings_clear_callback_map_[currentRequestId] = std::move(callback);
    }
    bool result = NWebExtensionContentSettingsDispatcher::GetInstance().OnClear(currentRequestId, clearParam);
    if (!result) {
        std::lock_guard<std::mutex> lock(g_content_settings_clear_callback_map_mutex);
        g_content_settings_clear_callback_map_.erase(currentRequestId);
    }
    return result;
#endif
}

void NWebExtensionContentSettingsCefDelegate::OnClearCallback(
    const NWebExtensionContentSettingsCallbackSetResult* result)
{
    if (!result) {
        LOG(ERROR) << "OnClearCallback result is null";
        return;
    }

    ContentSettingsClearCallback callback;
    {
        std::lock_guard<std::mutex> lock(g_content_settings_clear_callback_map_mutex);
        auto it = g_content_settings_clear_callback_map_.find(result->requestId);
        if (it == g_content_settings_clear_callback_map_.end()) {
            return;
        }

        callback = std::move(it->second);
        g_content_settings_clear_callback_map_.erase(it);
    }
    std::move(callback).Run(result->error);
}

}  // namespace OHOS::NWeb