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

#ifndef OHOS_ADAPTER_DEV_CONFIG_DEV_CONFIG_H_
#define OHOS_ADAPTER_DEV_CONFIG_DEV_CONFIG_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "ohos/adapter/export.h"

namespace ohos {
namespace adapter {

/*
 * `dev_config.json` is a configuration file used for developer to set
 * chrome flags in a limited subset, since OHOS does not provide a
 * convenient way to pass flags through args in startup.
 *
 * config example:
 * ```json
 * {
 *   "trace-startup" : true,
 *   "trace-categories":
 * "benchmark,blink,v8,cc,gpu,navigation,toplevel,viz,ui,views",
 *   "trace-startup-file"
 *     : "/data/storage/el2/base/cache/trace.json",
 *   "trace-startup-duration": 15,
 *   "trace-startup-record-mode": "record-until-full",
 *   "remote-debugging": true,
 *   "remote-debugging-port": 9222,
 *   "remote-allow-origins": "http://localhost:9222",
 *   "disable-occlusion-feature": false
 * }
 * ```
 */

class DevConfig {
 public:
  /** if true enable trace on startup */
  bool trace_startup = false;
  /**
   * trace categories, if specified, will be used instead of default categories.
   * the format is `[a-z\._]+(,[a-z\._]+)*`.
   * will be read if `trace_startup` or `long_tracing` is `true`.
   */
  std::string trace_categories;
  /**
   * file path in OHOS sandbox relative path format,
   * indicate the location of trace file.
   * The file will be overritten in next time start up.
   */
  std::string trace_startup_file;
  /** trace duration after start up in seconds. */
  int trace_startup_duration = 0;
  /**
   * see `--trace-startup-record-mode` flag, if specified, should be one of
   * three value: `"record-until-full"`, `"record-continuously"`,
   * `"record-as-much-as-possible"`, if not present or not in range will use
   * default `"record-until-full"`
   */
  std::string trace_startup_record_mode;
  /** if true enable remote debug */
  bool remote_debug = false;
  /**
   * remote debugging port opened locally, can use `hdc fport` to forward back
   * to development machine.
   */
  int32_t remote_debugging_port = 9222;
  /**
   * see chrome flag `--remote-allow-origins`, allow to be shared outside local
   * network.
   */
  std::string remote_allow_origins;
  /** if present and set true, will enter long tracing mode, will override
   * `trace_startup` config. */
  bool long_tracing = false;
  /**
   * if true on window visibility change don't trigger Chromium window hide
   * feature
   */
  bool disable_occlusion_feature = false;
  bool disable_partial_swap = false;
  bool disable_page_policy = false;
};

/**
 * open a dialog to request picker and pass the result through callback
 * @param callback a callback function accept a string of json array with 1
 * string element containing the absolute path in app sandbox
 */
void ADAPTER_EXPORT_API
RequestDevConfigDialog(std::function<void(const std::string&)> callback);

/**
 * read a devconfig instance by reading `dev_config.json` file
 * @param path the path of dev config file
 * @return if file not found, return `nullptr` unique_ptr otherwise return a
 * valid dev config file
 */
std::unique_ptr<DevConfig> ADAPTER_EXPORT_API
GetDevConfigFromPath(const std::string& path);

/**
 * Init a DevConfig instance by reading `dev_config.json` file,
 * the path is first `/data/storage/el2/base/preferences/dev_config.json`,
 * then `/data/storage/el1/bundle/libs/arm64/dev_config.json` or a default empty DevConfig.
 * the pointer cannot be null
 */
std::unique_ptr<DevConfig> GetDevConfig();

/**
 * Transform a DevConfig instance into chromium command lines stored in vector
 * of string, no side effects are performed.
 * @param config the DevConfig instance to be parsed
 * @return the command lines to append to startup file
 */
std::vector<std::string> DevConfigToCommandLines(const DevConfig& config);

/**
 * enable adapter by read `DevConfig`
 */
void EnableAdapterFromConfig(const DevConfig& config);

/** read `dev_config.json` file in default location and transform
 * `dev_config.json` configuration into extra chrome command line args, and
 * enable adapters
 * @return vector of string storing extra command line args, each is a chrome
 * flag. Empty vector if file not exist or file format invalid.
 */
std::vector<std::string> GetDevCommandLines();

}  // namespace adapter
}  // namespace ohos
#endif  // OHOS_ADAPTER_DEV_CONFIG_DEV_CONFIG_H_
