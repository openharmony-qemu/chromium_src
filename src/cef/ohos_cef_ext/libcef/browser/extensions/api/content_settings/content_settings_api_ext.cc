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

#include <set>
#include <string>
#include <map>
#include <utility>

#include "chrome/browser/extensions/api/content_settings/content_settings_api.h"
#include "extensions/browser/extension_function.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_content_settings_types.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_content_settings_cef_delegate.h"

#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/metrics/histogram_macros.h"
#include "base/strings/stringprintf.h"
#include "base/values.h"
#include "chrome/browser/content_settings/cookie_settings_factory.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/extensions/api/content_settings.h"
#include "components/content_settings/core/browser/content_settings_info.h"
#include "components/content_settings/core/browser/content_settings_registry.h"
#include "components/content_settings/core/browser/content_settings_uma_util.h"
#include "components/content_settings/core/browser/content_settings_utils.h"
#include "components/content_settings/core/browser/cookie_settings.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/webplugininfo.h"
#include "extensions/common/extension_id.h"
#include "extensions/browser/api/content_settings/content_settings_helpers.h"
#include "extensions/browser/api/content_settings/content_settings_service.h"
#include "extensions/browser/api/content_settings/content_settings_store.h"
#include "extensions/browser/extension_util.h"
#include "extensions/common/api/extension_types.h"
#include "extensions/common/constants.h"
#include "extensions/common/error_utils.h"
#include "net/cookies/site_for_cookies.h"

using content::BrowserThread;

namespace Clear = extensions::api::content_settings::ContentSetting::Clear;
namespace Get = extensions::api::content_settings::ContentSetting::Get;
namespace Set = extensions::api::content_settings::ContentSetting::Set;

namespace {

using extensions::api::types::ChromeSettingScope;

bool RemoveContentType(base::Value::List& args, ContentSettingsType* content_type) 
{
    if (args.empty() || !args[0].is_string()) {
        return false;
    }

    // Not a ref since we remove the underlying value after.
    std::string content_type_str = args[0].GetString();
    LOG(INFO) << "RemoveContentType" << args[0].GetString();

    // We remove the ContentSettingsType parameter since this is added by the
    // renderer, and is not part of the JSON schema.
    args.erase(args.begin());
    *content_type = extensions::content_settings_helpers::StringToContentSettingsType(content_type_str);
    return *content_type != ContentSettingsType::DEFAULT;
}

// Errors.
constexpr char kIncognitoContextError[] = "Can't modify regular settings from an incognito context.";
constexpr char kIncognitoSessionOnlyError[] = "You cannot read incognito content settings when no incognito window "
                                              "is open.";
constexpr char kInvalidUrlError[] = "The URL \"*\" is invalid.";
}  // namespace

namespace extensions {

#define NWEB_CONTENTSETTINGS_SAFE_FREE(ptr) \
    do {                                \
        if (ptr) {                        \
            free(ptr);                      \
            ptr = nullptr;                  \
        }                                 \
    } while (0)

void NWebExtensionContentSettingsGetParamRelease(NWebExtensionContentSettingsGetParam* param) 
{
    if (!param) {
        return;
    }
    if (param->incognito) {
        delete (param->incognito);
    }
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->primaryUrl);
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->secondaryUrl);
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->type);
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->extensionId);
}

void NWebExtensionContentSettingsSetParamRelease(NWebExtensionContentSettingsSetParam* param) 
{
    if (!param) {
        return;
    }
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->primaryPattern);
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->scope);
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->secondaryPattern);
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->contentSetting);
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->type);
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->extensionId);
}

void NWebExtensionContentSettingsClearParamRelease(NWebExtensionContentSettingsClearParam* param) 
{
    if (!param) {
        return;
    }
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->scope);
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->type);
    NWEB_CONTENTSETTINGS_SAFE_FREE(param->extensionId);
}

ContentSetting StringToContentSetting(const std::string &str)
{
    if (str == "allow") {
        return ContentSetting::CONTENT_SETTING_ALLOW;
    } else if (str == "block") {
        return ContentSetting::CONTENT_SETTING_BLOCK;
    } else if (str == "ask") {
        return ContentSetting::CONTENT_SETTING_ASK;
    } else {
        return ContentSetting::CONTENT_SETTING_DEFAULT;
    }
}

