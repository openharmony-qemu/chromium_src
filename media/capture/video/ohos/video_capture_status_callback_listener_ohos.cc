#include "media/capture/video/ohos/video_capture_device_factory_ohos.h"
#include "video_capture_status_callback_listener_ohos.h"
#include "base/logging.h"
#include "ohos_adapter_helper.h"
namespace media {

VideoCaptureCameraStatusCallbackListenerOHOS::VideoCaptureCameraStatusCallbackListenerOHOS(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    base::WeakPtr<VideoCaptureDeviceFactoryOHOS> capture_device_factory)
    : task_runner_(task_runner), capture_device_factory_(capture_device_factory) {
  DCHECK(task_runner_.get());
  DCHECK(capture_device_factory_);
}

VideoCaptureCameraStatusCallbackListenerOHOS::~VideoCaptureCameraStatusCallbackListenerOHOS() {}

void VideoCaptureCameraStatusCallbackListenerOHOS::OnCameraStatusChanged(CameraStatusAdapter camera_status) {
  task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&VideoCaptureDeviceFactoryOHOS::OnCameraStatusChanged, capture_device_factory_,
                     camera_status));
}

}  // namespace media
