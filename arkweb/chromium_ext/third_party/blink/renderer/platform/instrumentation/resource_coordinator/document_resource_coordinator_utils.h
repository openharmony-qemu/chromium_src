/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef DOCUMENT_RESOURCE_COORDINATOR_UTILS_H_
#define DOCUMENT_RESOURCE_COORDINATOR_UTILS_H_

#include <regex>

#include "arkweb/build/features/features.h"
#include "base/trace_event/trace_event.h"
#include "third_party/blink/renderer/platform/instrumentation/resource_coordinator/document_resource_coordinator.h"

namespace blink {

class DocumentResourceCoordinatorUtils {
 public:
  friend class DocumentResourceCoordinator;
  explicit DocumentResourceCoordinatorUtils(
      DocumentResourceCoordinator* stream_reader_url_loader);
  ~DocumentResourceCoordinatorUtils();

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
  void OnFormEditingStateChanged(uint64_t form_id, bool did_submit);
#endif

 private:
  DocumentResourceCoordinator* document_resource_coordinator_;
};
}  // namespace blink
#endif
