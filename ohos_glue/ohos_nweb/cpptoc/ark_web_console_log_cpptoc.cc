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

#include "ohos_nweb/cpptoc/ark_web_console_log_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

ArkWebString ARK_WEB_CALLBACK
ark_web_console_log_log(struct _ark_web_console_log_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebConsoleLogCppToC::Get(self)->Log();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_console_log_source_id(struct _ark_web_console_log_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebConsoleLogCppToC::Get(self)->SourceId();
}

int ARK_WEB_CALLBACK
ark_web_console_log_log_level(struct _ark_web_console_log_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebConsoleLogCppToC::Get(self)->LogLevel();
}

int ARK_WEB_CALLBACK
ark_web_console_log_line_numer(struct _ark_web_console_log_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebConsoleLogCppToC::Get(self)->LineNumer();
}

} // namespace

ArkWebConsoleLogCppToC::ArkWebConsoleLogCppToC() {
  GetStruct()->log = ark_web_console_log_log;
  GetStruct()->source_id = ark_web_console_log_source_id;
  GetStruct()->log_level = ark_web_console_log_log_level;
  GetStruct()->line_numer = ark_web_console_log_line_numer;
}

ArkWebConsoleLogCppToC::~ArkWebConsoleLogCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebConsoleLogCppToC, ArkWebConsoleLog,
                           ark_web_console_log_t>::kBridgeType =
        ARK_WEB_CONSOLE_LOG;

} // namespace OHOS::ArkWeb
