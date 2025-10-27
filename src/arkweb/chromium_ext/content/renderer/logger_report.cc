/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "content/renderer/logger_report.h"

#include "base/logging.h"
#include "base/ohos/logger.h"
#include "content/renderer/render_thread_impl.h"

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

}  // namespace content