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

#ifndef ARK_AUDIO_CAPTURER_ADAPTER_CTOCPP_H_
#define ARK_AUDIO_CAPTURER_ADAPTER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_adapter/capi/ark_audio_capturer_adapter_capi.h"
#include "ohos_adapter/include/ark_audio_capturer_adapter.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkAudioCapturerAdapterCToCpp
    : public ArkWebCToCppRefCounted<ArkAudioCapturerAdapterCToCpp,
                                    ArkAudioCapturerAdapter,
                                    ark_audio_capturer_adapter_t> {
public:
  ArkAudioCapturerAdapterCToCpp();
  virtual ~ArkAudioCapturerAdapterCToCpp();

  // ArkAudioCapturerAdapter methods.
  int32_t
  Create(const ArkWebRefPtr<ArkAudioCapturerOptionsAdapter> capturerOptions,
         ArkWebString &cachePath) override;

  bool Start() override;

  bool Stop() override;

  bool Release2() override;

  int32_t SetCapturerReadCallback(
      ArkWebRefPtr<ArkAudioCapturerReadCallbackAdapter> callbck) override;

  int32_t
  GetBufferDesc(ArkWebRefPtr<ArkBufferDescAdapter> buffferDesc) override;

  int32_t Enqueue(const ArkWebRefPtr<ArkBufferDescAdapter> bufferDesc) override;

  int32_t GetFrameCount(uint32_t &frameCount) override;

  int64_t GetAudioTime() override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_CAPTURER_ADAPTER_CTOCPP_H_
