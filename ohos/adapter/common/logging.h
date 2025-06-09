/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OHOS_ADAPTER_COMMON_LOGGING_H_
#define OHOS_ADAPTER_COMMON_LOGGING_H_

#include <hilog/log.h>

#define APP_LOG_DOMAIN 0x0001
#define APP_LOG_TAG "Adapter"

#define LOGI(...)                                                     \
  ((void)OH_LOG_Print(LOG_APP, LOG_INFO, APP_LOG_DOMAIN, APP_LOG_TAG, \
                      __VA_ARGS__))

#define LOGD(...)                                                      \
  ((void)OH_LOG_Print(LOG_APP, LOG_DEBUG, APP_LOG_DOMAIN, APP_LOG_TAG, \
                      __VA_ARGS__))

#define LOGW(...)                                                     \
  ((void)OH_LOG_Print(LOG_APP, LOG_WARN, APP_LOG_DOMAIN, APP_LOG_TAG, \
                      __VA_ARGS__))

#define LOGE(...)                                                      \
  ((void)OH_LOG_Print(LOG_APP, LOG_ERROR, APP_LOG_DOMAIN, APP_LOG_TAG, \
                      __VA_ARGS__))

#endif  // OHOS_ADAPTER_COMMON_LOGGING_H_
