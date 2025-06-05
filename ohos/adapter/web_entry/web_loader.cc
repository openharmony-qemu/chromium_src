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

#include "ohos/adapter/web_entry/web_loader.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::web_entry {

BasicLoader::BasicLoader(const std::string& dir, const std::string& name)
    : shared_library_(dir, name) {}

bool BasicLoader::Initialize() { return true; }

__attribute__((no_sanitize("cfi", "cfi-icall")))
int BasicLoader::StartMainProcess(const std::vector<std::string>& args) {
  if (!shared_library_.IsLoaded()) {
    LOGE("StartMainProcess shared_library_ load failed.");
    return -1;
  }

  auto entry = GetEntryPoint(ProcessType::kMainProcess);
  if (entry.empty()) {
    LOGE("Get Entry Point failed.");
    return -1;
  }
  auto fn = shared_library_.GetFunction<int(int, const char**)>(entry.c_str());
  if (fn != nullptr) {
    auto argv_origin = GetEntryArgs();
    std::vector<const char*> argv_cstr;
    for (const auto& arg : argv_origin) {
      argv_cstr.push_back(const_cast<char*>(arg.c_str()));
    }
    for (const auto& arg : args) {
      argv_cstr.push_back(const_cast<char*>(arg.c_str()));
    }
    return fn(argv_cstr.size(), argv_cstr.data());
  }
  LOGE("Start Main Process failed.");
  return -1;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int BasicLoader::StartChildProcess(int process_type,
                                   int argc, const char** argv) {
  if (!shared_library_.IsLoaded()) {
    LOGE("StartChildProcess shared_library_ load failed.");
    return -1;
  }

  auto entry = GetEntryPoint(process_type);
  if (entry.empty()) {
    LOGE("Get Entry Point failed.");
    return -1;
  }

  auto fn = shared_library_.GetFunction<int(int, const char**)>(entry.c_str());
  if (fn != nullptr) {
    return fn(argc, argv);
  }
  LOGE("Run Child Process failed.");
  return -1;
}

}  // namespace ohos::adapter::web_entry
