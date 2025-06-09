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

#include "ohos/adapter/web_entry/web_entry_init.h"

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>

#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/web_entry/web_entry.h"

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

static napi_value RunOtherProcessType(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1] = {nullptr};

  if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok) {
    LOGE("RunOtherProcessType napi_get_cb_info failed");
    return nullptr;
  }

  if (argc != 1) {
    napi_throw_type_error(env, nullptr, "Wrong number of arguments");
    return nullptr;
  }

  napi_valuetype valuetype;
  if (napi_typeof(env, args[0], &valuetype) != napi_ok) {
    napi_throw_type_error(env, nullptr, "napi_typeof failed");
    return nullptr;
  }

  if (valuetype != napi_number) {
    napi_throw_type_error(env, nullptr, "Wrong type of arguments");
    return nullptr;
  }

  int32_t process_type = -1;
  if (napi_get_value_int32(env, args[0], &process_type) != napi_ok) {
    napi_throw_type_error(env, nullptr, "napi_get_value failed");
    return nullptr;
  }

  TRACE_EVENT_1("RunOtherProcessType", "process_type", process_type);
  web_entry::RunOtherProcessType(process_type);
  return nullptr;
}

void Register(napi_env env, napi_value exports) {
  napi_property_descriptor desc[] = {
      DECLARE_NAPI_METHOD("runBrowser", web_entry::RunBrowser),
      DECLARE_NAPI_METHOD("runOtherProcessType", web_entry::RunOtherProcessType)
  };
  if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]),
                             desc) != napi_ok) {
    LOGE("napi_define_properties failed");
    return;
  }
}
}  // namespace ohos::adapter::web_entry
