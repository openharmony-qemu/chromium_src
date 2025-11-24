/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TRACE_EVENT_OH_H
#define TRACE_EVENT_OH_H

template <class ARG1_TYPE>
std::string GetStringFromArgs(const char* name,
                              const char* arg1_name,
                              ARG1_TYPE&& arg1_val) {
  std::string str(name);
  base::trace_event::TraceArguments args(arg1_name,
                                         std::forward<ARG1_TYPE>(arg1_val));
  str += " | ";
  str += arg1_name;
  str += "=";
  args.values()[0].AppendAsString(args.types()[0], &str);
  return str;
}

template <class ARG1_TYPE, class ARG2_TYPE>
std::string GetStringFromArgs(const char* name,
                              const char* arg1_name,
                              ARG1_TYPE&& arg1_val,
                              const char* arg2_name,
                              ARG2_TYPE&& arg2_val) {
  std::string str(name);
  base::trace_event::TraceArguments args(
      arg1_name, std::forward<ARG1_TYPE>(arg1_val), arg2_name,
      std::forward<ARG2_TYPE>(arg2_val));
  str += " | ";
  str += arg1_name;
  str += "=";
  args.values()[0].AppendAsString(args.types()[0], &str);
  str += " | ";
  str += arg2_name;
  str += "=";
  args.values()[1].AppendAsString(args.types()[1], &str);
  return str;
}
#endif  // TRACE_EVENT_OH_H
