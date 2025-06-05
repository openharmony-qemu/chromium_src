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

#include <memory>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/web_entry/chrome/chrome_loader.h"
#include "ohos/adapter/web_entry/shell/content_shell_loader.h"
#include "ohos/adapter/web_entry/web_loader.h"

namespace ohos::adapter::web_entry {

namespace {

static std::unique_ptr<BasicLoader> current_default_loader = nullptr;
const std::string kShellEntry = "content_shell_web";
const std::string kChromeEntry = "chrome_main_web";

std::unique_ptr<BasicLoader> GetCurrentDefaultLoader(int loaderType) {
  std::unique_ptr<BasicLoader> loader;
  switch (loaderType) {
    case WebLoaderType::kContent_shell:
      loader = std::make_unique<ContentShellLoader>("", kShellEntry);
      break;
    case WebLoaderType::kChrome:
      loader = std::make_unique<ChromeMainLoader>("", kChromeEntry);
      break;
    default:
      LOGE("create web loader error unknown loaderType: %{public}d", loaderType);
      break;
  }
  return loader && loader->Initialize() ? std::move(loader) : nullptr;
}

}  // namespace

// static
BasicLoader* BasicLoader::Create(int loaderType) {
  if (!current_default_loader) {
    current_default_loader = GetCurrentDefaultLoader(loaderType);
  }
  return current_default_loader.get();
}

// static
bool HasCurrentDefaultLoader() {
  return current_default_loader != nullptr;
}

// static
void ResetLoader() {
  current_default_loader.reset();
}

}  // namespace ohos::adapter::web_entry
