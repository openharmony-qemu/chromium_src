/* Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "content/browser/log_console_message.h"
#include <codecvt>

#include "arkweb/chromium_ext/content/browser/log_console_message_utils.h"
#include "arkweb/build/features/features.h"
#include "base/feature_list.h"
#include "base/logging.h"
#include "build/build_config.h"
#include "content/public/browser/console_message.h"
#include "content/public/common/content_features.h"
#if BUILDFLAG(ARKWEB_DFX_LOGGING)
#include "hilog_adapter.h"
#endif

namespace content {

void LogConsoleMessageUtils::LogConsoleMessage(int32_t resolved_level, const std::u16string& message,
    int32_t line_number, const std::u16string& source_id)
{
    auto priority = (resolved_level < 0) ? OHOS::NWeb::LogLevelAdapter::DEBUG
                                       : OHOS::NWeb::LogLevelAdapter::LEVEL_MAX;
    switch (resolved_level) {
        case logging::LOGGING_INFO:
            priority = OHOS::NWeb::LogLevelAdapter::INFO;
            break;
        case logging::LOGGING_WARNING:
            priority = OHOS::NWeb::LogLevelAdapter::WARN;
            break;
        case logging::LOGGING_ERROR:
            priority = OHOS::NWeb::LogLevelAdapter::ERROR;
            break;
        case logging::LOGGING_FATAL:
            priority = OHOS::NWeb::LogLevelAdapter::FATAL;
            break;
        case logging::LOGGING_DEBUG:
            priority = OHOS::NWeb::LogLevelAdapter::DEBUG;
  }
  std::ostringstream stream;
  stream << "\"" << message << "\", source: " << source_id << " (" << line_number << ")";
  std::string message_str(stream.str());
  OHOS::NWeb::HiLogAdapter::PrintConsoleLog(priority, "ARKWEB-CONSOLE", "[%{public}s:%{public}d] %{public}s",
                                            "CONSOLE", line_number, message_str.c_str());
}
}