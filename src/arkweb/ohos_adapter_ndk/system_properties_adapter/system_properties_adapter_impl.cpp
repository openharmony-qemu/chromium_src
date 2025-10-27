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

#include "system_properties_adapter_impl.h"

#include <sstream>

#include <native_interface_bundle.h>
#include "adapter_base.h"
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include <deviceinfo.h>
#include <AbilityKit/ability_runtime/application_context.h>

namespace OHOS::NWeb {
const std::string FACTORY_CONFIG_VALUE = "factoryConfig";
const std::string FACTORY_LEVEL_VALUE = "factoryLevel";
const std::string FACTORY_LEVEL_TV = "32";
const std::string FACTORY_LEVEL_WATCH = "16";
const std::string FACTORY_LEVEL_PC = "8";
const std::string FACTORY_LEVEL_TABLET = "4";
const std::string FACTORY_LEVEL_PHONE = "2";
const std::string FACTORY_LEVEL_DEFAULT = "1";

const std::string PROP_RENDER_DUMP = "web.render.dump";
const std::string PROP_DEBUG_TRACE = "web.debug.trace";
const int MAX_SIZE_OF_MAP = 2; // Currently, only 2 instructions need to be registered
const int VERSION_SIZE = 4;
const int DOT_SIZE = 3;
const int ARRAY_INDEX_0 = 0;
const int ARRAY_INDEX_1 = 1;
const int ARRAY_INDEX_2 = 2;
const int ARRAY_INDEX_3 = 3;
const std::unordered_map<std::string, PropertiesKey> PROP_KEY_MAP = {
    {PROP_RENDER_DUMP, PropertiesKey::PROP_RENDER_DUMP},
    {PROP_DEBUG_TRACE, PropertiesKey::PROP_DEBUG_TRACE}};

void SystemPropertiesChangeCallback(void *context, const OH_PreferencesPair *pairs, uint32_t count)
{
    for (int i = 0; i < count; i++) {
        const char* key = OH_PreferencesPair_GetKey(pairs, i);
        if (key == nullptr) {
            WVLOG_E("sys prop change, key is nullptr");
            continue;
        }
        const OH_PreferencesValue *object = OH_PreferencesPair_GetPreferencesValue(pairs, i);
        if (object == nullptr) {
            WVLOG_E("sys prop change, failed to get preferences value");
            continue;
        }
        Preference_ValueType type = OH_PreferencesValue_GetValueType(object);
        WVLOG_D("sys prop change, key: %{public}s, type: %{public}d ", key,  type);
        if (type == PREFERENCE_TYPE_STRING) {
            char* value = nullptr;
            uint32_t valueLen;
            int ret = OH_PreferencesValue_GetString(object, &value, &valueLen);
            if (ret != PREFERENCES_OK) {
                WVLOG_E("failed to get preferences string");
                continue;
            }
            WVLOG_D("sys prop change key: %{public}s ,value : %{public}s ", key,  value);
            SystemPropertiesAdapterImpl::GetInstance().DispatchAllWatcherInfo(key, value);
        }
    }
}

// static
SystemPropertiesAdapterImpl& SystemPropertiesAdapterImpl::GetInstance()
{
    static SystemPropertiesAdapterImpl instance;
    return instance;
}

SystemPropertiesAdapterImpl::SystemPropertiesAdapterImpl()
{
    InitPreferences();
    AddAllSysPropWatchers();
    std::string osFullName = OH_GetOSFullName();
    if (osFullName.empty()) {
        WVLOG_E("get os full name failed");
        return;
    }
    std::string baseOsNameTmp;
    std::string substring = osFullName;
    size_t index = osFullName.find('-');
    if (index != std::string::npos) {
        baseOsNameTmp = osFullName.substr(0, index);
        substring = osFullName.substr(index + 1);
    }
    int versions[VERSION_SIZE];
    char dots[DOT_SIZE];
    std::istringstream iss(substring);
    iss >> versions[ARRAY_INDEX_0] >> dots[ARRAY_INDEX_0] >> versions[ARRAY_INDEX_1]
        >> dots[ARRAY_INDEX_1] >> versions[ARRAY_INDEX_2] >> dots[ARRAY_INDEX_2] >> versions[ARRAY_INDEX_3];

    if (iss && dots[ARRAY_INDEX_0] == '.' && dots[ARRAY_INDEX_1] == '.' && dots[ARRAY_INDEX_2] == '.') {
        softwareMajorVersion_ = versions[ARRAY_INDEX_0];
        softwareSeniorVersion_ = versions[ARRAY_INDEX_1];
        baseOsName_ = baseOsNameTmp;
    } else {
        WVLOG_E("paser os full name failed");
    }
}

SystemPropertiesAdapterImpl::~SystemPropertiesAdapterImpl()
{
    RemoveAllSysPropWatchers();
    if (preferences_ != nullptr) {
        OH_Preferences_Close(preferences_);
    }

    if (preferencesOption_ != nullptr) {
        OH_PreferencesOption_Destroy(preferencesOption_);
    }
}

void SystemPropertiesAdapterImpl::InitPreferences()
{
    constexpr int bundleSize = 129; // bundle Name max size 128, + 1 = 129
    char bundleName[bundleSize] = { 0 };
    int32_t bundeleLength = 0;
    AbilityRuntime_ErrorCode code =
        OH_AbilityRuntime_ApplicationContextGetBundleName(bundleName, bundleSize, &bundeleLength);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("failed to get BundleName");
        return;
    }

