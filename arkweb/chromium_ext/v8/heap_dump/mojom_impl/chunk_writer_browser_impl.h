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

#ifndef CHUNK_WRITER_BROWSER_IMPL_H
#define CHUNK_WRITER_BROWSER_IMPL_H

#include "arkweb/chromium_ext/v8/heap_dump/mojom/dfx_heapdump.mojom.h"
#include "mojo/public/cpp/bindings/remote.h"

class ChunkWriterImpl : public heapdump::mojom::ChunkWriter {
 public:
  ChunkWriterImpl();
  ~ChunkWriterImpl() override;

  static void ProcessPendingReceiver(
      mojo::PendingReceiver<heapdump::mojom::ChunkWriter>& receiver);

  void OpenFile(const std::string& path,
                uint32_t total_bytes,
                mojo::ScopedDataPipeConsumerHandle consumer) override;
  void WriteBinaryToDisk(const uint8_t* data, size_t size);
  void CloseFile(const std::string& path) override;

 private:
  void OnReadable(MojoResult result, const mojo::HandleSignalsState& state);
  void clear();
  int fd_{-1};

  mojo::SimpleWatcher watcher_;
  mojo::ScopedDataPipeConsumerHandle consumer_;

  base::WeakPtrFactory<ChunkWriterImpl> weak_ptr_factory_{this};
};

#endif
