// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "native_initializer.h"

#include <napi/native_api.h>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/context_path/context_path_adapter.h"
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

static std::string GetStringProperty(napi_env env,
                                     napi_value obj,
                                     const char* key) {
  napi_value prop;
  if (napi_get_named_property(env, obj, key, &prop) != napi_ok) {
    return "";
  }
  napi_valuetype type;
  napi_typeof(env, prop, &type);
  if (type != napi_string) {
    return "";
  }
  size_t len;
  napi_get_value_string_utf8(env, prop, nullptr, 0, &len);
  std::string result(len, '\0');
  napi_get_value_string_utf8(env, prop, &result[0], len + 1, &len);
  return result;
}

static napi_value SetContextPathsWrapper(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];
  if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok) {
    napi_throw_type_error(env, nullptr, "napi_get_cb_info failed");
    return nullptr;
  }
  if (argc != 1) {
    napi_throw_type_error(env, nullptr, "Wrong number of arguments");
    return nullptr;
  }
  napi_valuetype type;
  if (napi_typeof(env, args[0], &type) != napi_ok) {
    napi_throw_type_error(env, nullptr, "napi_typeof failed");
    return nullptr;
  }
  if (type != napi_object) {
    napi_throw_type_error(env, nullptr, "Argument must be an object");
    return nullptr;
  }

  ohos::adapter::ContextPathAdapter::ContextPaths paths;
  paths.cache_dir = GetStringProperty(env, args[0], "cacheDir");
  paths.files_dir = GetStringProperty(env, args[0], "filesDir");
  paths.temp_dir = GetStringProperty(env, args[0], "tempDir");
  paths.user_document_dir = GetStringProperty(env, args[0], "userDocumentDir");
  paths.user_desktop_dir = GetStringProperty(env, args[0], "userDesktopDir");
  paths.user_download_dir = GetStringProperty(env, args[0], "userDownloadDir");

  ohos::adapter::ContextPathAdapter::SetContextPaths(paths);
  return nullptr;
}

static napi_value Initialize(napi_env env, napi_value exports) {
  TRACE_EVENT_0("Initialize");
  if (env == nullptr || exports == nullptr) {
    LOGE("Initialize invalid parameters!");
    return exports;
  }
  napi_property_descriptor desc[] = {
      DECLARE_NAPI_METHOD("getNativeContext", GetNativeContext),
      DECLARE_NAPI_METHOD("SetContextPaths", SetContextPathsWrapper)};
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
