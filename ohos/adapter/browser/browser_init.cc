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

#include <js_native_api.h>
#include <js_native_api_types.h>

#include <string>

#include "aki/value/array_buffer.h"
#include "browser_adapter.h"
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::browseradapter {

static napi_value StartNewWindow(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[2] = {nullptr};

  napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  aki::Value val0(args[0]);
  std::string uri = val0.As<std::string>();
  aki::Value val1(args[1]);
  bool force_open = val1.As<bool>();
  LOGI("StartNewWindow, uri=%{public}s, force_open=%{public}d", uri.c_str(), force_open);
  BrowserAdapter::GetInstance().StartNewWindow(uri, force_open);
  return nullptr;
}

void Register(napi_env env, napi_value exports) {
  napi_property_descriptor desc[] = {
      DECLARE_NAPI_METHOD("startNewWindow", StartNewWindow)};
  if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]),
                             desc) != napi_ok) {
    LOGE("napi_define_properties failed");
    return;
  }
}

}  // namespace ohos::adapter::browseradapter
