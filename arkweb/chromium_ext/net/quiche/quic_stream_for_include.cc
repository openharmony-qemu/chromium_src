/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef QUICHE_QUIC_CORE_QUIC_STREAM_H_
#error "must be in include form QUICHE_QUIC_CORE_QUIC_STREAM_H_"
#endif

#include "net/third_party/quiche/src/quiche/quic/core/quic_stream.h"

namespace quic {

uint64_t QuicStream::BufferedBytesInSequence() const {
  if (sequencer()) {
    return sequencer()->NumBytesBuffered();
  }

  return 0;
}

}  // namespace quic