    preferencesOption_ = OH_PreferencesOption_Create();
    if (preferencesOption_ == nullptr) {
        WVLOG_E("failed to create preferences option");
        return;
    }
    int ret = OH_PreferencesOption_SetFileName(preferencesOption_, "perferences");
    if (ret != PREFERENCES_OK) {
        WVLOG_E("failed to set file path");
        return;
    }
    ret = OH_PreferencesOption_SetBundleName(preferencesOption_, bundleName);
    if (ret != PREFERENCES_OK) {
        WVLOG_E("failed to set bundle name");
        return;
    }

    int errCode = 0;
    preferences_ = OH_Preferences_Open(preferencesOption_, &errCode);
    if (errCode != PREFERENCES_OK) {
        preferences_ = nullptr;
        WVLOG_E("failed to open preferences");
        return;
    }
    WVLOG_D("open preferences, bundle name %{public}s", bundleName);
    // If necessary, initialize the configuration here.
}

bool SystemPropertiesAdapterImpl::GetResourceUseHapPathEnable()
{
    return GetBoolParameter("compress", false);
}

std::string SystemPropertiesAdapterImpl::GetDeviceInfoProductModel()
{
    return OH_GetProductModel();
}

std::string SystemPropertiesAdapterImpl::GetDeviceInfoBrand()
{
    return OH_GetBrand();
}

int32_t SystemPropertiesAdapterImpl::GetDeviceInfoMajorVersion()
{
    return softwareMajorVersion_;
}

ProductDeviceType SystemPropertiesAdapterImpl::GetProductDeviceType()
{
    ProductDeviceType factoryLevel = AnalysisFromConfig();
    if (factoryLevel != ProductDeviceType::DEVICE_TYPE_UNKNOWN) {
        return factoryLevel;
    }
    WVLOG_W("read config factoryLevel: fail");
    // RK or other device cant read config，need read from system deviceType
    std::string deviceType = OH_GetDeviceType();
    if (deviceType == "phone" || deviceType == "default") {
        return ProductDeviceType::DEVICE_TYPE_MOBILE;
    } else if (deviceType == "tablet") {
        return ProductDeviceType::DEVICE_TYPE_TABLET;
    } else if (deviceType == "2in1") {
        return ProductDeviceType::DEVICE_TYPE_2IN1;
    } else if (deviceType == "wearable") {
        return ProductDeviceType::DEVICE_TYPE_WEARABLE;
    } else if (deviceType == "tv") {
        return ProductDeviceType::DEVICE_TYPE_TV;
    }
    return ProductDeviceType::DEVICE_TYPE_UNKNOWN;
}

ProductDeviceType SystemPropertiesAdapterImpl::AnalysisFromConfig()
{
#ifdef WEBVIEW_ONLY
    std::string factoryLevel = NWebConfigHelper::Instance()
        .ParsePerfConfig(FACTORY_CONFIG_VALUE, FACTORY_LEVEL_VALUE);
    if (factoryLevel.empty()) {
        NWebConfigHelper::Instance().ReadConfigIfNeeded();
        factoryLevel = NWebConfigHelper::Instance().
            ParsePerfConfig(FACTORY_CONFIG_VALUE, FACTORY_LEVEL_VALUE);
    }
    WVLOG_D("read config factoryLevel: %{public}s ", factoryLevel.c_str());
    if (factoryLevel == FACTORY_LEVEL_PHONE || factoryLevel == FACTORY_LEVEL_DEFAULT) {
        return ProductDeviceType::DEVICE_TYPE_MOBILE;
    } else if (factoryLevel == FACTORY_LEVEL_TABLET) {
        return ProductDeviceType::DEVICE_TYPE_TABLET;
    } else if (factoryLevel == FACTORY_LEVEL_PC) {
        return ProductDeviceType::DEVICE_TYPE_2IN1;
    } else if (factoryLevel == FACTORY_LEVEL_WATCH) {
        return ProductDeviceType::DEVICE_TYPE_WEARABLE;
    } else if (factoryLevel == FACTORY_LEVEL_TV) {
        return ProductDeviceType::DEVICE_TYPE_TV;
    }
    return ProductDeviceType::DEVICE_TYPE_UNKNOWN;
#else
    return ProductDeviceType::DEVICE_TYPE_MOBILE;
#endif
}

