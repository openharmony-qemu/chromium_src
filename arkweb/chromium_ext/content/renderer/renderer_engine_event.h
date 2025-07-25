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

#ifndef CONTENT_RENDERER_RENDERER_ENGINE_EVENT_H_
#define CONTENT_RENDERER_RENDERER_ENGINE_EVENT_H_

#include <string>

#include "content/common/content_export.h"

namespace content {

class CONTENT_EXPORT EventLog {
 public:
  // Report render event to ui throw browser.
  static void ReportEngineEvent(const std::string& module,
                                const std::string& resource,
                                const std::string& error_code,
                                const std::string& error_msg);
};

}  // namespace content

#endif  // CONTENT_RENDERER_RENDERER_ENGINE_EVENT_H_