/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_PIP_WINDOW_MANAGER_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_PIP_WINDOW_MANAGER_H_

#include "base/containers/flat_map.h"
#include "base/synchronization/lock.h"
#include "ui/ozone/platform/ohos/host/ohos_pip_window.h"

namespace ui {

class OhosPipWindowManager {
 public:
  static OhosPipWindowManager& GetInstance();

  void AddPipWindow(uint32_t controller_id, base::WeakPtr<OhosPipWindow> pip_window);

  base::WeakPtr<OhosPipWindow> GetPipWindowById(uint32_t controller_id);

  void RemovePipWindow(uint32_t controller_id);
  size_t GetPipWindowMapSize();

  OhosPipWindowManager(const OhosPipWindowManager&) = delete;
  OhosPipWindowManager& operator=(const OhosPipWindowManager&) = delete;

 private:
  OhosPipWindowManager();
  ~OhosPipWindowManager();

  base::flat_map<uint32_t, base::WeakPtr<OhosPipWindow>> pip_window_map_;
  base::Lock lock_;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_PIP_WINDOW_MANAGER_H_
