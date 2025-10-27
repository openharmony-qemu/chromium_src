/* Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef CONTENT_BROWSER_LOG_CONSOLE_MESSAGE_H_UTILS_H_
#define CONTENT_BROWSER_LOG_CONSOLE_MESSAGE_H_UTILS_H_

#include <string>

#include "base/logging.h"
#include "third_party/blink/public/mojom/devtools/console_message.mojom.h"
namespace content {
class LogConsoleMessageUtils {
public:
  LogConsoleMessageUtils() = default;
  LogConsoleMessageUtils(const LogConsoleMessageUtils&) = delete;
  LogConsoleMessageUtils& operator=(const LogConsoleMessageUtils&) = delete;

  ~LogConsoleMessageUtils();
// Optionally logs a message from the console, depending on the set logging
// levels and incognito state.
static void LogConsoleMessage(int32_t resolved_level, const std::u16string& message,
    int32_t line_number, const std::u16string& source_id);
};
}  // namespace content
#endif  // CONTENT_BROWSER_LOG_CONSOLE_MESSAGE_H_UTILS_H_
