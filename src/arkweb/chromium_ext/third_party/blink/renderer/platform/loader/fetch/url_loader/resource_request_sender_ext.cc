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

#include "arkweb/build/features/features.h"

namespace blink {

#if BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)
void ResourceRequestSender::OnTransferDataWithSharedMemory(
    base::ReadOnlySharedMemoryRegion region,
    uint64_t buffer_size) {
  if (!request_info_) {
    return;
  }
  LOG(DEBUG)
      << "intercept ResourceRequestSender::OnTransferDataWithSharedMemory, "
         "buffer_size="
      << buffer_size;
  if (request_info_->resource_load_info_notifier_wrapper) {
    request_info_->resource_load_info_notifier_wrapper->SetUsedSharedMemory();
  } else {
    LOG(ERROR) << "resource load info notifier wrapper is nullptr";
  }
  request_info_->client->OnTransferDataWithSharedMemory(std::move(region),
                                                        buffer_size);
}
#endif

}  // namespace blink