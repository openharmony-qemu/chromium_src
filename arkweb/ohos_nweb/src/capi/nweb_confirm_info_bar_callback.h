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

#ifndef OHOS_NWEB_SRC_NWEB_CONFIRM_INFO_BAR_CALLBACK_H_
#define OHOS_NWEB_SRC_NWEB_CONFIRM_INFO_BAR_CALLBACK_H_
#include <string>
#include "arkweb/ohos_nweb/include/nweb_value_callback.h"

struct ConfirmInfoBarMessage {
  std::string title;
  std::string infoId;
  std::string message;
  int buttons;
  std::string buttonLabelOK;
  std::string buttonLabelCancel;
};
typedef void (*OnShowConfirmInfoBarFunc)(const std::string& title,
                                         const std::string& infoId,
                                         const std::string& message,
                                         int buttons,
                                         const std::string& buttonLabelOK,
                                         const std::string& buttonLabelCancel);
#if !BUILDFLAG(ARKWEB_NWEB_EX)
typedef void (*OnArkWebStaticShowConfirmInfoBarFunc)(const char* title,
                                                     const char* infoId,
                                                     const char* message,
                                                     int buttons,
                                                     const char* buttonLabelOK,
                                                     const char* buttonLabelCancel);

#endif
#endif  // OHOS_NWEB_SRC_NWEB_CONFIRM_INFO_BAR_CALLBACK_H_
