// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/life_cycle/lifecycle_init.h"

#include <atomic>
#include <chrono>
#include <future>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/life_cycle/app_lifecycle.h"
#include "ohos/adapter/life_cycle/page_lifecycle.h"

namespace ohos::adapter::life_cycle {

std::atomic<bool> g_launchStatus(true);

static napi_value RegisterLifecycle(napi_env env, napi_callback_info info) {
  napi_value exports;
  if (napi_create_object(env, &exports) != napi_ok) {
    napi_throw_type_error(env, nullptr, "napi_create_object failed");
    return nullptr;
  }

  AppLifeCycle::RegisterAppLifecycle(env, exports);
  PageLifeCycle::RegisterPageLifecycle(env, exports);
  return exports;
}

void Register(napi_env env, napi_value exports) {
  napi_property_descriptor desc[] = {{"registerLifecycle", nullptr,
                                      RegisterLifecycle, nullptr, nullptr,
                                      nullptr, napi_default, nullptr}};

  if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]),
                             desc) != napi_ok) {
    LOGE("napi_define_properties failed");
    return;
  }
}

void Launch(bool state) {
  LOGI("The browser process launch state: %{public}d", state);
  g_launchStatus.store(state);
}

void Destroy() {
  LOGI("The browser process has exited");

  auto jsfunc =
      ohos::adapter::GetJSFunction("AppLifecycleAdapter.onWebDestroy");
  if (!jsfunc) {
    LOGE("AppLifecycleAdapter.onWebDestroy not defined");
    return;
  }
  jsfunc->Invoke<void>();
}

}  // namespace ohos::adapter::life_cycle
