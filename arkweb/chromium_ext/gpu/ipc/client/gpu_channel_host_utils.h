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

#ifndef GPU_IPC_CLIENT_GPU_CHANNEL_HOST_UTILS_H_
#define GPU_IPC_CLIENT_GPU_CHANNEL_HOST_UTILS_H_

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include <string>
#endif

namespace gpu {
class GpuChannelHost;

class GpuChannelHostUtils {
 public:
  GpuChannelHost* gpu_channel_Host_;
  GpuChannelHostUtils(GpuChannelHost* gpu_channel_Host);

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void SetBlanklessDumpInfo(uint32_t nweb_id,
                            uint64_t blankless_key,
                            uint64_t frame_sink_id,
                            int32_t lcp_time,
                            int64_t pref_hash);
#endif
};

}  // namespace gpu

#endif  // GPU_IPC_CLIENT_GPU_CHANNEL_HOST_UTILS_H_