// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_MULTIPROCESS_COMMAND_LINE_COMMAND_LINE_HELPER_H_
#define OHOS_ADAPTER_MULTIPROCESS_COMMAND_LINE_COMMAND_LINE_HELPER_H_

#include <string>
#include <vector>

namespace ohos::adapter::multiprocess {
class CommandLineHelper {
 public:
  static std::string BuildCommands(
      const std::vector<std::string>& commands,
      const std::vector<std::string>& extra_commands = {});

  static std::vector<std::string> Split(const std::string& src);

  static std::string BuildSharedFDCommand(
      const std::vector<std::pair<int32_t, int32_t>>& fds);
};
}  // namespace ohos::adapter::multiprocess
#endif  // OHOS_ADAPTER_MULTIPROCESS_COMMAND_LINE_COMMAND_LINE_HELPER_H_
