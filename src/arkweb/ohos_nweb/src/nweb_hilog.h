/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_HILOG_H
#define NWEB_HILOG_H

#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_DFX_LOGGING)
#include "third_party/ohos_ndk/includes/ohos_adapter/hilog_adapter.h"
constexpr char logTag[] = "chromium";

#define FILE_NAME (__builtin_strrchr("/" __FILE__, '/') + 1)
#define FUNC_LINE_FMT "[%{public}s:%{public}d] "

using OHOS::NWeb::HiLogAdapter;
using OHOS::NWeb::LogLevelAdapter;

#define WVLOG_D(fmt, ...)                                                   \
  HiLogAdapter::PrintLog(LogLevelAdapter::DEBUG, logTag, FUNC_LINE_FMT fmt, \
                         FILE_NAME, __LINE__, ##__VA_ARGS__)
#define WVLOG_I(fmt, ...)                                                  \
  HiLogAdapter::PrintLog(LogLevelAdapter::INFO, logTag, FUNC_LINE_FMT fmt, \
                         FILE_NAME, __LINE__, ##__VA_ARGS__)
#define WVLOG_W(fmt, ...)                                                  \
  HiLogAdapter::PrintLog(LogLevelAdapter::WARN, logTag, FUNC_LINE_FMT fmt, \
                         FILE_NAME, __LINE__, ##__VA_ARGS__)
#define WVLOG_E(fmt, ...)                                                   \
  HiLogAdapter::PrintLog(LogLevelAdapter::ERROR, logTag, FUNC_LINE_FMT fmt, \
                         FILE_NAME, __LINE__, ##__VA_ARGS__)
#define WVLOG_F(fmt, ...)                                                   \
  HiLogAdapter::PrintLog(LogLevelAdapter::FATAL, logTag, FUNC_LINE_FMT fmt, \
                         FILE_NAME, __LINE__, ##__VA_ARGS__)

#else  // not ARKWEB_DFX_LOGGING

#define WVLOG(fmt, ...)

#define WVLOG_D(fmt, ...) WVLOG(fmt, ##__VA_ARGS__)
#define WVLOG_I(fmt, ...) WVLOG(fmt, ##__VA_ARGS__)
#define WVLOG_W(fmt, ...) WVLOG(fmt, ##__VA_ARGS__)
#define WVLOG_E(fmt, ...) WVLOG(fmt, ##__VA_ARGS__)
#define WVLOG_F(fmt, ...) WVLOG(fmt, ##__VA_ARGS__)

#endif  // ARKWEB_DFX_LOGGING

#endif  // NWEB_HILOG_H