std::string ChromeSettingScopeToString(ChromeSettingScope scope)
{
    static const std::map<ChromeSettingScope, std::string> scopeMap = {{ChromeSettingScope::kNone, "None"},
        {ChromeSettingScope::kRegular, "Regular"},
        {ChromeSettingScope::kRegularOnly, "RegularOnly"},
        {ChromeSettingScope::kIncognitoPersistent, "IncognitoPersistent"},
        {ChromeSettingScope::kIncognitoSessionOnly, "IncognitoSessionOnly"}};

    auto it = scopeMap.find(scope);
    if (it != scopeMap.end()) {
        return it->second;
    }
    return "Unknown";
}

bool SetExtensionIdToParam(NWebExtensionContentSettingsGetParam* param, const std::string &extension_id)
{
    if (extension_id.empty()) {
        return true;
    }

    size_t ext_len = extension_id.length();
    param->extensionId = static_cast<char*>(calloc(ext_len + 1, sizeof(char)));
    if (!param->extensionId) {
        return false;
    }

    if (strcpy_s(param->extensionId, ext_len + 1, extension_id.c_str()) != 0) {
        return false;
    }
    return true;
}

bool SetTypeToParam(NWebExtensionContentSettingsGetParam* param, ContentSettingsType content_type)
{
    std::string typeStr = content_settings_helpers::ContentSettingsTypeToString(content_type);
    size_t len = typeStr.length();

    param->type = static_cast<char*>(calloc(len + 1, sizeof(char)));
    if (!param->type) {
        return false;
    }

    if (strcpy_s(param->type, len + 1, typeStr.c_str()) != 0) {
        return false;
    }
    return true;
}

bool SetTypeToSetParam(NWebExtensionContentSettingsSetParam* param, ContentSettingsType content_type)
{
    std::string typeStr = content_settings_helpers::ContentSettingsTypeToString(content_type);
    size_t len = typeStr.length();

    param->type = static_cast<char*>(calloc(len + 1, sizeof(char)));
    if (!param->type) {
        return false;
    }

    if (strcpy_s(param->type, len + 1, typeStr.c_str()) != 0) {
        return false;
    }
    return true;
}

bool SetScopeToSetParam(NWebExtensionContentSettingsSetParam* param, ChromeSettingScope scope)
{
    std::string scopeStr = ChromeSettingScopeToString(scope);
    size_t len = scopeStr.length();

    param->scope = static_cast<char*>(calloc(len + 1, sizeof(char)));
    if (!param->scope) {
        return false;
    }

    if (strcpy_s(param->scope, len + 1, scopeStr.c_str()) != 0) {
        return false;
    }
    return true;
}

bool SetExtensionIdToSetParam(NWebExtensionContentSettingsSetParam* param, const std::string &extension_id)
{
    if (extension_id.empty()) {
        return true;
    }

    size_t ext_len = extension_id.length();
    param->extensionId = static_cast<char*>(calloc(ext_len + 1, sizeof(char)));
    if (!param->extensionId) {
        return false;
    }
    
    if (strcpy_s(param->extensionId, ext_len + 1, extension_id.c_str()) != 0) {
        return false;
    }
    return true;
}

bool SetTypeToClearParam(NWebExtensionContentSettingsClearParam* param, ContentSettingsType content_type)
{
    std::string typeStr = content_settings_helpers::ContentSettingsTypeToString(content_type);
    size_t len = typeStr.length();

    param->type = static_cast<char*>(calloc(len + 1, sizeof(char)));
    if (!param->type) {
        return false;
    }

    if (strcpy_s(param->type, len + 1, typeStr.c_str()) != 0) {
        return false;
    }
    return true;
}

bool SetScopeToClearParam(NWebExtensionContentSettingsClearParam* param, ChromeSettingScope scope)
{
    std::string scopeStr = ChromeSettingScopeToString(scope);
    size_t len = scopeStr.length();

    param->scope = static_cast<char*>(calloc(len + 1, sizeof(char)));
    if (!param->scope) {
        return false;
    }

    if (strcpy_s(param->scope, len + 1, scopeStr.c_str()) != 0) {
        return false;
    }
    return true;
}

