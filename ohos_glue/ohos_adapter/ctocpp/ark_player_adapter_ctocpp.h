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

#ifndef ARK_PLAYER_ADAPTER_CTOCPP_H_
#define ARK_PLAYER_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_media_adapter_capi.h"
#include "ohos_adapter/include/ark_media_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkPlayerAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkPlayerAdapterCToCpp, ArkPlayerAdapter,
                                    ark_player_adapter_t> {
public:
  ArkPlayerAdapterCToCpp();
  virtual ~ArkPlayerAdapterCToCpp();

  // ArkPlayerAdapter methods.
  int32_t SetPlayerCallback(
      ArkWebRefPtr<ArkPlayerCallbackAdapter> callbackAdapter) override;

  int32_t SetSource(const ArkWebString &url) override;

  int32_t SetSource(int32_t fd, int64_t offset, int64_t size) override;

  int32_t SetVideoSurface(
      ArkWebRefPtr<ArkIConsumerSurfaceAdapter> cSurfaceAdapter) override;

  int32_t SetVolume(float leftVolume, float rightVolume) override;

  int32_t Seek(int32_t mSeconds, int32_t mode) override;

  int32_t Play() override;

  int32_t Pause() override;

  int32_t PrepareAsync() override;

  int32_t GetCurrentTime(int32_t &currentTime) override;

  int32_t GetDuration(int32_t &duration) override;

  int32_t SetPlaybackSpeed(int32_t mode) override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_PLAYER_ADAPTER_CTOCPP_H_
