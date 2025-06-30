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

#ifndef BUFFER_DESC_ADAPTER_IMPL_H
#define BUFFER_DESC_ADAPTER_IMPL_H

#include "third_party/ohos_ndk/includes/ohos_adapter/audio_capturer_adapter.h"
#include "base/memory/raw_ptr.h"

namespace OHOS::NWeb {

class BufferDescAdapterImpl : public BufferDescAdapter {
 public:
  BufferDescAdapterImpl() = default;

  uint8_t* GetBuffer() override;

  size_t GetBufLength() override;

  size_t GetDataLength() override;

  void SetBuffer(uint8_t* buffer) override;

  void SetBufLength(size_t bufLength) override;

  void SetDataLength(size_t dataLength) override;

 private:
  raw_ptr<uint8_t> buffer_ = nullptr;
  size_t buf_length_ = 0;
  size_t data_length_ = 0;
};

}  // namespace OHOS::NWeb

#endif  // BUFFER_DESC_ADAPTER_IMPL_H
