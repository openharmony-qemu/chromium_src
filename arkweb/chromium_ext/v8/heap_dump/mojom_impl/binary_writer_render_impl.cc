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

#include "binary_writer_render_impl.h"

#include "arkweb/chromium_ext/v8/heap_dump/mojom/dfx_heapdump.mojom.h"
#include "arkweb/ohos_nweb_ex/third_party/securec/include/securec.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "mojo/public/cpp/system/data_pipe.h"
#include "mojo/public/cpp/system/simple_watcher.h"
#include "mojo/public/cpp/system/wait.h"

namespace dfx {

BinaryWriterRender::BinaryWriterRender()
    : chunk_size_(kChunkSize), chunk_(chunk_size_) {
  LOG(INFO) << "HeapDump: binary writer init";
  if (!receiver_.is_valid()) {
    LOG(ERROR) << "HeapDump: receiver_ is not valid";
    return;
  }
  remote_.set_disconnect_handler(
      base::BindOnce([] { LOG(ERROR) << "HeapDump Remote disconnected"; }));
}

void BinaryWriterRender::OpenFile(const std::string& path) {
  if (!remote_.is_bound()) {
    LOG(ERROR) << "HeapDump Remote not bound";
    return;
  }
  if (path_.size()) {
    LOG(ERROR) << "HeapDump path has been inited";
    return;
  }
  // DataPipe
  mojo::ScopedDataPipeProducerHandle producer;
  mojo::ScopedDataPipeConsumerHandle consumer;
  const uint32_t kCapacity = 4 * chunk_size_;
  MojoCreateDataPipeOptions opts{sizeof(MojoCreateDataPipeOptions),
                                 MOJO_CREATE_DATA_PIPE_FLAG_NONE, 1, kCapacity};
  if (CreateDataPipe(&opts, producer, consumer) != MOJO_RESULT_OK) {
    LOG(ERROR) << "HeapDump: MojoCreateDataPipe Failed";
    return;
  }
  producer_ = std::move(producer);
  // OpenFile
  LOG(INFO) << "HeapDump MojoCreateDataPipe";
  path_ = path;
  remote_->OpenFile(path_, 0, std::move(consumer));
}

const std::string& BinaryWriterRender::GetFilePath() {
  return path_;
}

bool BinaryWriterRender::WriteBinBlock(const uint8_t* block,
                                       uint32_t block_size) {
  if (!remote_.is_bound() || !producer_.is_valid()) {
    LOG(ERROR) << "HeapDump Remote not bound";
    return false;
  }
  uint32_t cursor = 0;
  while (block_size) {
    if (!MaybeWriteChunk()) {
      return false;
    }
    if (chunk_size_ < cur_size_) {
       LOG(ERROR) << "Fail to memcpy.";
       return false;
     }
    uint32_t dst_size = chunk_size_ - cur_size_;
    uint32_t write_size = std::min(dst_size, block_size);
    // dst_size >= write_size is always true
    memcpy(chunk_.data() + cur_size_, block + cursor, write_size);
    cursor += write_size;
    block_size -= write_size;
    cur_size_ += write_size;
  }
  return true;
}

bool BinaryWriterRender::MaybeWriteChunk() {
  if (cur_size_ == chunk_size_) {
    return WriteChunk();
  }
  return true;
}

void BinaryWriterRender::CloseFile() {
  if (!remote_.is_bound()) {
    LOG(ERROR) << "HeapDump:binary writer remote is not bound.";
    return;
  }
  LOG(INFO) << "HeapDump: CloseFile start to write chunk";
  CHECK(WriteChunk());
  remote_->CloseFile(path_);
  clear();
  LOG(INFO) << "CloseFile";
}

bool BinaryWriterRender::WriteChunk() {
  if (!remote_.is_bound() || !producer_.is_valid()) {
    LOG(ERROR) << "HeapDump: WriteChunk binary writer remote or producer is "
                  "not bound.";
    return false;
  }
  uint8_t* data = chunk_.data();
  while (cur_size_) {
    base::span<uint8_t> buffer;
    MojoResult result = producer_->BeginWriteData(
        cur_size_, MOJO_BEGIN_WRITE_DATA_FLAG_NONE, buffer);
    if (result == MOJO_RESULT_SHOULD_WAIT) {
      result = mojo::Wait(producer_.get(), MOJO_HANDLE_SIGNAL_WRITABLE);
      if (result != MOJO_RESULT_OK) {
        LOG(ERROR) << "HeapDump: WriteChunk failed to begin write data"
                   << result;
        return false;
      }
      result = producer_->BeginWriteData(
          cur_size_, MOJO_BEGIN_WRITE_DATA_FLAG_NONE, buffer);
    }

    if (result != MOJO_RESULT_OK) {
      LOG(ERROR) << "HeapDump: WriteChunk failed to begin write data" << result;
      return false;
    }
    uint32_t dst_size = static_cast<uint32_t>(buffer.size());
    uint32_t write_size = std::min(dst_size, cur_size_);
    // dst_size >= write_size is always true
    memcpy(buffer.data(), data, write_size);
    producer_->EndWriteData(write_size);
    data += write_size;
    cur_size_ -= write_size;
  }
  return true;
}

std::shared_ptr<BinaryWriterRender> BinaryWriterRender::GetInstance() {
  static std::shared_ptr<BinaryWriterRender> instance =
      std::make_shared<BinaryWriterRender>();
  LOG(INFO) << "HeapDump BinaryWriterRender::GetInstance";
  return instance;
}

mojo::PendingReceiver<heapdump::mojom::ChunkWriter>&
BinaryWriterRender::GetPendingReceiver() {
  if (!receiver_.is_valid()) {
    LOG(ERROR) << "HeapDump:receiver_ is not valid";
  } else {
    LOG(INFO) << "HeapDump: GetPendingReceiver receiver_.is_valid";
  }
  return receiver_;
}

void BinaryWriterRender::clear() {
  path_ = "";
  cur_size_ = 0;
  producer_.reset();
}

}  // namespace dfx
