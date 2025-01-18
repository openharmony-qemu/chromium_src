/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.
 */

#include "content/renderer/logger_report.h"
#include "base/logging.h"
#include "content/renderer/render_thread_impl.h"
#include "base/ohos/logger.h"

namespace content {

// static
void EventLog::ReportRendererLog(int policy, const std::string& event_msg) {
  RenderThreadImpl* render_thread_impl = RenderThreadImpl::current();
  if (!render_thread_impl) {
    return;
  }

  mojom::RendererHost* renderer_host = render_thread_impl->GetRendererHost();
  if (!renderer_host) {
    return;
  }

  renderer_host->ReportRendererLog(policy, event_msg);
}

}