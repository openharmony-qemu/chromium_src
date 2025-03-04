// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/web_entry/chrome/chrome_loader.h"
#include "ohos/adapter/web_entry/shell/content_shell_loader.h"
#include "ohos/adapter/web_entry/web_loader.h"

namespace ohos::adapter::web_entry {

namespace {

static std::unique_ptr<BasicLoader> current_default_loader = nullptr;
const std::string SHELL_ENTRY = "content_shell_web";
const std::string CHROME_ENTRY = "chrome_main_web";

std::unique_ptr<BasicLoader> GetCurrentDefaultLoader(int loaderType) {
  std::unique_ptr<BasicLoader> loader;
  switch (loaderType) {
    case WebLoaderType::kContent_shell:
      loader = std::make_unique<ContentShellLoader>("", SHELL_ENTRY);
      break;
    case WebLoaderType::kChrome:
      loader = std::make_unique<ChromeMainLoader>("", CHROME_ENTRY);
      break;
    default:
      LOGE("create web loader error unknown loaderType: %{public}d",
           loaderType);
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