bool SetExtensionIdToClearParam(NWebExtensionContentSettingsClearParam* param, const std::string &extension_id)
{
    if (extension_id.empty()) {
        return true;
    }

    size_t ext_len = extension_id.length();
    param->extensionId = static_cast<char*>(calloc(ext_len + 1, sizeof(char)));
    if (!param->extensionId) {
        return false;
    }
    
    if (strcpy_s(param->extensionId, ext_len + 1, extension_id.c_str()) != 0) {
        return false;
    }
    return true;
}

// Note: If returns false, the caller MUST call
// NWebExtensionBookmarksGetParamRelease to free any partially allocated memory
// in getParam
bool ContentSettingsBuildGetParams(
    const std::optional<extensions::api::content_settings::ContentSetting::Get::Params>& params,
    NWebExtensionContentSettingsGetParam* getParam)
{
    if (!params) {
        return false;
    }
    LOG(INFO) << "ContentSettingsBuildGetParams get parameters";

    if (!params->details.primary_url.empty()) {
        getParam->primaryUrl = strdup(params->details.primary_url.c_str());
        if (!getParam->primaryUrl) {
            return false;
        }
    }

    if (params->details.secondary_url) {
        getParam->secondaryUrl = strdup(params->details.secondary_url->c_str());
        if (!getParam->secondaryUrl) {
            return false;
        }
    }

    if (params->details.incognito.has_value()) {
        getParam->incognito = new bool(*params->details.incognito);
        if (getParam->incognito == nullptr) {
            return false;
        }
    } else {
        getParam->incognito = nullptr;
    }  
    return true;
}

bool ContentSettingsBuildSetParams(
    const std::optional<extensions::api::content_settings::ContentSetting::Set::Params>& params,
    NWebExtensionContentSettingsSetParam* setParam)
{
    LOG(INFO) << "ContentSettingsBuildSetParams set parameters";
    if (!params) {
        return false;
    }  

    if (!params->details.primary_pattern.empty()) {
        setParam->primaryPattern = strdup(params->details.primary_pattern.c_str());
        if (!setParam->primaryPattern) {
            return false;
        }
    }

    if (!params->details.setting.GetString().empty()) {
        const std::string &settingStr = params->details.setting.GetString();
        setParam->contentSetting = strdup(settingStr.c_str());
        if (!setParam->contentSetting) {
            return false;
        }
    }

    if (params->details.secondary_pattern) {
        setParam->secondaryPattern = strdup(params->details.secondary_pattern->c_str());
        if (!setParam->secondaryPattern) {
            return false;
        }
    }
    return true;
}

