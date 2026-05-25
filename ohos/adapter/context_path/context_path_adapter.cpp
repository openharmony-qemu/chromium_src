// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/context_path/context_path_adapter.h"

#include <functional>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::ContextPathAdapter {
namespace {

class ContextPathConfig {
 public:
  static ContextPathConfig& GetInstance() {
    static ContextPathConfig instance;
    return instance;
  }

  void SetContextPaths(const ContextPaths& paths) {
    cache_dir_ = paths.cache_dir;
    files_dir_ = paths.files_dir;
    temp_dir_ = paths.temp_dir;
    user_document_dir_ = paths.user_document_dir;
    user_desktop_dir_ = paths.user_desktop_dir;
    user_download_dir_ = paths.user_download_dir;
  }

  std::string GetAppDownloadDir() {
    if (app_download_dir_.empty()) {
      app_download_dir_ = GetAppDownloadDirFromJS();
    }
    return app_download_dir_;
  }

  const std::string& cache_dir() const { return cache_dir_; }
  const std::string& files_dir() const { return files_dir_; }
  const std::string& temp_dir() const { return temp_dir_; }
  const std::string& user_document_dir() const { return user_document_dir_; }
  const std::string& user_desktop_dir() const { return user_desktop_dir_; }
  const std::string& user_download_dir() const { return user_download_dir_; }

 private:
  ContextPathConfig() = default;
  ContextPathConfig(const ContextPathConfig&) = delete;
  ContextPathConfig& operator=(const ContextPathConfig&) = delete;

  static std::string GetAppDownloadDirFromJS() {
    if (auto getDirFunc = ohos::adapter::GetJSFunction(
            "ContextPathAdapter.GetAppDownloadDir")) {
      auto promise = std::make_shared<std::promise<std::string>>();
      auto future = promise->get_future();
      std::function<void(std::string)> callback = [promise](std::string ret) {
        promise->set_value(ret);
      };
      getDirFunc->Invoke<void>(callback);
      auto status = future.wait_for(std::chrono::seconds(3));
      if (status == std::future_status::timeout) {
        LOGE("[OhosContextPath] GetAppDownloadDir timed out.");
        return "";
      }
      return future.get();
    }
    LOGE("[OhosContextPath] GetAppDownloadDir JS function not found.");
    return "";
  }

  std::string cache_dir_;
  std::string files_dir_;
  std::string temp_dir_;
  std::string user_document_dir_;
  std::string user_desktop_dir_;
  std::string user_download_dir_;
  std::string app_download_dir_;
};
}  // namespace

void SetContextPaths(const ContextPaths& paths) {
  ContextPathConfig::GetInstance().SetContextPaths(paths);
}

std::string GetCacheDir() {
  return ContextPathConfig::GetInstance().cache_dir();
}

std::string GetFilesDir() {
  return ContextPathConfig::GetInstance().files_dir();
}

std::string GetTempDir() {
  return ContextPathConfig::GetInstance().temp_dir();
}

std::string GetUserDocumentDir() {
  return ContextPathConfig::GetInstance().user_document_dir();
}

std::string GetUserDesktopDir() {
  return ContextPathConfig::GetInstance().user_desktop_dir();
}

std::string GetUserDownloadDir() {
  return ContextPathConfig::GetInstance().user_download_dir();
}

std::string GetAppDownloadDir() {
  return ContextPathConfig::GetInstance().GetAppDownloadDir();
}
}  // namespace ohos::adapter::ContextPathAdapter
