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

#ifndef NWEB_CONSOLE_LOG_H
#define NWEB_CONSOLE_LOG_H

#include <memory>
#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebConsoleLog {
public:
    enum NWebConsoleLogLevel {
        DEBUG = 1,
        INFO,
        WARNING,
        ERROR,
        UNKNOWN
    };

    NWebConsoleLog(int line_number,
                   std::string message,
                   NWebConsoleLogLevel log_level,
                   std::string sourceId)
        : line_number_(line_number),
          log_(message),
          log_level_(log_level),
          sourceId_(sourceId) {}

    ~NWebConsoleLog() = default;

    /**
     * @brief Get console log line number
     *
     * @retval line number
     */
    int LineNumer() const {
        return line_number_;
    }

    /**
     * @brief Get console log message
     *
     * @retval message
     */
    const std::string& Log() const {
        return log_;
    }

    /**
     * @brief Get console log message level
     *
     * @retval message level
     */
    NWebConsoleLogLevel LogLevel() const {
        return log_level_;
    }

    /**
     * @brief Get console log source id
     *
     * @retval source id
     */
    const std::string& SourceId() const {
        return sourceId_;
    }

private:
    int line_number_;
    std::string log_;
    NWebConsoleLogLevel log_level_;
    std::string sourceId_;
};
}

#endif // NWEB_CONSOLE_LOG_H