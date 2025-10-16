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

#include <fstream>
#include <memory>
#include <string>

#include "ohos/adapter/file_picker/file_select_picker.h"
#include "ohos/adapter/res_sched/res_sched.h"
#include "ohos/adapter/tracing/tracing_adapter.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

namespace ohos {
namespace adapter {
using std::string_literals::operator""s;

/** the user preference dev_config.json file */
const std::string kDevConfigPath =
    "/data/storage/el1/bundle/libs/arm64/dev_config.json"s;

const std::string kSwitchPrefix = "--"s;
const std::string kSwitchSeparator = "="s;
const std::string kDevConfigTraceStartup = "trace-startup"s;
const std::string kDevConfigTraceStartupFile = "trace-startup-file"s;
const std::string kDevConfigTraceStartupDuration = "trace-startup-duration"s;
const std::string kDevConfigTraceCategories = "trace-categories"s;
const std::string kDevConfigTraceStartupRecordMode =
    "trace-startup-record-mode"s;

const std::string kDevConfigRemoteDebugging = "remote-debugging"s;
const std::string kDevConfigRemoteDebuggingPort = "remote-debugging-port"s;
const std::string kDevConfigRemoteAllowOrigins = "remote-allow-origins"s;
const std::string kDevConfigDisableOcclusionFeature =
    "disable-occlusion-feature"s;
const std::string kDevConfigDisablePartialSwap = "disable-partial-swap"s;
const std::string kDevConfigDisablePagePolicy = "disable-page-policy"s;
const std::string kDevConfigLongTracing = "long-tracing"s;
const std::string kOhosTrace = "ohos-trace"s;

const int max_port = 65535;

/**
 * append a string with `"--<switch_name>"` format to string vector
 */
void AppendSwitchFlag(std::vector<std::string>& target,
                      const std::string& switch_name) {
  std::string& new_str = target.emplace_back(kSwitchPrefix);
  new_str += switch_name;
}

/**
 * append a string with `"--<switch_name>=<switch_value>"` format to string
 * vector
 */
void AppendSwitchValue(std::vector<std::string>& target,
                       const std::string& switch_name,
                       const std::string& switch_value) {
  std::string& new_str = target.emplace_back(kSwitchPrefix);
  size_t reserve_size = kSwitchPrefix.size() + kSwitchSeparator.size() +
                        switch_name.size() + switch_value.size();
  new_str.reserve(reserve_size);
  new_str += switch_name;
  new_str += kSwitchSeparator;
  new_str += switch_value;
}

/**
 * if the obj has specific field `key` and its type is string, assign it to
 * `target`
 * @param target a valid string reference, if the key exist and is string,
 * assign it, otherwise keep untouched
 * @param obj the valid json value
 * @param key the key name to read
 */
void ReadJsonObjString(std::string& target,
                       const Json::Value& obj,
                       const std::string& key) {
  if (obj.isMember(key) && obj[key].isString()) {
    target = obj[key].asString();
  }
}

/**
 * if the obj has specific field `key` and its type is bool, assign it to
 * `target`
 * @param target a valid bool reference, if the key exist and is string, assign
 * it, otherwise keep untouched
 * @param obj the valid json value
 * @param key the key name to read
 */
void ReadJsonObjBool(bool& target,
                     const Json::Value& obj,
                     const std::string& key) {
  if (obj.isMember(key) && obj[key].isBool()) {
    target = obj[key].asBool();
  }
}

/**
 * if the obj has specific field `key` and its type is int, assign it to
 * `target`
 * @param target a valid int reference, if the key exist and is string, assign
 * it, otherwise keep untouched
 * @param obj the valid json value
 * @param key the key name to read
 */
void ReadJsonObjInt(int& target,
                    const Json::Value& obj,
                    const std::string& key) {
  if (obj.isMember(key) && obj[key].isInt()) {
    target = obj[key].asInt();
  }
}

/**
 * parse the JSON structure and write fields to `config`
 * @param document the Json document
 * @param config the config object to write
 */
void ReadDevConfigFromJson(const Json::Value& document, DevConfig& config) {
  if (!document.isObject()) {
    return;
  }
  ReadJsonObjBool(config.trace_startup, document, kDevConfigTraceStartup);
  ReadJsonObjString(config.trace_categories, document,
                    kDevConfigTraceCategories);
  ReadJsonObjString(config.trace_startup_file, document,
                    kDevConfigTraceStartupFile);
  ReadJsonObjInt(config.trace_startup_duration, document,
                 kDevConfigTraceStartupDuration);
  ReadJsonObjString(config.trace_startup_record_mode, document,
                    kDevConfigTraceStartupRecordMode);
  ReadJsonObjBool(config.remote_debug, document, kDevConfigRemoteDebugging);
  ReadJsonObjInt(config.remote_debugging_port, document,
                 kDevConfigRemoteDebuggingPort);
  ReadJsonObjString(config.remote_allow_origins, document,
                    kDevConfigRemoteAllowOrigins);
  ReadJsonObjBool(config.long_tracing, document, kDevConfigLongTracing);
  ReadJsonObjBool(config.disable_occlusion_feature, document,
                  kDevConfigDisableOcclusionFeature);
  ReadJsonObjBool(config.disable_partial_swap, document,
                  kDevConfigDisablePartialSwap);
  ReadJsonObjBool(config.disable_page_policy, document,
                  kDevConfigDisablePagePolicy);
}

std::vector<std::string> DevConfigToCommandLines(const DevConfig& config) {
  std::vector<std::string> commandlines;
  if (config.remote_debug) {
    if (config.remote_debugging_port > 0 &&
        config.remote_debugging_port < max_port) {
      std::string port_str = std::to_string(config.remote_debugging_port);
      AppendSwitchValue(commandlines, kDevConfigRemoteDebuggingPort, port_str);
    }

    if (!config.remote_allow_origins.empty()) {
      AppendSwitchValue(commandlines, kDevConfigRemoteAllowOrigins,
                        config.remote_allow_origins);
    }
  }

  // either enable long tracing mode or append startup arguments
  if (config.long_tracing) {
    // long tracing enable hitrace by passing switches to enable
    if (!config.trace_categories.empty()) {
      AppendSwitchValue(commandlines, kOhosTrace, config.trace_categories);
    } else {
      AppendSwitchFlag(commandlines, kOhosTrace);
    }
  } else if (config.trace_startup) {
    // if not long_tracing enabled, just enable the startup tracing and other
    // related configs
    if (!config.trace_categories.empty()) {
      AppendSwitchValue(commandlines, kDevConfigTraceStartup,
                        config.trace_categories);
    } else {
      AppendSwitchFlag(commandlines, kDevConfigTraceStartup);
    }
    // add extra configs
    if (!config.trace_startup_file.empty()) {
      AppendSwitchValue(commandlines, kDevConfigTraceStartupFile,
                        config.trace_startup_file);
    }
    if (config.trace_startup_duration > 0) {
      std::string duration = std::to_string(config.trace_startup_duration);
      AppendSwitchValue(commandlines, kDevConfigTraceStartupDuration, duration);
    }
    if (!config.trace_startup_record_mode.empty()) {
      AppendSwitchValue(commandlines, kDevConfigTraceStartupRecordMode,
                        config.trace_startup_record_mode);
    }
  }

  if (config.disable_occlusion_feature) {
    AppendSwitchFlag(commandlines, kDevConfigDisableOcclusionFeature);
  }
  if (config.disable_partial_swap) {
    AppendSwitchFlag(commandlines, kDevConfigDisablePartialSwap);
  }
  if (config.disable_page_policy) {
    AppendSwitchFlag(commandlines, kDevConfigDisablePagePolicy);
  }

  return commandlines;
}

void EnableAdapterFromConfig(const DevConfig& config) {
  // perform the side effects, init all adapter if required
  if (config.trace_startup) {
    // it is noop now
    tracing::TracingAdapter::GetInstance().EnableAdapterTrace();
  }
  if (config.disable_occlusion_feature) {
    xcomponent::WindowAdapter::GetInstance().DisableOcclusionFeature();
  }
}

/** control not concurrently call picker */
std::atomic<bool> is_trigger_picker{false};

/**
 * wrapper to make `is_trigger_picker` on invocation
 */
struct WrapCallback {
 public:
  std::function<void(const std::string&)> callback_;
  explicit WrapCallback(std::function<void(const std::string&)>&& callback)
      : callback_(callback) {}
  WrapCallback(WrapCallback&& other) : callback_(std::move(other.callback_)) {}
  // copy constructor just steal the origin `callback_` not copy it
  WrapCallback(const WrapCallback& other)
      : callback_(std::move(const_cast<WrapCallback&>(other).callback_)) {}
  WrapCallback& operator=(const WrapCallback&) = delete;
  WrapCallback& operator=(WrapCallback&&) = delete;
  void operator()(const std::string& str) const {
    is_trigger_picker.store(false);
    if (callback_) {
      callback_(str);
    }
  }
};

void RequestDevConfigDialog(std::function<void(const std::string&)> callback) {
  if (!callback) {
    return;
  }
  bool expect_value = false;
  // concurrent call of RequestDevConfigDialog just make second and later
  // callback invoke with empty string
  if (!is_trigger_picker.compare_exchange_weak(expect_value, true)) {
    callback("");
    return;
  }
  file_select_picker::SelectFileDialogParams param;
  param.multi_files = false;
  param.extensions.emplace_back(std::initializer_list<std::string>{"json"});
  param.descriptions.emplace_back("select dev_config.json file");
  param.include_all_files = false;
  file_select_picker::FileSelectPicker& picker =
      file_select_picker::FileSelectPicker::GetInstance();
  picker.ShowFilePickerDialog(param, WrapCallback(std::move(callback)));
}

std::unique_ptr<DevConfig> GetDevConfigFromPath(const std::string& path) {
  std::unique_ptr<DevConfig> result{nullptr};
  std::ifstream file;
  if (path.empty()) {
    return result;
  } else {
    file.open(path, std::ifstream::binary);
  }
  if (!file.is_open()) {
    return result;
  }
  Json::Value document;
  Json::CharReaderBuilder readerBuilder;
  std::string errs;
  if (!Json::parseFromStream(readerBuilder, file, &document, &errs)) {
    return result;
  }
  // then we init dev config and return the value
  result = std::make_unique<DevConfig>();
  ReadDevConfigFromJson(document, *result);
  return result;
}

std::unique_ptr<DevConfig> GetDevConfig() {
  std::unique_ptr<DevConfig> result = GetDevConfigFromPath(kDevConfigPath);
  if (result) {
    return result;
  }
  result = std::make_unique<DevConfig>();
  return result;
}

std::vector<std::string> GetDevCommandLines() {
  std::unique_ptr<DevConfig> config = GetDevConfig();
  EnableAdapterFromConfig(*config);
  return DevConfigToCommandLines(*config);
}

}  // namespace adapter
}  // namespace ohos
