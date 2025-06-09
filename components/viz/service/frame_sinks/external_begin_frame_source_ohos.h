/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
 #ifndef COMPONENTS_VIZ_SERVICE_FRAME_SINKS_EXTERNAL_BEGIN_FRAME_SOURCE_OHOS_H_
 #define COMPONENTS_VIZ_SERVICE_FRAME_SINKS_EXTERNAL_BEGIN_FRAME_SOURCE_OHOS_H_

 #include <algorithm>
 #include <cstdint>
 #include <deviceinfo.h>
 #include <memory>
 #include <native_vsync/native_vsync.h>
 
 #include "base/time/time.h"
 #include "components/viz/common/frame_sinks/begin_frame_source.h"
 #include "components/viz/common/surfaces/frame_sink_id.h"
 #include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
 #include "components/viz/service/viz_service_export.h"

 namespace viz {
 class VIZ_SERVICE_EXPORT ExternalBeginFrameSourceOHOS
     : public ExternalBeginFrameSource,
       public ExternalBeginFrameSourceClient {
  public:
   explicit ExternalBeginFrameSourceOHOS(uint32_t restart_id, uint64_t surface_id);
   ~ExternalBeginFrameSourceOHOS() override;
   ExternalBeginFrameSourceOHOS(const ExternalBeginFrameSourceOHOS&) = delete;
   ExternalBeginFrameSourceOHOS& operator=(const ExternalBeginFrameSourceOHOS&) =
       delete;

  static void OnVSync(long long timestamp, void* data);
  class VSyncUserData;
  void OnVSyncImpl(int64_t timestamp, VSyncUserData* user_data);

   // ExternalBeginFrameSourceClient implementation.
   void OnNeedsBeginFrames(bool needs_begin_frames) override;

  private:
   void SetEnabled(bool enabled);

   BeginFrameArgsGenerator begin_frame_args_generator_;
   bool vsync_notification_enabled_;

   std::unique_ptr<VSyncUserData> user_data_;
   struct OH_NativeVSync *native_vsync_ = nullptr;
  
   long long vsync_period_ = 0;
   base::WeakPtrFactory<ExternalBeginFrameSourceOHOS> weak_factory_{this};

   uint64_t surface_id_;
 };
 }  // namespace viz

 #endif