ExtensionFunction::ResponseAction ContentSettingsContentSettingGetFunction::Run() 
{
    LOG(INFO) << "ContentSettingsContentSettingGetFunction run";

    ContentSettingsType content_type;
    EXTENSION_FUNCTION_VALIDATE(RemoveContentType(mutable_args(), &content_type));
    std::string extension_id = this->extension_id();

    std::optional<Get::Params> params = Get::Params::Create(args());
    EXTENSION_FUNCTION_VALIDATE(params);

    if (content_type == ContentSettingsType::DEPRECATED_PPAPI_BROKER) {
        return RespondNow(Error("failed to get contentSettings type::DEPRECATED_PPAPI_BROKER for get"));
    }

    GURL primary_url(params->details.primary_url);
    if (!primary_url.is_valid()) {
        return RespondNow(Error(kInvalidUrlError, params->details.primary_url));
    }

    GURL secondary_url(primary_url);
    if (params->details.secondary_url) {
        secondary_url = GURL(*params->details.secondary_url);
        if (!secondary_url.is_valid()) {
            return RespondNow(Error(kInvalidUrlError, *params->details.secondary_url));
        }
    }

    bool incognito = false;
    if (params->details.incognito)
        incognito = *params->details.incognito;
    if (incognito && !include_incognito_information())
        return RespondNow(Error(extension_misc::kIncognitoErrorMessage));

    HostContentSettingsMap *map;
    scoped_refptr<content_settings::CookieSettings> cookie_settings;
    Profile *profile = Profile::FromBrowserContext(browser_context());
    if (!profile) {
        LOG(ERROR)<<"Failed to get profile from browser context";
        return RespondNow(Error("Failed to get profile"));
    }
    if (incognito) {
        if (!profile->HasPrimaryOTRProfile()) {
            // TODO(bauerb): Allow reading incognito content settings
            // outside of an incognito session.
            return RespondNow(Error(kIncognitoSessionOnlyError));
        }
        map = HostContentSettingsMapFactory::GetForProfile(profile->GetPrimaryOTRProfile(/*create_if_needed=*/true));
        cookie_settings = 
            CookieSettingsFactory::GetForProfile(profile->GetPrimaryOTRProfile(/*create_if_needed=*/true));
    } else {
        map = HostContentSettingsMapFactory::GetForProfile(profile);
        cookie_settings = CookieSettingsFactory::GetForProfile(profile);
    }

    NWebExtensionContentSettingsGetParam getParam = {0};

    if (!SetTypeToParam(&getParam, content_type) || !SetExtensionIdToParam(&getParam, extension_id) ||
        !ContentSettingsBuildGetParams(params, &getParam)) {
        NWebExtensionContentSettingsGetParamRelease(&getParam);
        LOG(INFO) << "ContentSettingsGetFunction failed to build get parameters";
        return RespondNow(BadMessage());
      }

    call_get_content_settings_ = true;
    bool success = OHOS::NWeb::NWebExtensionContentSettingsCefDelegate::GetInstance().OnGet(&getParam,
        base::BindRepeating(&ContentSettingsContentSettingGetFunction::GetCallback, weak_ptr_factory_.GetWeakPtr()));
    call_get_content_settings_ = false;
    NWebExtensionContentSettingsGetParamRelease(&getParam);
    if (did_respond()) {
        LOG(INFO) << "ContentSettingsContentSettingGetFunction did_respond";
        return AlreadyResponded();
    }

    if (success) {
        AddRef();
        LOG(INFO) << "ContentSettingsContentSettingGetFunction AddRef";
        return RespondLater();
    } else {
        // TODO(crbug.com/40247160): Consider whether the following check should
        // somehow determine real CookieSettingOverrides rather than default to none.
        net::SiteForCookies site_for_cookies = net::SiteForCookies::FromUrl(secondary_url);
        site_for_cookies.CompareWithFrameTreeSiteAndRevise(net::SchemefulSite(primary_url));
        ContentSetting setting =
            content_type == ContentSettingsType::COOKIES
                ? cookie_settings->GetCookieSetting(
                      primary_url, site_for_cookies, secondary_url, net::CookieSettingOverrides(), nullptr)
                : map->GetContentSetting(primary_url, secondary_url, content_type);

        base::Value::Dict result;
        std::string setting_string = content_settings::ContentSettingToString(setting);
        DCHECK(!setting_string.empty());
        result.Set(ContentSettingsStore::kContentSettingKey, setting_string);

        return RespondNow(WithArguments(std::move(result)));
    }
}

void ContentSettingsContentSettingGetFunction::GetCallback(
    const base::WeakPtr<ContentSettingsContentSettingGetFunction>& function,
    const NWebExtensionContentSettingsDetail *detailParam, const char* error)
{
    LOG(INFO) << "ContentSettingsContentSettingGetFunction::GetCallback";
    DCHECK(function);
    if (!function) {
        LOG(ERROR) << "ContentSettingsGetFunction is empty!!!!";
        return;
    }

    if (!detailParam || error || !detailParam->contentSetting) {
        std::string errorMessage = error ? error : "get error";
        LOG(INFO) << "ContentSettingsContentSettingGetFunction::GetCallback param " << errorMessage;
        function->Respond(function->Error(errorMessage));
    } else {
        LOG(INFO) << "ContentSettingsContentSettingGetFunction::GetCallback param " << detailParam->contentSetting;
        base::Value::Dict result;
        std::string contentSettingStr(detailParam->contentSetting);
        ContentSetting setting = StringToContentSetting(contentSettingStr);
        std::string setting_string = content_settings::ContentSettingToString(setting);
        DCHECK(!setting_string.empty());
        result.Set(ContentSettingsStore::kContentSettingKey, setting_string);
        function->Respond(function->WithArguments(std::move(result)));
    }

    if (!function->call_get_content_settings_) {
        LOG(INFO) << "ContentSettingsGetFunction Release";
        function->Release();
    }
}

