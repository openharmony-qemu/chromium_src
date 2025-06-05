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

#include <string>
#include <vector>
namespace ohos {
namespace adapter {

/*
 * config example
 * {
 *   "trace-startup" : true,
 *   "trace-startup-file"
 *     : "/data/storage/el2/base/cache/trace.json",
 *   "trace-startup-duration": 15,
 *   "remote-debugging": true,
 *   "remote-debugging-port": 9222,
 *   "remote-allow-origins": "http://localhost:9222",
 *   "disable-occlusion-feature" : true
 * }
 */

class DevConfig {
 public:
  bool trace_startup = false;
  std::string trace_startup_file;
  int trace_startup_duration;
  bool remote_debug = false;
  int32_t remote_debugging_port = 9222;
  std::string remote_allow_origins;
  bool disable_occlusion_feature = false;
};

DevConfig GetDevConfig(const std::string& path);
std::vector<std::string> GetDevCommandLines();

}  // namespace adapter
}  // namespace ohos
#endif  // OHOS_ADAPTER_DEV_CONFIG_DEV_CONFIG_H_
