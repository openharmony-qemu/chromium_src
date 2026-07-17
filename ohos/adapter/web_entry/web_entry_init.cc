/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#include "ohos/adapter/web_entry/web_entry_init.h"

#include <vector>

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>

#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/web_entry/web_entry.h"

namespace {
std::vector<int32_t> GetInt32Array(napi_env env, napi_value array) {
  std::vector<int32_t> result;
  uint32_t length = 0;
  if (napi_get_array_length(env, array, &length) == napi_ok) {
    for (uint32_t i = 0; i < length; i++) {
      napi_value element;
      if (napi_get_element(env, array, i, &element) == napi_ok) {
        int32_t value = -1;
        if (napi_get_value_int32(env, element, &value) == napi_ok) {
          result.push_back(value);
        }
      }
    }
  }
  return result;
}
}  // namespace

namespace ohos::adapter::web_entry {
static napi_value RunBrowser(napi_env env, napi_callback_info info) {
  TRACE_EVENT_0("RunBrowser");
  std::vector<std::string> vec_args;
  size_t argc = 1;
  napi_value args[1] = {nullptr};
  
  if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok) {
    LOGE("RunBrowser napi_get_cb_info failed");
    return nullptr;
  }
  uint32_t args_len = 0;
  napi_status status = napi_get_array_length(env, args[0], &args_len);
  if (status != napi_ok || args_len == 0) {
    web_entry::RunBrowser(vec_args);
    return nullptr;
  }

  for (uint32_t i = 0; i < args_len; i++) {
    napi_value element;
    status = napi_get_element(env, args[0], i, &element);
    if (status != napi_ok) {
      break;
    }
    size_t element_len = 0;
    status = napi_get_value_string_utf8(env, element, nullptr, 0, &element_len);
    if (status != napi_ok) {
      break;
    }
    char* element_value = new char[element_len + 1];
    if (element_value) {
      status = napi_get_value_string_utf8(env, element, element_value,
                                          element_len + 1, &element_len);
      if (status != napi_ok) {
        delete[] element_value;
        break;
      }
      vec_args.push_back(element_value);
      delete[] element_value;
    }
  }
  
  web_entry::RunBrowser(vec_args);
  return nullptr;
}

static napi_value RunArkProcess(napi_env env, napi_callback_info info) {
  size_t argc = 3;
  napi_value args[3] = {nullptr};

  if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok) {
    LOGE("RunArkProcess napi_get_cb_info failed");
    return nullptr;
  }

  if (argc != 3) {
    napi_throw_type_error(env, nullptr, "Wrong number of arguments");
    return nullptr;
  }

  size_t entry_params_len = 0;
  if (napi_get_value_string_utf8(env, args[0], nullptr, 0, &entry_params_len) !=
      napi_ok) {
    napi_throw_type_error(env, nullptr,
                          "napi_get_value for entry_params failed");
    return nullptr;
  }

  std::vector<char> entry_params(entry_params_len + 1);
  if (napi_get_value_string_utf8(env, args[0], entry_params.data(),
                                 entry_params_len + 1,
                                 &entry_params_len) != napi_ok) {
    napi_throw_type_error(env, nullptr,
                          "napi_get_value for entry_params failed");
    return nullptr;
  }

  std::vector<int32_t> fds_id = GetInt32Array(env, args[1]);
  std::vector<int32_t> fds = GetInt32Array(env, args[2]);

  web_entry::RunArkProcess(entry_params.data(), entry_params_len,
                           fds_id, fds);
  return nullptr;
}

void Register(napi_env env, napi_value exports) {
  napi_property_descriptor desc[] = {
      DECLARE_NAPI_METHOD("runBrowser", web_entry::RunBrowser),
      DECLARE_NAPI_METHOD("runArkProcess", web_entry::RunArkProcess)
  };
  if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]),
                             desc) != napi_ok) {
    LOGE("napi_define_properties failed");
    return;
  }
}
}  // namespace ohos::adapter::web_entry