bool SystemPropertiesAdapterImpl::GetWebOptimizationValue()
{
    return GetBoolParameter("web.optimization", true);
}

bool SystemPropertiesAdapterImpl::IsAdvancedSecurityMode()
{
    // Only open to system applications, return false
    return false;
}

std::string SystemPropertiesAdapterImpl::GetUserAgentOSName()
{
    return OH_GetDistributionOSName();
}

std::string SystemPropertiesAdapterImpl::GetUserAgentOSVersion()
{
    return GetStringParameter("const.product.os.dist.apiname", "").empty() ?
        GetStringParameter("const.product.os.dist.version", "") :
        GetStringParameter("const.product.os.dist.apiname", "");
}

std::string SystemPropertiesAdapterImpl::GetUserAgentBaseOSName()
{
    return baseOsName_;
}

int32_t SystemPropertiesAdapterImpl::GetSoftwareMajorVersion()
{
    return softwareMajorVersion_;
}

int32_t SystemPropertiesAdapterImpl::GetSoftwareSeniorVersion()
{
    return softwareSeniorVersion_;
}

std::string SystemPropertiesAdapterImpl::GetNetlogMode()
{
    return GetStringParameter("web.debug.netlog", "");
}

bool SystemPropertiesAdapterImpl::GetTraceDebugEnable()
{
    return GetBoolParameter("web.debug.trace", false);
}

std::string SystemPropertiesAdapterImpl::GetSiteIsolationMode()
{
    return GetStringParameter("web.debug.strictsiteIsolation.enable", "");
}

int32_t SystemPropertiesAdapterImpl::GetFlowBufMaxFd()
{
    return GetIntParameter("web.flowbuffer.maxfd", -1);
}

bool SystemPropertiesAdapterImpl::GetOOPGPUEnable()
{
    if (GetDeviceInfoProductModel() == "emulator") {
        return false;
    }
    if (GetStringParameter("web.oop.gpu", "") == "true") {
        return true;
    }

    return false;
}

std::string SystemPropertiesAdapterImpl::GetOOPGPUStatus()
{
    if (GetDeviceInfoProductModel() == "emulator") {
        return "false";
    }
    return GetStringParameter("web.oop.gpu", "");
}

void SystemPropertiesAdapterImpl::SetOOPGPUDisable()
{
    if (GetStringParameter("web.oop.gpu", "") == "None") {
        SetStringParameter("web.oop.gpu", "false");
    }
    return;
}

void SystemPropertiesAdapterImpl::AddAllSysPropWatchers()
{
    if (preferences_ == nullptr) {
        WVLOG_E("preferences is null");
        return;
    }
    int num = PROP_KEY_MAP.size();
    if (num > MAX_SIZE_OF_MAP) {
        WVLOG_E("The size of the map is abnormal");
        return;
    }
    const char **keys = const_cast<const char **>(new char *[num]);
    int i = 0;
    for (auto &item : PROP_KEY_MAP) {
        keys[i] = item.first.c_str();
        i++;
    }
    int ret = OH_Preferences_RegisterDataObserver(preferences_, nullptr,
        SystemPropertiesChangeCallback, keys, num);
    delete[] keys;
    if (ret != PREFERENCES_OK) {
        WVLOG_E("add watch error result: %{public}d", ret);
        return;
    }

    for (auto &item : PROP_KEY_MAP) {
        sysPropObserver_[item.second];
        sysPropMutex_[item.second];
    }
}

