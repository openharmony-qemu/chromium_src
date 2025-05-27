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
 
#ifndef MEDIA_OHOS_BASE_SCREEN_CAPTURE_SOURCE_BRIDGE_H_
#define MEDIA_OHOS_BASE_SCREEN_CAPTURE_SOURCE_BRIDGE_H_
 
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "media/base/audio_capturer_source.h"
#include "media/base/media_export.h"
#include "content/common/content_export.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/screen_capture_adapter.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/base_screen_capture_read_callback.h"
 
namespace base {
class SingleThreadTaskRunner;
}  // namespace base
 
namespace webrtc {
using namespace OHOS::NWeb;
using namespace media;
 
class BaseScreenCaptureSource {
public:
 
    // Single Instance
    BaseScreenCaptureSource(const BaseScreenCaptureSource&) = delete;
 
    BaseScreenCaptureSource(BaseScreenCaptureSource&&) = delete;
 
    BaseScreenCaptureSource& operator=(const BaseScreenCaptureSource&) = delete;
 
    BaseScreenCaptureSource& operator=(BaseScreenCaptureSource&&) = delete;
 
    static BaseScreenCaptureSource& GetInstance() {
        static BaseScreenCaptureSource instance;
        return instance;
    }
 
    bool SetScreenCaptureConfig(int nweb_id);

    void SetScreenCapturePickerShow(bool show_pick);
 
    int32_t RegisterAudioCaptureCallback(std::shared_ptr<BaseScreenCaptureReadCallback> callback, int nweb_id);
 
    int32_t RegisterWindowCaptureCallback(std::shared_ptr<BaseScreenCaptureReadCallback> callback, int nweb_id);
 
    void SetScreenCaptureState(const OHOS::NWeb::ScreenCaptureStateCodeAdapter& stateCode, int nweb_id);
 
    int32_t StopCapture(int nweb_id);
 
    int32_t StartCapture(int nweb_id);

    void ReleaseCapture(int nweb_id);

    std::unordered_map<int, std::unique_ptr<OHOS::NWeb::ScreenCaptureAdapter>> screen_capture_adapter_map_;

    bool ScreenCaptureAdapterIsExist(int nweb_id);
 
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;

    std::unordered_map<int, std::shared_ptr<BaseScreenCaptureReadCallback>> audio_callback_map_;

    bool AudioCallbackIsExist(int nweb_id);

    std::unordered_map<int, std::shared_ptr<BaseScreenCaptureReadCallback>> window_callback_map_;

    bool WindowCallbackIsExist(int nweb_id);
 
    std::unordered_map<int, OHOS::NWeb::ScreenCaptureStateCodeAdapter> capture_state_code_map_;
 
    base::WeakPtrFactory<BaseScreenCaptureSource> weak_factory_{this};
 
private:

    bool is_picker_show_ = false;
  
    BaseScreenCaptureSource();
    
    ~BaseScreenCaptureSource();
};
}
 
#endif // MEDIA_OHOS_BASE_SCREEN_CAPTURE_SOURCE_BRIDGE_H_