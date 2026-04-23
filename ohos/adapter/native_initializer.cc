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

#include "native_initializer.h"

#include <napi/native_api.h>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/life_cycle/lifecycle_init.h"
#include "ohos/adapter/media_manager/media_init.h"
#include "ohos/adapter/task_runner/main_thread_task_runner.h"
#include "ohos/adapter/web_entry/web_entry_init.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"
EXTERN_C_START
static napi_value GetNativeContextByType(napi_env env,
                                         napi_value exports,
                                         int32_t process_type) {
  TRACE_EVENT_0("GetNativeContextByType");
  switch (process_type) {
    case ProcessType::kMainProcess: {
      ohos::adapter::web_entry::Register(env, exports);
      ohos::adapter::life_cycle::Register(env, exports);
      ohos::adapter::media::Register(env, exports);
      ohos::adapter::taskRunner::MainThreadTaskRunner::GetInstance().Initialize(
          env);
      aki::JSBind::BindSymbols(env, exports);
      aki::Binding::SetScopedEnv(env);
      break;
    }

    case ProcessType::kRenderProcess:
    case ProcessType::kCrashpadHandler:
      ohos::adapter::web_entry::Register(env, exports);
      break;
    default:
      break;
  }

  return exports;
}

static napi_value GetNativeContext(napi_env env, napi_callback_info info) {
  napi_value exports;
  if (napi_create_object(env, &exports) != napi_ok) {
    napi_throw_type_error(env, nullptr, "napi_create_object failed");
    return exports;
  }

  size_t argc = 1;
  napi_value args[1] = {nullptr};

  if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok) {
    napi_throw_type_error(env, nullptr, "napi_get_cb_info failed");
    return exports;
  }

  if (argc != 1) {
    napi_throw_type_error(env, nullptr, "Wrong number of arguments");
    return exports;
  }

  napi_valuetype valuetype;
  if (napi_typeof(env, args[0], &valuetype) != napi_ok) {
    napi_throw_type_error(env, nullptr, "napi_typeof failed");
    return exports;
  }

  if (valuetype != napi_number) {
    napi_throw_type_error(env, nullptr, "Wrong type of arguments");
    return exports;
  }

  int32_t process_type = -1;
  if (napi_get_value_int32(env, args[0], &process_type) != napi_ok) {
    napi_throw_type_error(env, nullptr, "napi_get_value failed");
    return exports;
  }

  return GetNativeContextByType(env, exports, process_type);
}

static napi_value Initialize(napi_env env, napi_value exports) {
  TRACE_EVENT_0("Initialize");
  if (env == nullptr || exports == nullptr) {
    LOGE("Initialize invalid parameters!");
    return exports;
  }
  napi_property_descriptor desc[] = {
      DECLARE_NAPI_METHOD("getNativeContext", GetNativeContext)
  };
  if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]),
                             desc) != napi_ok) {
    LOGE("napi_define_properties failed");
    return exports;
  }

  // Very ugly design!
  // this method to check whether the environment variable contains
  // an instance of the XComponent component, and if so,
  // register the relevant interfaces for drawing.
  //
  // Triggered when libadapter.so is loaded.
  ohos::adapter::xcomponent::XComponentManager::GetInstance()->Initialize(
      env, exports);
  return exports;
}
EXTERN_C_END

static napi_module entryModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Initialize,
    .nm_modname = "adapter",
    .nm_priv = ((void*)nullptr),
    .reserved = {nullptr},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) {
  napi_module_register(&entryModule);
}
