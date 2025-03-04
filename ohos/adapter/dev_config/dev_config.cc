// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "dev_config.h"

#include <json/json.h>

#include <cstdio>
#include <fstream>
#include <memory>
#include <string>

#include "ohos/adapter/tracing/tracing_adapter.h"

namespace ohos {
namespace adapter {

const char kDevConfigPath[] =
    "/data/storage/el2/base/preferences/dev_config.json";

const char kSwitchPrefix[] = "--";
const char kSwitchSeparator[] = "=";
const char kDevConfigTraceStartup[] = "trace-startup";
const char kDevConfigTraceStartupFile[] = "trace-startup-file";
const char kDevConfigTraceStartupDuration[] = "trace-startup-duration";

const char kDevConfigRemoteDebugging[] = "remote-debugging";
const char kDevConfigRemoteDebuggingPort[] = "remote-debugging-port";
const char kDevConfigRemoteAllowOrigins[] = "remote-allow-origins";

struct FileDeleter {
  void operator()(FILE* fp) const {
    if (fp) {
      fclose(fp);
    }
  }
};

using ScopeFilePtr = std::unique_ptr<FILE, FileDeleter>;

DevConfig GetDevConfig(const std::string& path) {
  std::ifstream file;
  if (path.empty()) {
    file.open(kDevConfigPath, std::ifstream::binary);
  } else {
    file.open(path, std::ifstream::binary);
  }

  DevConfig config;
  if (!file.is_open()) {
    return config;
  }

  Json::Value document;
  Json::CharReaderBuilder readerBuilder;
  std::string errs;
  if (!Json::parseFromStream(readerBuilder, file, &document, &errs)) {
    return config;
  }

  if (document.isMember(kDevConfigTraceStartup) &&
      document[kDevConfigTraceStartup].isBool()) {
    config.trace_startup = document[kDevConfigTraceStartup].asBool();
  }

  if (document.isMember(kDevConfigTraceStartupFile) &&
      document[kDevConfigTraceStartupFile].isString()) {
    config.trace_startup_file = document[kDevConfigTraceStartupFile].asString();
  }

  if (document.isMember(kDevConfigTraceStartupDuration) &&
      document[kDevConfigTraceStartupDuration].isInt()) {
    config.trace_startup_duration =
        document[kDevConfigTraceStartupDuration].asInt();
  }

  if (document.isMember(kDevConfigRemoteDebugging) &&
      document[kDevConfigRemoteDebugging].isBool()) {
    config.remote_debug = document[kDevConfigRemoteDebugging].asBool();
  }

  if (document.isMember(kDevConfigRemoteDebuggingPort) &&
      document[kDevConfigRemoteDebuggingPort].isInt()) {
    config.remote_debugging_port =
        document[kDevConfigRemoteDebuggingPort].asInt();
  }

  if (document.isMember(kDevConfigRemoteAllowOrigins) &&
      document[kDevConfigRemoteAllowOrigins].isString()) {
    config.remote_allow_origins =
        document[kDevConfigRemoteAllowOrigins].asString();
  }

  return config;
}

std::vector<std::string> GetDevCommandLines() {
  std::vector<std::string> commandlines;
  DevConfig config = GetDevConfig("");
  const int max_port = 65535;
  if (config.remote_debug && config.remote_debugging_port > 0 &&
      config.remote_debugging_port < max_port) {
    std::string remote_debug_command =
        std::string(kSwitchPrefix) + kDevConfigRemoteDebuggingPort +
        kSwitchSeparator + std::to_string(config.remote_debugging_port);
    commandlines.emplace_back(remote_debug_command);
  }

  if (config.remote_debug && !config.remote_allow_origins.empty()) {
    std::string remote_allow_origins_command =
        std::string(kSwitchPrefix) + kDevConfigRemoteAllowOrigins +
        kSwitchSeparator + config.remote_allow_origins;
    commandlines.emplace_back(remote_allow_origins_command);
  }

  if (config.trace_startup) {
    commandlines.emplace_back(std::string(kSwitchPrefix) +
                              kDevConfigTraceStartup);
    tracing::TracingAdapter::GetInstance().EnableAdapterTrace();
  }

  if (config.trace_startup_duration > 0) {
    std::string startup_duration_command =
        std::string(kSwitchPrefix) + kDevConfigTraceStartupDuration +
        kSwitchSeparator + std::to_string(config.trace_startup_duration);
    commandlines.emplace_back(startup_duration_command);
  }

  if (!config.trace_startup_file.empty()) {
    std::string startup_file_command =
        std::string(kSwitchPrefix) + kDevConfigTraceStartupFile +
        kSwitchSeparator + config.trace_startup_file;
    commandlines.emplace_back(startup_file_command);
  }
  return commandlines;
}

}  // namespace adapter
}  // namespace ohos
