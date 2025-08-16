/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef CONTENT_RENDERER_LOGGER_REPORT_H_
#define CONTENT_RENDERER_LOGGER_REPORT_H_

#include <string>

#include "content/common/content_export.h"

namespace content {

class CONTENT_EXPORT EventLog {
 public:
  // Report render log to ui.
  static void ReportRendererLog(int policy, const std::string& event_msg);
};
}  // namespace content
#endif