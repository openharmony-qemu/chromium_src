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

#ifndef OHOS_ADAPTER_WEB_ENTRY_WEB_LOADER_H_
#define OHOS_ADAPTER_WEB_ENTRY_WEB_LOADER_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/shared_library.h"

namespace ohos::adapter::web_entry {

using OnLoaderCallback = std::function<void(int error_code)>;

enum WebLoaderType {
  kContent_shell = 0,
  kChrome,
};

class BasicLoader {
 public:
  BasicLoader(const BasicLoader&) = delete;
  BasicLoader& operator=(const BasicLoader&) = delete;
  BasicLoader(const std::string& dir, const std::string& name);

  virtual ~BasicLoader() {}

  // A factory method that can create any of the derived types of BasicLoader
  // (ContentShellLoader and ChromeMainLoader ).
  static BasicLoader* Create(int loaderType);

  bool Initialize();

  int StartMainProcess(const std::vector<std::string>& args);
  int StartChildProcess(int process_type, int argc, const char** argv);

  virtual std::string GetEntryPoint(int process_type) = 0;
  virtual const std::vector<std::string> GetEntryArgs() = 0;

 protected:
  common::SharedLibrary shared_library_;
};
}  // namespace ohos::adapter::web_entry
#endif  // OHOS_ADAPTER_WEB_ENTRY_WEB_LOADER_H_
