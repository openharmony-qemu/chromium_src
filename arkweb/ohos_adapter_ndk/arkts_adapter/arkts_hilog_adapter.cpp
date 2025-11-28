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

 #include "arkts_hilog_adapter.h"

 #include <unistd.h>
 #include <sys/types.h>
 #include "securec.h"
 #include <cstdarg>
 #include <cstddef>
 #include <unordered_map>

constexpr uint32_t BROWSER_UID_BASE = 20000000;
#if defined(X86_64_ENABLE)
constexpr uint32_t LOG_APP_DOMAIN = 0x004500;
constexpr uint32_t LOG_RENDER_DOMAIN = 0x004501;
#else
constexpr uint32_t LOG_APP_DOMAIN = 0xD004510;
constexpr uint32_t LOG_RENDER_DOMAIN = 0xD004511;
#endif
constexpr uint32_t LOG_CONSOLE_DOMAIN = 0x001194;
constexpr uint32_t MAX_LENGTH = 1024;
const std::string PUBLIC_STR = "{public}";
const std::string PRIVATE_STR = "{private}";
const std::string STD_FORMAT = "%{public}s";
const uint32_t ARG_NUM = 3;

static const std::unordered_map<LogLevel,  std::string> LEVEL_MAP = {
    {LogLevel::DEBUG, "debug"},
    {LogLevel::INFO, "info"},
    {LogLevel::WARN, "warn"},
    {LogLevel::ERROR, "error"},
    {LogLevel::FATAL, "fatal"},
};

extern "C" {
inline void Format(std::string& fmtStr)
{
    size_t pos = 0;
    while (std::string::npos != (pos = fmtStr.find(PUBLIC_STR))) {
        fmtStr.erase(pos, PUBLIC_STR.length());
    }
    pos = 0;
    while (std::string::npos != (pos = fmtStr.find(PRIVATE_STR))) {
        fmtStr.erase(pos, PRIVATE_STR.length());
    }
    if (fmtStr.length() > MAX_LENGTH) {
        fmtStr.erase(MAX_LENGTH);
    }
}

std::string GetFormatString(const char* fmt, va_list ap)
{
    std::string fmtStr(fmt);
    Format(fmtStr);
    char buffer[MAX_LENGTH];
    int ret = vsnprintf_s(buffer, MAX_LENGTH, MAX_LENGTH - 1, fmtStr.c_str(), ap);
    if (ret < 0) {
        return "";
    }
    std::string result(buffer);
    return result;
}
}

napi_env ArktsHilogAdapter::GetEnv()
{
    static napi_env env = nullptr;
    if (!env) {
        napi_create_ark_runtime(&env);
    }
    return env;
}

napi_value ArktsHilogAdapter::GetHilogModule()
{
    static napi_value hilogModule = nullptr;
    napi_env env = GetEnv();
    if (!hilogModule) {
        napi_load_module_with_info(env, "@ohos.hilog", nullptr, &hilogModule);
    }
    return hilogModule;
}

int ArktsHilogAdapter::LogInternal(LogLevel level, const char* fmt, ...)
{
    napi_env env = GetEnv();
    if (!env) {
        return -1;
    }

    napi_value hilogModule = GetHilogModule();
    if(!hilogModule) {
        return -1;
    }

    napi_value infoFn;
    auto it = LEVEL_MAP.find(level);
    if (it == LEVEL_MAP.end()) {
        return -1;
    }
    napi_status status = napi_get_named_property(env, hilogModule, it->second.c_str(), &infoFn);
    if (status != napi_ok) {
        return -1;
    }

    napi_value tag;
    std::string formatStr = "chromium";
    napi_create_string_utf8(env, formatStr.c_str(), formatStr.size(), &tag);

    va_list ap;
    va_start(ap, fmt);
    std::string outputString = GetFormatString(fmt, ap);
    va_end(ap);

    napi_value outputResult;
    napi_create_string_utf8(env, outputString.c_str(), outputString.size(), &outputResult);

    napi_value flag;
    uint32_t domain = LOG_RENDER_DOMAIN;
    if ((getuid() / BROWSER_UID_BASE) != 0) {
        domain = LOG_APP_DOMAIN;
    }
    napi_create_int32(env, domain, &flag);

    napi_value args[ARG_NUM] = {flag, tag, outputResult};
    status = napi_call_function(env, hilogModule, infoFn, ARG_NUM, args, nullptr);
    if (status != napi_ok) {
        return -1;
    }

    return 0;
}
