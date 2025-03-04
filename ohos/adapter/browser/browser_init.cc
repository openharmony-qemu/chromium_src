// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
  size_t argc = 1;
  napi_value args[1] = {nullptr};

  napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  aki::Value val(args[0]);
  std::string uri = val.As<std::string>();
  LOGI("StartNewWindow, uri=%{public}s", uri.c_str());
  BrowserAdapter::GetInstance().StartNewWindow(uri);
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
