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

#ifndef NWEB_CONSOLE_LOG_IMPL_H
#define NWEB_CONSOLE_LOG_IMPL_H

#include "nweb_console_log.h"

namespace OHOS::NWeb {

class NWebConsoleLogImpl : public NWebConsoleLog {
 public:
  NWebConsoleLogImpl(int line_number,
                     std::string message,
                     NWebConsoleLogLevel log_level,
                     NWebConsoleLogSource log_source,
                     std::string sourceId);
  ~NWebConsoleLogImpl() = default;

  /**
   * @brief Get console log line number
   *
   * @retval line number
   */
  int LineNumer() override;

  /**
   * @brief Get console log message
   *
   * @retval message
   */
  std::string Log() override;

  /**
   * @brief Get console log message level
   *
   * @retval message level
   */
  NWebConsoleLog::NWebConsoleLogLevel LogLevel() override;

  /**
   * @brief Get console log source id
   *
   * @retval source id
   */
  std::string SourceId() override;

  /**
   * @brief Get console log source
   *
   * @retval source
   */
  NWebConsoleLog::NWebConsoleLogSource Source() override;

 private:
  int line_number_ = 0;
  std::string log_;
  NWebConsoleLog::NWebConsoleLogLevel log_level_ =
      NWebConsoleLog::NWebConsoleLogLevel::UNKNOWN;
  NWebConsoleLog::NWebConsoleLogSource log_source_ =
      NWebConsoleLog::NWebConsoleLogSource::OTHER;
  std::string sourceId_;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_CONSOLE_LOG_IMPL_H
