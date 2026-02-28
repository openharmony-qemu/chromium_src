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

#ifndef BINARY_WRITER_RENDER_IMPL_H
#define BINARY_WRITER_RENDER_IMPL_H

#include "arkweb/chromium_ext/v8/heap_dump/binary_writer_base.h"
#include "arkweb/chromium_ext/v8/heap_dump/mojom/dfx_heapdump.mojom.h"
#include "mojo/public/cpp/bindings/remote.h"

namespace dfx {
class BinaryWriterRender : public BinaryWriterBase {
 public:
  BinaryWriterRender();

  ~BinaryWriterRender() override = default;
  void OpenFile(const std::string& path) override;
  const std::string& GetFilePath() override;
  bool WriteBinBlock(const uint8_t* block, uint32_t block_size) override;
  void CloseFile() override;

  mojo::PendingReceiver<heapdump::mojom::ChunkWriter>& GetPendingReceiver();
  // a render has only one
  static std::shared_ptr<BinaryWriterRender> GetInstance();
  bool IsInitialized() { return is_intialized_; }
  void SetInitialized() { is_intialized_ = true; }

 private:
  bool MaybeWriteChunk() override;
  bool WriteChunk() override;
  void clear();

  std::string path_{""};
  const uint32_t chunk_size_;
  std::vector<uint8_t> chunk_;
  uint32_t cur_size_{0};
  bool is_intialized_{false};

  // mojo related
  mojo::Remote<heapdump::mojom::ChunkWriter> remote_;
  mojo::PendingReceiver<heapdump::mojom::ChunkWriter> receiver_ =
      remote_.BindNewPipeAndPassReceiver();

  mojo::ScopedDataPipeProducerHandle producer_;

  static constexpr size_t kChunkSize = 4 * 1024 * 1024;
};

}  // namespace dfx
#endif
