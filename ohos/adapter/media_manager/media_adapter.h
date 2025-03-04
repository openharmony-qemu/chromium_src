// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_MEDIA_ADAPTER_H_
#define OHOS_MEDIA_ADAPTER_H_

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <multimedia/image_framework/image_mdk.h>
#include <multimedia/image_framework/image_mdk_common.h>
#include <multimedia/image_framework/image_receiver_mdk.h>
#include <native_buffer/native_buffer.h>
#include <native_image/native_image.h>
#include <native_window/external_window.h>

#include <functional>
#include <string>

#include "napi/native_api.h"
#include "ohos/adapter/export.h"

namespace ohos {
namespace adapter {
enum OhosImageComponentType {
  FORMAT_YUV_Y = 1,
  FORMAT_YUV_U = 2,
  FORMAT_YUV_V = 3,
  FORMAT_JPEG = 4,
};

class ADAPTER_EXPORT_API MediaAdapter {
  using BufferAvailableCallback =
      std::function<void(uint8_t* data, size_t data_size)>;

 public:
  MediaAdapter() = default;
  ~MediaAdapter() = default;
  static MediaAdapter& GetInstance();
  static std::string GetPreviewSurfaceId();
  static napi_value ReadImageFromReceiver(napi_env env,
                                          napi_callback_info info);
  static void InitImageReceiver();
  BufferAvailableCallback GetBufferAvailableCallback() {
    return bufferAvailableCallback_;
  }
  void RegisterBufferAvailableCallback(BufferAvailableCallback callback);

 private:
  BufferAvailableCallback bufferAvailableCallback_;
};
}  // namespace adapter
}  // namespace ohos

#endif  // OHOS_SURFACEID_ADAPTER_H_
