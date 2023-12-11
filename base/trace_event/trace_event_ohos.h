// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef BASE_TRACE_EVENT_TRACE_EVENT_OHOS_H
#define BASE_TRACE_EVENT_TRACE_EVENT_OHOS_H
 
#include <string>
#include <utility>
 
union BytraceArgValue {
  double as_double;
  const char* as_string;
};
 
enum BytraceArgType {
  TYPE_NUMBER,
  TYPE_STRING,
  TYPE_INVALID,
};
 
struct BytraceArg {
  BytraceArgValue value;
  BytraceArgType type;
};
 
bool IsBytraceEnable();
BytraceArg GetArg(double i);
BytraceArg GetArg(const char* i);
std::string GetStringFromArg(const BytraceArg& arg);
std::string GetStringWithArgs(const std::string& name);
 
template <class ARG1_TYPE>
std::string GetStringWithArgs(const std::string& name,
                              const char* arg1_name,
                              ARG1_TYPE&& arg1_val) {
  if (IsBytraceEnable()) {
    BytraceArg arg1 = GetArg(std::forward<ARG1_TYPE>(arg1_val));
    return name + " | " + arg1_name + "=" + GetStringFromArg(arg1);
  }
  return "";
}
 
template <class ARG1_TYPE, class ARG2_TYPE>
std::string GetStringWithArgs(const std::string& name,
                              const char* arg1_name,
                              ARG1_TYPE&& arg1_val,
                              const char* arg2_name,
                              ARG2_TYPE&& arg2_val) {
  if (IsBytraceEnable()) {
    BytraceArg arg1 = GetArg(std::forward<ARG1_TYPE>(arg1_val));
    BytraceArg arg2 = GetArg(std::forward<ARG2_TYPE>(arg2_val));
    return name + " | " + arg1_name + "=" + GetStringFromArg(arg1) + " | " +
           arg2_name + "=" + GetStringFromArg(arg2);
  }
  return "";
}
  
void StartBytrace(const std::string& value);
void FinishBytrace();
void StartAsyncBytrace(const std::string& value, int32_t taskId);
void FinishAsyncBytrace(const std::string& value, int32_t taskId);
void CountBytrace(const std::string& name, int64_t count);
 
class ScopedBytrace {
 public:
  ScopedBytrace(const std::string& proc);
  ScopedBytrace();
  ~ScopedBytrace();
  static void SendTraceEvent(const std::string& data);
 private:
  std::string proc_;
};

#define OHOS_BY_TRACE_CONNENCT(a, b) a##b
#define OHOS_BY_TRACE_NAME2(a, b) OHOS_BY_TRACE_CONNENCT(a, b)
#define OHOS_BY_TRACE_NAME(a) OHOS_BY_TRACE_NAME2(a, __LINE__)
 
#define BYTRACE_SCOPED_INIT()  \
    ScopedBytrace OHOS_BY_TRACE_NAME(bytrace)

#define BYTRACE_SCOPED_TRACE_EVENT(name)  \
    ScopedBytrace::SendTraceEvent(name)
 
#endif  // BASE_TRACE_EVENT_TRACE_EVENT_OHOS_H_
