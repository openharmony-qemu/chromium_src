/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "content/renderer/renderer_engine_event.h"

#include "base/logging.h"
#include "content/renderer/render_thread_impl.h"

namespace content {

// static
void EventLog::ReportEngineEvent(const std::string& module,
                                 const std::string& resource,
                                 const std::string& error_code,
                                 const std::string& error_msg) {
  RenderThreadImpl* render_thread_impl = RenderThreadImpl::current();
  if (render_thread_impl == nullptr) {
    LOG(ERROR) << "Get current render thread impl failed.";
    return;
  }

  mojom::RendererHost *renderer_host = render_thread_impl->GetRendererHost();
  if (renderer_host == nullptr) {
    LOG(ERROR) << "Get current render thread's render host failed.";
    return;
  }

  renderer_host->ReportEngineEvent(module, resource, error_code, error_msg);
}

}  // namespace content