ExtensionFunction::ResponseAction ContentSettingsContentSettingSetFunction::Run() 
{
    LOG(INFO) << "ContentSettingsContentSettingSetFunction run";
    ContentSettingsType content_type;
    EXTENSION_FUNCTION_VALIDATE(RemoveContentType(mutable_args(), &content_type));
    std::optional<Set::Params> params = Set::Params::Create(args());
    EXTENSION_FUNCTION_VALIDATE(params);
    if (content_type == ContentSettingsType::DEPRECATED_PPAPI_BROKER) {
        return RespondNow(Error("failed to get contentSettings type::DEPRECATED_PPAPI_BROKER for set"));
    }

    std::string primary_error;
    ContentSettingsPattern primary_pattern =
        content_settings_helpers::ParseExtensionPattern(params->details.primary_pattern, &primary_error);
    if (!primary_pattern.IsValid())
        return RespondNow(Error(primary_error));  

    ContentSettingsPattern secondary_pattern = ContentSettingsPattern::Wildcard();
    if (params->details.secondary_pattern) {
        std::string secondary_error;
        secondary_pattern = 
            content_settings_helpers::ParseExtensionPattern(*params->details.secondary_pattern, &secondary_error);
        if (!secondary_pattern.IsValid())
            return RespondNow(Error(secondary_error));
    }

    EXTENSION_FUNCTION_VALIDATE(params->details.setting.is_string());
    std::string setting_str = params->details.setting.GetString();
    ContentSetting setting;
    EXTENSION_FUNCTION_VALIDATE(content_settings::ContentSettingFromString(setting_str, &setting));
    // The content settings extensions API does not support setting any content
    // settings to |CONTENT_SETTING_DEFAULT|.
    EXTENSION_FUNCTION_VALIDATE(CONTENT_SETTING_DEFAULT != setting);
    EXTENSION_FUNCTION_VALIDATE(
        content_settings::ContentSettingsRegistry::GetInstance()->Get(content_type)->IsSettingValid(setting));

    const content_settings::ContentSettingsInfo* info =
        content_settings::ContentSettingsRegistry::GetInstance()->Get(content_type);
    if (!info) {
        LOG(ERROR) << "Failed to get ContentSettingsInfo for content type: " << content_type;
        return RespondNow(Error("Invalid content type"));
    }

    // The ANTI_ABUSE content setting does not support site-specific settings.
    if (content_type == ContentSettingsType::ANTI_ABUSE &&
        (primary_pattern != ContentSettingsPattern::Wildcard() ||
            secondary_pattern != ContentSettingsPattern::Wildcard())) {
        return RespondNow(Error("Site-specific settings are not allowed for this type. The URL "
                                "pattern must be '<all_urls>'."));
    }

    // Some content setting types support the full set of values listed in
    // content_settings.json only for exceptions. For the default setting,
    // some values might not be supported.
    // For example, camera supports [allow, ask, block] for exceptions, but only
    // [ask, block] for the default setting.
    if (primary_pattern == ContentSettingsPattern::Wildcard() &&
        secondary_pattern == ContentSettingsPattern::Wildcard() && !info->IsDefaultSettingValid(setting)) {
        static const char kUnsupportedDefaultSettingError[] = "'%s' is not supported as the default setting of %s.";

        // TODO(msramek): Get the same human readable name as is presented
        // externally in the API, i.e. chrome.contentSettings.<name>.set().
        std::string readable_type_name;
        if (content_type == ContentSettingsType::MEDIASTREAM_MIC) {
            readable_type_name = "microphone";
        } else if (content_type == ContentSettingsType::MEDIASTREAM_CAMERA) {
            readable_type_name = "camera";
        } else {
            NOTREACHED() << "No human-readable type name defined for this type.";
        }

        return RespondNow(Error(
            base::StringPrintf(kUnsupportedDefaultSettingError, setting_str.c_str(), readable_type_name.c_str())));
    }

    if (primary_pattern != secondary_pattern && secondary_pattern != ContentSettingsPattern::Wildcard()) {
        content_settings_uma_util::RecordContentSettingsHistogram(
            "ContentSettings.ExtensionEmbeddedSettingSet", content_type);
    } else {
        content_settings_uma_util::RecordContentSettingsHistogram(
            "ContentSettings.ExtensionNonEmbeddedSettingSet", content_type);
    }

    if (primary_pattern != secondary_pattern && secondary_pattern != ContentSettingsPattern::Wildcard() &&
        !info->website_settings_info()->SupportsSecondaryPattern()) {
        static const char kUnsupportedEmbeddedException[] = "Embedded patterns are not supported for this setting.";
        return RespondNow(Error(kUnsupportedEmbeddedException));
    }

    ChromeSettingScope scope = ChromeSettingScope::kRegular;
    bool incognito = false;
    if (params->details.scope == api::content_settings::Scope::kIncognitoSessionOnly) {
        scope = ChromeSettingScope::kIncognitoSessionOnly;
        incognito = true;
    }

    if (incognito) {
        // Regular profiles can't access incognito unless the extension is allowed
        // to run in incognito contexts.
        if (!browser_context()->IsOffTheRecord() &&
            !extensions::util::IsIncognitoEnabled(extension_id(), browser_context())) {
            return RespondNow(Error(extension_misc::kIncognitoErrorMessage));
        }
    } else {
        // Incognito profiles can't access regular mode ever, they only exist in
        // split mode.
        if (browser_context()->IsOffTheRecord())
            return RespondNow(Error(kIncognitoContextError));
    }

    if (scope == ChromeSettingScope::kIncognitoSessionOnly &&
        !Profile::FromBrowserContext(browser_context())->HasPrimaryOTRProfile()) {
        return RespondNow(Error(extension_misc::kIncognitoSessionOnlyErrorMessage));
    }

    NWebExtensionContentSettingsSetParam setParam = {0};
    std::string extension_id = this->extension_id();

    if (!SetTypeToSetParam(&setParam, content_type) || !SetScopeToSetParam(&setParam, scope) ||
        !SetExtensionIdToSetParam(&setParam, extension_id) || !ContentSettingsBuildSetParams(params, &setParam)) {
        NWebExtensionContentSettingsSetParamRelease(&setParam);
        LOG(INFO)<<"ContentSettingsContentSettingSetFunction failed to build get parameters";
        return RespondNow(BadMessage());
    }

    call_set_content_settings_ = true;
    bool success = OHOS::NWeb::NWebExtensionContentSettingsCefDelegate::GetInstance().OnSet(&setParam,
        base::BindRepeating(&ContentSettingsContentSettingSetFunction::SetCallback, weak_ptr_factory_.GetWeakPtr()));
    call_set_content_settings_ = false;
    NWebExtensionContentSettingsSetParamRelease(&setParam);

    if (success) {
        AddRef();
        LOG(INFO) << "ContentSettingsContentSettingSetFunction AddRef";
        return RespondLater();
    } else {
        scoped_refptr<ContentSettingsStore> store =
            ContentSettingsService::Get(browser_context())->content_settings_store();
        store->SetExtensionContentSetting(
            extension_id, primary_pattern, secondary_pattern, content_type, setting, scope);

        return RespondNow(NoArguments());
    }
}

