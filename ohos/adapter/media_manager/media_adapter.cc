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

#include "media_adapter.h"

#include <fstream>
#include <iosfwd>
#include <iostream>
#include <thread>

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos {
namespace adapter {

MediaAdapter& MediaAdapter::GetInstance() {
  static MediaAdapter helper;
  return helper;
}
void MediaAdapter::RegisterBufferAvailableCallback(
    BufferAvailableCallback callback) {
  if (callback != nullptr) {
    bufferAvailableCallback_ = callback;
  }
}
std::string MediaAdapter::GetPreviewSurfaceId() {
  std::string surfaceId;
  std::thread t([&] {
    std::promise<std::string> promise;
    std::function<void(std::string)> callback = [&](std::string message) {
      promise.set_value(message);
    };
    auto insert =
        ohos::adapter::GetJSFunction("mediaAdapter.getPreviewSurfaceId");
    if (insert) {
      insert->Invoke<void>(callback);
      surfaceId = promise.get_future().get();
    }
  });
  t.join();
  return surfaceId;
}

napi_value MediaAdapter::ReadImageFromReceiver(napi_env env,
                                               napi_callback_info info) {
  size_t argc = 1;
  napi_value args[2] = {nullptr};
  napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  napi_valuetype valuetype0;
  napi_typeof(env, args[0], &valuetype0);
  napi_ref reference;
  napi_create_reference(env, args[0], 1, &reference);
  napi_value imgReceiver_js;
  napi_get_reference_value(env, reference, &imgReceiver_js);

  ImageReceiverNative* imgReceiver_c =
      OH_Image_Receiver_InitImageReceiverNative(env, imgReceiver_js);

  int32_t capacity;
  OH_Image_Receiver_GetCapacity(imgReceiver_c, &capacity);

  int32_t format;
  OH_Image_Receiver_GetFormat(imgReceiver_c, &format);

  OhosImageSize size;
  OH_Image_Receiver_GetSize(imgReceiver_c, &size);

  int32_t ret;
  napi_value nextImage;
  ret = OH_Image_Receiver_ReadNextImage(imgReceiver_c, &nextImage);

  ImageNative* nextImage_native = OH_Image_InitImageNative(env, nextImage);

  OhosImageSize imageSize;
  OH_Image_Size(nextImage_native, &imageSize);

  OhosImageComponent imgComponent;
  ret = OH_Image_GetComponent(nextImage_native, OHOS_IMAGE_COMPONENT_FORMAT_JPEG, &imgComponent);

  if (MediaAdapter::GetInstance().GetBufferAvailableCallback() != nullptr) {
    MediaAdapter::GetInstance().GetBufferAvailableCallback()(
        imgComponent.byteBuffer, imgComponent.size);
  }

  ret = OH_Image_Release(nextImage_native);
  ret = OH_Image_Receiver_Release(imgReceiver_c);
  return nextImage;
}

void MediaAdapter::InitImageReceiver() {
  auto insert = ohos::adapter::GetJSFunction("mediaAdapter.getImageReceiver");
  if (insert) {
    insert->Invoke<void>();
  }
}
}  // namespace adapter
}  // namespace ohos
