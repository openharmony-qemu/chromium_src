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
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/instrumentation/resource_coordinator/document_resource_coordinator_utils.h"

namespace blink {

// LCOV_EXCL_START
DocumentResourceCoordinatorUtils::DocumentResourceCoordinatorUtils(
    DocumentResourceCoordinator* document_resource_coordinator)
{
  this->document_resource_coordinator_ = document_resource_coordinator;
}

DocumentResourceCoordinatorUtils::~DocumentResourceCoordinatorUtils() = default;

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
void DocumentResourceCoordinatorUtils::OnFormEditingStateChanged(
    uint64_t form_id,
    bool did_submit)
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (!document_resource_coordinator_ || !document_resource_coordinator_->service_) {
    return;
  }
  document_resource_coordinator_->service_->OnFormEditingStateChanged(
      form_id, did_submit);
}
#endif
// LCOV_EXCL_STOP

}  // namespace blink
