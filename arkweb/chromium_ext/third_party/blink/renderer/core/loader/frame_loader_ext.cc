/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "third_party/blink/renderer/core/loader/frame_loader.h"

namespace blink {

#if BUILDFLAG(ARKWEB_LOGGER_REPORT) && !BUILDFLAG(ARKWEB_NWEB_EX)
void FrameLoader::InitFrameLoader() {}

void FrameLoader::StartNavigationLoggerReport(FrameLoadRequest& request,
                                              ResourceRequest& resource_request,
                                              WebFrameLoadType frame_load_type,
                                              bool same_document_navigation,
                                              bool is_triggered_by_js,
                                              WebNavigationType navigation_type,
                                              const KURL& url) {}
#endif

}  // namespace blink
