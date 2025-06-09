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

#ifndef OHOS_ADAPTER_RES_SCHED_MANAGER_RES_API_WRAPPER_H_
#define OHOS_ADAPTER_RES_SCHED_MANAGER_RES_API_WRAPPER_H_

#include <cstdint>
#include <string>
#include <unordered_map>

#include "ohos/adapter/common/shared_library.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter::res_sched::manager {
/**
 * ResApiWrapper wrap the process of get inner res_sched api
 * and process of invoke api.
 */
class ADAPTER_EXPORT_API ResApiWrapper {
 public:
  static ResApiWrapper& GetInstance();
  /** not copyable and transferable */
  ResApiWrapper(const ResApiWrapper&) = delete;
  /** call before `invokeApi` to avoid construct payload parameter */
  bool isValid() const { return fn; }
  /**
   * invoke the inner res_sched api to change schedule
   * @param resType the inner api category number
   * @param param extra parameter according to resType
   * @param payload extra payload, dist that key value are both string
   */
  bool invokeApi(uint32_t resType,
                 int64_t param,
                 std::unordered_map<std::string, std::string>& payload);

 private:
  /**
   * init the wrapper api in constructor
   */
  ResApiWrapper();
  ~ResApiWrapper() = default;
  /** dll handle ptr */
  void* handle = nullptr;
  using ReportFunc =
      void (*)(uint32_t,
               int64_t,
               const std::unordered_map<std::string, std::string>&);
  /** pointer to inner api */
  ReportFunc fn = nullptr;
  /** the client dll */
  common::SharedLibrary ressched_client_dll_;
  std::atomic<bool> log_first_empty = false;
};
}  // namespace ohos::adapter::res_sched::manager

#endif  // OHOS_ADAPTER_RES_SCHED_MANAGER_RES_API_WRAPPER_H_
