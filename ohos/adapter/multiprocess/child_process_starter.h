// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_CHILD_PROCESS_STARTER_H
#define OHOS_ADAPTER_CHILD_PROCESS_STARTER_H

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "ohos/adapter/export.h"

namespace ohos::adapter::multiprocess {

class ADAPTER_EXPORT_API ChildProcessStarter {
 public:
  using StartCallbackFunc = std::function<void(int32_t)>;

  static ChildProcessStarter& GetInstance();
  ChildProcessStarter() = default;

  int StartChildProcess(const std::vector<std::string>& command,
                        const std::vector<std::pair<int, int>>& fds);

  int StartIsolateChildProcess(const std::vector<std::string>& command,
                               const std::vector<std::pair<int, int>>& fds);

  int StartGpuProcess(const std::vector<std::string>& command,
                      const std::vector<std::pair<int, int>>& fds);

 private:
  int StartChildProcess(std::function<void(StartCallbackFunc)> startFunc,
                        const std::vector<std::string>& command,
                        const std::vector<std::pair<int, int>>& fds);
};

}  // namespace ohos::adapter::multiprocess

#endif  // OHOS_ADAPTER_CHILD_PROCESS_STARTER_H
