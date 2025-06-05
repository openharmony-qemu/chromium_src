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

#include "ohos/adapter/res_sched/manager/res_api_wrapper.h"

#include <bundle/native_interface_bundle.h>
#include <dlfcn.h>

#include <type_traits>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/shared_library.h"

namespace ohos::adapter::res_sched::manager {

namespace {
/**
 * use an false initialize atomic bool to guard first time
 */
bool ForFirstTime(std::atomic<bool>& false_first) {
  bool false_value = false;
  return false_first.compare_exchange_weak(false_value, true);
}
}  // namespace

ResApiWrapper& ResApiWrapper::GetInstance() {
  static ResApiWrapper singleton;
  return singleton;
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) ResApiWrapper::ResApiWrapper()
    : ressched_client_dll_("/system/lib64/platformsdk/",
                           "libressched_client.z") {
  fn = reinterpret_cast<ReportFunc>(
      ressched_client_dll_.GetSymbol("ReportData"));
  if (!fn) {
    LOGE(
        "ResApiWrapper::ResApiWrapper get ReportData failed for dlsym "
        "ReportData function");
    return;
  }
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) bool ResApiWrapper::invokeApi(
    uint32_t resType,
    int64_t param,
    std::unordered_map<std::string, std::string>& payload) {
  if (!fn) {
    if (ForFirstTime(log_first_empty)) {
      LOGE("ResApiWrapper::ResApiWrapper fn empty");
    }
    return false;
  }
  fn(resType, param, payload);
  return true;
}
}  // namespace ohos::adapter::res_sched::manager
