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

#include "dev_config.h"
#include <json/json.h>
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>

#include "ohos/adapter/tracing/tracing_adapter.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

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
const char kDevConfigDisableOcclusionFeature[] = "disable-occlusion-feature";

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

  if (document.isMember(kDevConfigDisableOcclusionFeature) &&
      document[kDevConfigDisableOcclusionFeature].isBool()) {
    config.disable_occlusion_feature =
        document[kDevConfigDisableOcclusionFeature].asBool();
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

  if (config.disable_occlusion_feature) {
    commandlines.emplace_back(std::string(kSwitchPrefix) +
                              kDevConfigDisableOcclusionFeature);
    xcomponent::WindowAdapter::GetInstance().DisableOcclusionFeature();
  }

  return commandlines;
}

}  // namespace adapter
}  // namespace ohos