void ContentSettingsContentSettingSetFunction::SetCallback(
    const base::WeakPtr<ContentSettingsContentSettingSetFunction>& function, const char* error)
{
    LOG(INFO) << "ContentSettingsContentSetting SetCallback";
    DCHECK(function);
    if (!function) {
        LOG(ERROR) << "ContentSettingsContentSettingSetFunction is empty!!!!";
        return;
    }

    if (error) {
        LOG(INFO) << "ContentSettingsContentSetting SetCallback error " << error;
        std::string errorMessage = error ? error : "get error";
        function->Respond(function->Error(errorMessage));
    } else {
        LOG(INFO) << "ContentSettingsContentSetting no error";
        function->Respond(function->NoArguments());
    }

    if (!function->call_set_content_settings_) {
        LOG(INFO) << "ContentSettingsContentSettingSetFunction Release";
        function->Release();
    }
}

ExtensionFunction::ResponseAction ContentSettingsContentSettingClearFunction::Run() 
{
    LOG(INFO) << "ContentSettingsContentSettingClearFunction run";
    ContentSettingsType content_type;
    EXTENSION_FUNCTION_VALIDATE(RemoveContentType(mutable_args(), &content_type));

    std::optional<Clear::Params> params = Clear::Params::Create(args());
    EXTENSION_FUNCTION_VALIDATE(params);

    if (content_type == ContentSettingsType::DEPRECATED_PPAPI_BROKER) {
        return RespondNow(Error("failed to get contentSettings type::DEPRECATED_PPAPI_BROKER for clear"));
    }

    ChromeSettingScope scope = ChromeSettingScope::kRegular;
    bool incognito = false;
    if (params->details.scope == api::content_settings::Scope::kIncognitoSessionOnly) {
        scope = ChromeSettingScope::kIncognitoSessionOnly;
        incognito = true;
    }

    if (incognito) {
        // We don't check incognito permissions here, as an extension should be
        // always allowed to clear its own settings.
    } else if (browser_context()->IsOffTheRecord()) {
        // Incognito profiles can't access regular mode ever, they only exist in
        // split mode.
        return RespondNow(Error(kIncognitoContextError));
    }

    if (!params) {
       return RespondNow(BadMessage());
    }

    NWebExtensionContentSettingsClearParam clearParam = {0};
    std::string extension_id = this->extension_id();

    if (!SetTypeToClearParam(&clearParam, content_type) || !SetScopeToClearParam(&clearParam, scope) ||
        !SetExtensionIdToClearParam(&clearParam, extension_id)) {
        NWebExtensionContentSettingsClearParamRelease(&clearParam);
        LOG(INFO) << "ContentSettingsContentSettingClearFunction failed to build clear parameters";
        return RespondNow(BadMessage());
      }

    call_clear_content_settings_ = true;
    bool success = OHOS::NWeb::NWebExtensionContentSettingsCefDelegate::GetInstance().OnClear(&clearParam,
        base::BindRepeating(
            &ContentSettingsContentSettingClearFunction::ClearCallback, weak_ptr_factory_.GetWeakPtr()));
    call_clear_content_settings_ = false;
    NWebExtensionContentSettingsClearParamRelease(&clearParam);

    if (success) {
        AddRef();
        LOG(INFO) << "ContentSettingsContentSettingClearFunction AddRef";
        return RespondLater();
    } else {
        scoped_refptr<ContentSettingsStore> store =
            ContentSettingsService::Get(browser_context())->content_settings_store();
        store->ClearContentSettingsForExtensionAndContentType(extension_id, scope, content_type);

        return RespondNow(NoArguments());
    }
}

void ContentSettingsContentSettingClearFunction::ClearCallback(
    const base::WeakPtr<ContentSettingsContentSettingClearFunction>& function,const char* error)
{
    LOG(INFO) << "ContentSettingsContentSettingClearFunction ClearCallback run";
    DCHECK(function);
    if (!function) {
        LOG(ERROR) << "ContentSettingsContentSettingClearFunction is empty!!!!";
        return;
    }

    if (error) {
        std::string errorMessage = error ? error : "get error";
        LOG(INFO) << "ContentSettingsContentSetting ClearCallback error" << errorMessage;
        function->Respond(function->Error(errorMessage));
    } else {
        LOG(INFO) << "ContentSettingsContentSetting ClearCallback no error";
        function->Respond(function->NoArguments());
    }

    if (!function->call_clear_content_settings_) {
        LOG(INFO) << "ContentSettingsContentSettingClearFunction Release";
        function->Release();
    }
}

}  // namespace extensions