void SystemPropertiesAdapterImpl::RemoveAllSysPropWatchers()
{
    if (preferences_ == nullptr) {
        WVLOG_E("preferences is null");
        return;
    }

    int num = PROP_KEY_MAP.size();
    if (num > MAX_SIZE_OF_MAP) {
        WVLOG_E("The size of the map is abnormal");
        return;
    }
    const char **keys = const_cast<const char **>(new char *[num]);
    int i = 0;
    for (auto &item : PROP_KEY_MAP) {
        keys[i] = item.first.c_str();
        i++;
    }
    int ret = OH_Preferences_UnregisterDataObserver(preferences_, nullptr,
        SystemPropertiesChangeCallback, keys, num);
    delete[] keys;
    if (ret != PREFERENCES_OK) {
        WVLOG_E("remove watch error result: %{public}d", ret);
        return;
    }

    for (auto &item : PROP_KEY_MAP) {
        sysPropObserver_[item.second];
        sysPropMutex_[item.second];
    }
}

void SystemPropertiesAdapterImpl::DispatchAllWatcherInfo(const char* key, const char* value)
{
    auto propKeyIt = PROP_KEY_MAP.find(key);
    if (propKeyIt == PROP_KEY_MAP.end()) {
        WVLOG_E("sys prop change key is invalid: %{public}s", key);
        return;
    }

    PropertiesKey propkey = propKeyIt->second;
    auto& keyObservers = sysPropObserver_[propkey];

    if (keyObservers.size() == 0) {
        WVLOG_E("no observers in this key: %{public}s", key);
        return;
    }

    std::shared_lock lock(sysPropMutex_[propkey]);
    for (auto &item : keyObservers) {
        item->PropertiesUpdate(value);
    }
}

void SystemPropertiesAdapterImpl::AttachSysPropObserver(PropertiesKey key, SystemPropertiesObserver* observer)
{
    auto observerIt = sysPropObserver_.find(key);
    if (observerIt == sysPropObserver_.end()) {
        WVLOG_E("properties key invalid in attach");
        return;
    }

    if (observer == nullptr) {
        WVLOG_E("properties key observer invalid in attach");
        return;
    }

    std::vector<SystemPropertiesObserver*>& observerVec = observerIt->second;
    std::unique_lock lock(sysPropMutex_[key]);
    observerVec.push_back(observer);
}

void SystemPropertiesAdapterImpl::DetachSysPropObserver(PropertiesKey key, SystemPropertiesObserver* observer)
{
    auto observerIt = sysPropObserver_.find(key);
    if (observerIt == sysPropObserver_.end()) {
        WVLOG_E("properties key invalid in detach");
        return;
    }

    if (observer == nullptr) {
        WVLOG_E("properties key observer invalid in detach");
        return;
    }

    std::vector<SystemPropertiesObserver*>& observerVec = observerIt->second;
    std::unique_lock lock(sysPropMutex_[key]);

    auto it = std::find(observerVec.begin(), observerVec.end(), observer);
    if (it != observerVec.end()) {
        observerVec.erase(it);
    }
}

bool SystemPropertiesAdapterImpl::GetBoolParameter(const std::string& key, bool defaultValue)
{
    return GetBoolParameter(key.c_str(), defaultValue);
}

bool SystemPropertiesAdapterImpl::GetBoolParameter(const char *key, bool defaultValue)
{
    if (preferences_ == nullptr) {
        WVLOG_E("preferences is null");
        return defaultValue;
    }

    if (key == nullptr) {
        WVLOG_E("key is nullptr");
        return defaultValue;
    }

    bool value = defaultValue;
    int ret = OH_Preferences_GetBool(preferences_, key, &value);
    if (ret == PREFERENCES_ERROR_KEY_NOT_FOUND) {
        WVLOG_I("the key does not exist, key %{public}s", key);
    } else if (ret != PREFERENCES_OK) {
        WVLOG_E("failed to get bool, ret %{public}d", ret);
    }

    WVLOG_D("get bool param, key:%{public}s, value:%{public}d", key, value);
    return value;
}

int SystemPropertiesAdapterImpl::GetIntParameter(const char *key, int defaultValue)
{
    if (preferences_ == nullptr) {
        WVLOG_E("preferences is null");
        return defaultValue;
    }

    if (key == nullptr) {
        WVLOG_E("key is nullptr");
        return defaultValue;
    }

    int value = defaultValue;
    int ret = OH_Preferences_GetInt(preferences_, key, &value);
    if (ret == PREFERENCES_ERROR_KEY_NOT_FOUND) {
        WVLOG_I("the key does not exist");
    } else if (ret != PREFERENCES_OK) {
        WVLOG_E("failed to get int, ret %{public}d", ret);
    }

    WVLOG_D("get int param, key:%{public}s, value:%{public}d", key, value);
    return value;
}

