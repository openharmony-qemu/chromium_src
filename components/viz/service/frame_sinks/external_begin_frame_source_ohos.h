/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
   RAW_PTR_EXCLUSION OH_NativeVSync* native_vsync_ = nullptr;
  
   long long vsync_period_ = 0;

   uint64_t surface_id_;

   base::WeakPtrFactory<ExternalBeginFrameSourceOHOS> weak_factory_{this};
 };
 }  // namespace viz

 #endif