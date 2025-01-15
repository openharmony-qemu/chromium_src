/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.
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
}
#endif