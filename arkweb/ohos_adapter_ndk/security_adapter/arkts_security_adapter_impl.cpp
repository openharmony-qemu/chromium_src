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
 #include <sstream>

 #define ARG_NUM 1
const static uint64_t EVENT_ID = 0x02C000002;
const static std::string VERSION = "1.0";

napi_env ArktsSecurityAdapter::GetEnv()
{
    static napi_env env = nullptr;
    if (!env) {
        napi_create_ark_runtime(&env);
    }
    return env;
}

napi_value ArktsSecurityAdapter::GetSecurityGuardModule()
{
    static napi_value securityGuardModule = nullptr;
    napi_env env = GetEnv();
    if (!securityGuardModule) {
        napi_load_module_with_info(env, "@ohos.security.securityGuard", nullptr, &securityGuardModule);
    }
    return securityGuardModule;
}

int ArktsSecurityAdapter::RegisterEvent(const std::string& event)
{
    napi_env env = GetEnv();
    if (!env) {
        return -1;
    }

    napi_value securityModule = GetSecurityGuardModule();
    if(!securityModule) {
        return -1;
    }

    napi_value reportFn;
    napi_status status = napi_get_named_property(env, securityModule, "reportSecurityEvent", &reportFn);
    if (status != napi_ok) {
        return -1;
    }

    napi_value eventId;
    napi_create_int32(env, EVENT_ID, &eventId);

    napi_value version;
    napi_create_string_utf8(env, VERSION.c_str(), VERSION.size(), &version);

    // construct JSON
    std::ostringstream oss;
    oss << R"({"content":")" << event << R"("})";
    std::string securityInfo = oss.str();
    napi_value content;
    napi_create_string_utf8(env, securityInfo.c_str(), securityInfo.size(), &content);

    napi_value obj;
    napi_create_object(env,&obj);

    napi_set_named_property(env, obj, "eventId", eventId);
    napi_set_named_property(env, obj, "version", version);
    napi_set_named_property(env, obj, "content", content);

    napi_value args[ARG_NUM] = {obj};
    status = napi_call_function(env, securityModule, reportFn, ARG_NUM, args, nullptr);
    if (status != napi_ok) {
        return -1;
    }

    return 0;
}