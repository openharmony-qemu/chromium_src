/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nweb_console_log_impl.h"

namespace OHOS::NWeb {

NWebConsoleLogImpl::NWebConsoleLogImpl(int line_number, std::string message,
                                       NWebConsoleLogLevel log_level,
                                       std::string sourceId)
    : line_number_(line_number), log_(message), log_level_(log_level),
      sourceId_(sourceId) {
}

int NWebConsoleLogImpl::LineNumer() {
  return line_number_;
}

std::string NWebConsoleLogImpl::Log() {
  return log_;
}

NWebConsoleLog::NWebConsoleLogLevel NWebConsoleLogImpl::LogLevel() {
  return log_level_;
}

std::string NWebConsoleLogImpl::SourceId() {
  return sourceId_;
}

} // namespace OHOS::NWeb