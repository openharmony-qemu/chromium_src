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

#ifndef OHOS_ADAPTER_COMMON_TRACE_H_
#define OHOS_ADAPTER_COMMON_TRACE_H_

#include "ohos/adapter/tracing/tracing_adapter.h"

// used for scoped trace record
class AdapterScopedTracer {
 public:
  AdapterScopedTracer() = default;
  ~AdapterScopedTracer();

  void Initialize(bool enabled, std::string& arg_str);

 private:
  bool trace_enabled_ = false;
};

// used for trace arguments conversion
enum TraceArgType : uint8_t {
  TYPE_BOOL,
  TYPE_UINT,
  TYPE_INT,
  TYPE_DOUBLE,
  TYPE_STRING
};

union TraceArgValue {
  bool as_bool;
  unsigned long long as_uint;
  long long as_int;
  double as_double;
  const char* as_string;

  std::string ToString(TraceArgType type) const;
};

struct TraceArg {
  TraceArg(bool v);
  TraceArg(uint32_t v);
  TraceArg(int32_t v);
  TraceArg(double v);
  TraceArg(const std::string& v);

  TraceArgType type;
  TraceArgValue value;
};

// convert trace args to string params
inline std::string ConvertArgsToString(const std::string& name) {
  return name;
}

template <class ARG1_TYPE>
inline std::string ConvertArgsToString(const std::string& name,
                                       const char* arg1_name,
                                       ARG1_TYPE&& arg1_val) {
  TraceArg arg(std::forward<ARG1_TYPE>(arg1_val));
  return name + " | " + arg1_name + arg.value.ToString(arg.type);
}

template <class ARG1_TYPE, class ARG2_TYPE>
inline std::string ConvertArgsToString(const std::string& name,
                                       const char* arg1_name,
                                       ARG1_TYPE&& arg1_val,
                                       const char* arg2_name,
                                       ARG2_TYPE&& arg2_val) {
  TraceArg arg1(std::forward<ARG1_TYPE>(arg1_val));
  TraceArg arg2(std::forward<ARG2_TYPE>(arg2_val));
  return name + " | " + arg1_name + "=" + arg1.value.ToString(arg1.type) +
         " | " + arg2_name + "=" + arg2.value.ToString(arg2.type);
}

// make the trace tag name to unique
#define UNIQUE_TRACE_EVENT_UID3(a, b) trace_event_unique_##a##b
#define UNIQUE_TRACE_EVENT_UID2(a, b) UNIQUE_TRACE_EVENT_UID3(a, b)
#define UNIQUE_TRACE_EVENT_UID(name_prefix) \
  UNIQUE_TRACE_EVENT_UID2(name_prefix, __LINE__)

// scoped trace event, if not enabled, do not compute params and record
#define ADAPTER_INTERNAL_TRACE_EVENT_ADD_SCOPED(name, ...)                 \
  AdapterScopedTracer UNIQUE_TRACE_EVENT_UID(ohos_adapter);                \
  if (bool enabled = ohos::adapter::tracing::TracingAdapter::GetInstance() \
                         .IsAdapterTraceEnabled()) {                       \
    std::string arg_str = ConvertArgsToString(name, ##__VA_ARGS__);        \
    UNIQUE_TRACE_EVENT_UID(ohos_adapter).Initialize(enabled, arg_str);     \
  }

// public trace event API
#define TRACE_EVENT_0(name) ADAPTER_INTERNAL_TRACE_EVENT_ADD_SCOPED(name)

#define TRACE_EVENT_1(name, arg_name, arg_value) \
  ADAPTER_INTERNAL_TRACE_EVENT_ADD_SCOPED(name, arg_name, arg_value)

#define TRACE_EVENT_2(name, arg1_name, arg1_val, arg2_name, arg2_val) \
  ADAPTER_INTERNAL_TRACE_EVENT_ADD_SCOPED(name, arg1_name, arg1_val,  \
                                          arg2_name, arg2_val)

#endif  // OHOS_ADAPTER_COMMON_TRACE_H_
