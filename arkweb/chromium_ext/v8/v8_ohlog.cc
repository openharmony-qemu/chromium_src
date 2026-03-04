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
#include "v8_ohlog.h"

#include <iostream>

#include "hilog.h"
#include "ohlog.h"

#if defined(USING_OHOS_WEB) || defined(USING_OHOS)

#ifdef USING_OHOS_WEB
void LogInfo(std::string message) {
  StreamHilog(message);
}

#else   // USING_OHOS_WEB

void LogInfo(std::string message) {
  HilogPrint(INFO, "%{public}s", message.c_str());
}
#endif  // USING_OHOS_WEB

#else  // defined(USING_OHOS_WEB) || defined(USING_OHOS)

void LogInfo(std::string message) {
  std::cout << message << std::endl;
}

#endif  // defined(USING_OHOS_WEB) || defined(USING_OHOS)
