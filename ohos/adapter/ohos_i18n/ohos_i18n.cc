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

#include "ohos/adapter/ohos_i18n/ohos_i18n.h"

#include <BasicServicesKit/time_service.h>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::ohos_i18n {
std::string GetTimeZone() {
  char time_zone[kLen] = "";
  TimeService_ErrCode code = OH_TimeService_GetTimeZone(time_zone, kLen);
  if (code == TimeService_ErrCode::TIMESERVICE_ERR_OK) {
    return std::string(time_zone);
  }
  LOGE("get [GetTimeZone] function fail.");
  return "";
}

std::string GetLocaleLang() {
  if (auto getLocaleLangFunc =
          ohos::adapter::GetJSFunction("OhosI18nAdapter.GetLocaleLang")) {
    return getLocaleLangFunc->Invoke<std::string>();
  }
  LOGE("get [GetLocaleLang] function fail.");
  return "";
}

std::string getLocaleRegion() {
  if (auto getLocaleRegionFunc =
          ohos::adapter::GetJSFunction("OhosI18nAdapter.getLocaleRegion")) {
    return getLocaleRegionFunc->Invoke<std::string>();
  }
  LOGE("get [getLocaleRegion] function fail.");
  return "";
}

void RegisterTimeZoneListener(TimeZoneChangeCallback callback) {
  auto func =
      ohos::adapter::GetJSFunction("OhosI18nAdapter.RegisterTimeZoneListener");
  if (func) {
    func->Invoke<void>(callback);
  }
}

void UnsubscribeTimeZoneListener() {
  auto func =
      ohos::adapter::GetJSFunction("OhosI18nAdapter.UnsubscribeTimeZoneListener");
  if (func) {
    func->Invoke<void>();
  }
}
}  // namespace ohos::adapter::ohos_i18n