std::string SystemPropertiesAdapterImpl::GetStringParameter(const std::string& key, const std::string& defaultValue)
{
  return GetStringParameter(key.c_str(), defaultValue);
}

std::string SystemPropertiesAdapterImpl::GetStringParameter(const char *key, std::string defaultValue)
{
    if (preferences_ == nullptr) {
        WVLOG_E("preferences is null");
        return defaultValue;
    }

    if (key == nullptr) {
        WVLOG_E("key is nullptr");
        return defaultValue;
    }

    char* value = nullptr;
    uint32_t size = 0;
    int ret = OH_Preferences_GetString(preferences_, key, &value, &size);
    if (ret == PREFERENCES_ERROR_KEY_NOT_FOUND) {
        WVLOG_I("the key does not exist key:%{public}s", key);
        return defaultValue;
    } else if (ret != PREFERENCES_OK) {
        WVLOG_E("failed to get string, ret %{public}d", ret);
        return defaultValue;
    }

    if (value == nullptr) {
        WVLOG_E("failed to get string, value is nullptr");
        return defaultValue;
    }

    WVLOG_D("get string param, key:%{public}s, value:%{public}s", key, value);
    std::string str = value;
    OH_Preferences_FreeString(value);
    return str;
}

void SystemPropertiesAdapterImpl::SetStringParameter(const char *key, const char *value)
{
    if (preferences_ == nullptr) {
        WVLOG_E("preferences is null");
        return;
    }

    if (key == nullptr || value == nullptr) {
        WVLOG_E("param is nullptr");
        return;
    }

    int ret = OH_Preferences_SetString(preferences_, key, value);
    if (ret != PREFERENCES_OK) {
        WVLOG_E("failed to set string, ret %{public}d", ret);
        return;
    }
    WVLOG_D("set string param, key:%{public}s, value:%{public}s", key, value);
}

std::vector<FrameRateSetting> SystemPropertiesAdapterImpl::GetLTPOConfig(const std::string& settingName)
{
#ifdef WEBVIEW_ONLY
    return NWebConfigHelper::Instance().GetPerfConfig(settingName);
#else
    std::vector<FrameRateSetting> vector;
    return vector;
#endif
}

bool SystemPropertiesAdapterImpl::IsLTPODynamicApp(const std::string& bundleName)
{
#ifdef WEBVIEW_ONLY
    return NWebConfigHelper::Instance().IsLTPODynamicApp(bundleName);
#else
    return false;
#endif
}

int32_t SystemPropertiesAdapterImpl::GetLTPOStrategy()
{
#ifdef WEBVIEW_ONLY
    return NWebConfigHelper::Instance().GetLTPOStrategy();
#else
    return 0;
#endif
}

std::string SystemPropertiesAdapterImpl::GetVulkanStatus()
{
    std::string gpuVendor = GetStringParameter("const.gpu.vendor", "0");
    if ((gpuVendor.compare("higpu.v200") == 0)
        || (gpuVendor.compare("higpu.v210") == 0)
        || (gpuVendor.compare("higpu.v300") == 0)) {
        return GetStringParameter("web.ohos.vulkan", "");
    } else {
        return "false";
    }
}

std::string SystemPropertiesAdapterImpl::GetCompatibleDeviceType()
{
    char* compatibleDeviceType = OH_NativeBundle_GetCompatibleDeviceType();
    if (compatibleDeviceType == nullptr) {
        WVLOG_E("failed get compatible device type.");
        return "";
    }

    std::string deviceType(compatibleDeviceType);
    free(compatibleDeviceType);

    return deviceType;
}

std::string SystemPropertiesAdapterImpl::GetDeviceInfoApiVersion()
{
    return GetStringParameter("const.product.os.dist.apiversion", "");
}

std::string SystemPropertiesAdapterImpl::GetPRPPreloadMode()
{
    return GetStringParameter("web.prppreload.mode", "none");
}

std::string SystemPropertiesAdapterImpl::GetScrollVelocityScale()
{
    return GetStringParameter("persist.scrollable.velocityScale", "");
}

std::string SystemPropertiesAdapterImpl::GetScrollFriction()
{
    return GetStringParameter("persist.scrollable.friction", "");
}

std::string SystemPropertiesAdapterImpl::GetBundleName()
{
#ifdef WEBVIEW_ONLY
    return NWebConfigHelper::Instance().GetBundleName();
#else
    return "";
#endif
}
} // namespace OHOS::NWeb
