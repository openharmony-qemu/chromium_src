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

#ifndef NWEB_EXTENSION_CONTENT_SETTINGS_CEF_DELEGATE_H_
#define NWEB_EXTENSION_CONTENT_SETTINGS_CEF_DELEGATE_H_

#include <memory>

#include "base/functional/callback.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_content_settings_types.h"

using ContentSettingsGetCallback =
    base::RepeatingCallback<void(const NWebExtensionContentSettingsDetail* detailParam, const char* error)>;

using ContentSettingsSetCallback =
    base::RepeatingCallback<void(const char* error)>;

using ContentSettingsClearCallback =
    base::RepeatingCallback<void(const char* error)>;

namespace OHOS::NWeb {

class NWebExtensionContentSettingsCefDelegate {
public:
    static NWebExtensionContentSettingsCefDelegate &GetInstance();

    // contentsettings.get
    bool HasOnGet();
    bool OnGet(const NWebExtensionContentSettingsGetParam* getParam, ContentSettingsGetCallback callback);

    void OnGetCallback(const NWebExtensionContentSettingsCallbackResult* result);

    // contentsettings.set
    bool HasOnSet();
    bool OnSet(const NWebExtensionContentSettingsSetParam* setParam, ContentSettingsSetCallback callback);

    void OnSetCallback(const NWebExtensionContentSettingsCallbackSetResult* result);

    // contentsettings.clear
    bool HasOnClear();
    bool OnClear(const NWebExtensionContentSettingsClearParam* setParam, ContentSettingsClearCallback callback);

    void OnClearCallback(const NWebExtensionContentSettingsCallbackSetResult* result);

private:
    NWebExtensionContentSettingsCefDelegate() = default;
    ~NWebExtensionContentSettingsCefDelegate() = default;
    NWebExtensionContentSettingsCefDelegate(const NWebExtensionContentSettingsCefDelegate &) = delete;
    NWebExtensionContentSettingsCefDelegate& operator=(const NWebExtensionContentSettingsCefDelegate &) = delete;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_EXTENSION_CONTENT_SETTINGS_CEF_DELEGATE_H_