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

#include "arkweb/chromium_ext/gpu/ipc/client/gpu_channel_host_utils.h"
#include "gpu/ipc/client/gpu_channel_host.h"

namespace gpu {

GpuChannelHostUtils::GpuChannelHostUtils(GpuChannelHost* gpu_channel_Host) {
  this->gpu_channel_Host_ = gpu_channel_Host;
}


#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void GpuChannelHostUtils::SetBlanklessDumpInfo(uint32_t nweb_id, uint64_t blankless_key,
                                               uint64_t frame_sink_id, int32_t lcp_time, int64_t pref_hash) {
  if (gpu_channel_Host_->gpu_channel_) {
    gpu_channel_Host_->gpu_channel_->SetBlanklessDumpInfo(nweb_id, blankless_key, frame_sink_id, lcp_time, pref_hash);
  }
}
#endif

}  // namespace gpu