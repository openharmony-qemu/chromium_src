/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef V8_UTILS_HILOG_H_
#define V8_UTILS_HILOG_H_

typedef enum {
  /** Debug level to be used by {@link OH_LOG_DEBUG} */
  DEBUG = 3,
  /** Informational level to be used by {@link OH_LOG_INFO} */
  INFO = 4,
  /** Warning level to be used by {@link OH_LOG_WARN} */
  WARN = 5,
  /** Error level to be used by {@link OH_LOG_ERROR} */
  ERROR = 6,
  /** Fatal level to be used by {@link OH_LOG_FATAL} */
  FATAL = 7,
} HiLogLevel;

#ifdef USING_OHOS
#define LOG_TAG "jsvm_hilog"
#include "log.h"
#define HilogPrint(logLevel, ...)                                       \
  ((void)OH_LOG_Print(LOG_APP, LogLevel(logLevel), LOG_DOMAIN, LOG_TAG, \
                      __VA_ARGS__))
#else
#define HilogPrint(logLevel, ...)
#endif

#endif
