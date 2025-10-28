/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <unistd.h>

#include <sstream>

#include "include/cef_app.h"
#include "nweb_export.h"
#include "nweb_hilog.h"

extern "C" OHOS_NWEB_EXPORT void NWebRenderMain(const char* args) {
  WVLOG_I("NWebRenderMain start, sandbox pid=%{public}d", getpid());

  std::string args_str = args;
  std::stringstream args_ss(args_str);
  const char separator = '#';
  std::vector<std::string> argv_str;
  std::string arg_str;
  while (std::getline(args_ss, arg_str, separator)) {
    argv_str.push_back(arg_str);
  }
  std::vector<char*> argv_cstr;
  int argc = static_cast<int>(argv_str.size());
  argv_cstr.reserve(argc + 1);
  for (const auto& arg : argv_str) {
    argv_cstr.push_back(const_cast<char*>(arg.c_str()));
  }
  argv_cstr.push_back(nullptr);

  CefMainArgs main_args(argc, const_cast<char**>(argv_cstr.data()));
  (void)CefExecuteProcess(main_args, nullptr, nullptr);

  WVLOG_I("NWebRenderMain end, sandbox pid=%{public}d", getpid());
}
