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

#ifndef ARKTS_HILOG_ADAPTER_H
#define ARKTS_HILOG_ADAPTER_H

#include "napi/native_api.h"
#include "js_native_api.h"
#include <string>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    FATAL = 4
};

class ArktsHilogAdapter {
public:
    static napi_env GetEnv();
    static napi_value GetHilogModule();
    static int LogInternal(LogLevel level, const char* fmt, ...);
};


#define FILE_NAME (__builtin_strrchr("/" __FILE__, '/') + 1)
#define FUNC_LINE_FMT "[%{public}s:%{public}d] "

#define ALOG_D(fmt, ...) ArktsHilogAdapter::LogInternal(LogLevel::DEBUG, FUNC_LINE_FMT fmt, \
    FILE_NAME, __LINE__, ##__VA_ARGS__)
#define ALOG_I(fmt, ...) ArktsHilogAdapter::LogInternal(LogLevel::INFO, FUNC_LINE_FMT fmt, \
    FILE_NAME, __LINE__, ##__VA_ARGS__)
#define ALOG_W(fmt, ...) ArktsHilogAdapter::LogInternal(LogLevel::WARN, FUNC_LINE_FMT fmt, \
    FILE_NAME, __LINE__, ##__VA_ARGS__)
#define ALOG_E(fmt, ...) ArktsHilogAdapter::LogInternal(LogLevel::ERROR, FUNC_LINE_FMT fmt, \
    FILE_NAME, __LINE__, ##__VA_ARGS__)
#define ALOG_F(fmt, ...) ArktsHilogAdapter::LogInternal(LogLevel::FATAL, FUNC_LINE_FMT fmt, \
    FILE_NAME, __LINE__, ##__VA_ARGS__)

#endif // ARKTS_HILOG_ADAPTER_H
