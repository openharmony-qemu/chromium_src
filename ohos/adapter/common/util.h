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

#ifndef OHOS_ADAPTER_COMMON_UTIL_H_
#define OHOS_ADAPTER_COMMON_UTIL_H_

#include <string>
#include "aki/jsbind.h"
#include "ohos/adapter/common/logging.h"

template <typename R, typename... P>
R GetAkiResult(const std::string& func_name,
               P... args,
               const R& default_value) {
  auto aki_func = ohos::adapter::GetJSFunction(func_name);
  if (!aki_func) {
    LOGE("GetAkiResult js binding error: %{public}s function undefined.",
         func_name.c_str());
    return default_value;
  }
  return aki_func->Invoke<R>(std::forward<P>(args)...);
}

template <typename R, typename... P>
R GetAkiResultAsync(const std::string& func_name,
                    P... args,
                    const R& default_value) {
  auto aki_func = ohos::adapter::GetJSFunction(func_name);
  if (!aki_func) {
    LOGE("GetAkiResult js binding error: %{public}s function undefined.",
         func_name.c_str());
    return default_value;
  }

  std::promise<R> result_promise;
  std::function<void(R)> callback = [&result_promise](R result) {
    result_promise.set_value(result);
  };
  aki_func->Invoke<void>(std::forward<P>(args)..., callback);
  return result_promise.get_future().get();
}

template <class T>
T GetValue(aki::Value aki_value, T default_value) {
  if (!(aki_value.IsUndefined() || aki_value.IsNull())) {
    return aki_value.As<T>();
  }
  return default_value;
}

uint32_t GetArrayLength(const aki::Value& array) {
  uint32_t length = 0;
  if (!array.IsArray()) {
    return length;
  }

  napi_get_array_length(aki::JSBind::GetScopedEnv(), array.GetHandle(),
                        &length);
  return length;
}

#endif  // OHOS_ADAPTER_COMMON_UTIL_H_
