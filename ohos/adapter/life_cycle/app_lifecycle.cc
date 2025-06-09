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

#include "ohos/adapter/life_cycle/app_lifecycle.h"

#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::life_cycle {

void AppLifeCycle::RegisterAppLifecycle(napi_env env, napi_value exports) {
  napi_property_descriptor desc[] = {
      {"onCreate", nullptr, AppLifeCycle::OnCreate, nullptr, nullptr, nullptr,
       napi_default, nullptr},
      {"onShow", nullptr, AppLifeCycle::OnShow, nullptr, nullptr, nullptr,
       napi_default, nullptr},
      {"onHide", nullptr, AppLifeCycle::OnHide, nullptr, nullptr, nullptr,
       napi_default, nullptr},
      {"onDestroy", nullptr, AppLifeCycle::OnDestroy, nullptr, nullptr, nullptr,
       napi_default, nullptr}};
  napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
}

napi_value AppLifeCycle::OnCreate(napi_env env, napi_callback_info info) {
  LOGE("AppLifeCycle::OnCreate");
  return nullptr;
}

napi_value AppLifeCycle::OnShow(napi_env env, napi_callback_info info) {
  LOGE("AppLifeCycle::OnShow");
  return nullptr;
}

napi_value AppLifeCycle::OnHide(napi_env env, napi_callback_info info) {
  LOGE("AppLifeCycle::OnHide");
  return nullptr;
}

napi_value AppLifeCycle::OnDestroy(napi_env env, napi_callback_info info) {
  LOGE("AppLifeCycle::OnDestroy");
  return nullptr;
}

}  // namespace life_cycle
