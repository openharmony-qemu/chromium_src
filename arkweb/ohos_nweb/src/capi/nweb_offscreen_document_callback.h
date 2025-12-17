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

#ifndef OHOS_NWEB_SRC_NWEB_OFFSCREEN_DOCUMENT_CALLBACK_H_
#define OHOS_NWEB_SRC_NWEB_OFFSCREEN_DOCUMENT_CALLBACK_H_

#include <string>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef void (*OnArkWebStaticOffscreenDocumentAlertFunc)(const char* extensionId,
                                                         const char* url,
                                                         const char* message,
                                                         const int requestId);

typedef void (*OnArkWebStaticOffscreenDocumentConfirmFunc)(const char* extensionId,
                                                           const char* url,
                                                           const char* message,
                                                           const int requestId);

typedef void (*OnArkWebStaticOffscreenDocumentPromptFunc)(const char* extensionId,
                                                          const char* url,
                                                          const char* message,
                                                          const char* value,
                                                          const int requestId);

typedef void (*OnArkWebStaticOffscreenDocumentWindowNewFunc)(const char* extensionId,
                                                             const char* originUrl,
                                                             bool isAlert,
                                                             bool isUserTrigger,
                                                             const char* targetUrl);
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // OHOS_NWEB_SRC_NWEB_OFFSCREEN_DOCUMENT_CALLBACK_H_
