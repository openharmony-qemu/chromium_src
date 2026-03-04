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

#include "chunk_writer_browser_impl.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "base/logging.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "mojo/public/cpp/system/data_pipe.h"
#include "mojo/public/cpp/system/simple_watcher.h"

ChunkWriterImpl::ChunkWriterImpl()
    : watcher_(FROM_HERE,
               mojo::SimpleWatcher::ArmingPolicy::MANUAL,
               base::SequencedTaskRunner::GetCurrentDefault()) {}

void ChunkWriterImpl::ProcessPendingReceiver(
    mojo::PendingReceiver<heapdump::mojom::ChunkWriter>& receiver) {
  mojo::MakeSelfOwnedReceiver(std::make_unique<ChunkWriterImpl>(),
                              std::move(receiver));
}

void ChunkWriterImpl::OpenFile(const std::string& path,
                               uint32_t total_bytes,
                               mojo::ScopedDataPipeConsumerHandle consumer) {
  if (fd_ >= 0) {
    LOG(ERROR) << "HeapDump: Open File twice.";
    return;
  }
  if (!consumer.is_valid()) {
    LOG(ERROR) << "HeapDump: OpenFile consumer is not valid.";
    return;
  }
  consumer_ = std::move(consumer);

  fd_ = ::open(path.c_str(),
               O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW | O_CLOEXEC,
               S_IRUSR | S_IWUSR);
  if (fd_ < 0) {
    LOG(ERROR) << "HeapDump: Open File Failed.";
    return;
  }
  LOG(INFO) << "HeapDump: Open File sucessfuly.";
  watcher_.Watch(consumer_.get(), MOJO_HANDLE_SIGNAL_READABLE,
                 MOJO_TRIGGER_CONDITION_SIGNALS_SATISFIED,
                 base::BindRepeating(&ChunkWriterImpl::OnReadable,
                                     weak_ptr_factory_.GetWeakPtr()));
  watcher_.ArmOrNotify();
}

void ChunkWriterImpl::OnReadable(MojoResult result,
                                 const mojo::HandleSignalsState& state) {
  if (result != MOJO_RESULT_OK && result != MOJO_RESULT_SHOULD_WAIT) {
    clear();
    return;
  }
  base::span<const uint8_t> buffer;
  MojoResult r = consumer_->BeginReadData(MOJO_READ_DATA_FLAG_NONE, buffer);
  if (r == MOJO_RESULT_SHOULD_WAIT) {
    watcher_.ArmOrNotify();
    return;
  }
  if (r != MOJO_RESULT_OK) {
    LOG(ERROR) << "HeapDump: BeginReadData failed: " << r;
    clear();
    return;
  }
  base::span<const uint8_t> data = base::as_byte_span(buffer);
  WriteBinaryToDisk(data.data(), data.size());
  consumer_->EndReadData(buffer.size());
  watcher_.ArmOrNotify();
}

void ChunkWriterImpl::WriteBinaryToDisk(const uint8_t* data, size_t size) {
  if (fd_ < 0) {
    clear();
    LOG(ERROR) << "HeapDump: file hasn't been opend";
    return;
  }
  ssize_t r = ::write(fd_, data, size);
  CHECK(r > 0);
}

void ChunkWriterImpl::CloseFile(const std::string& path) {
  LOG(INFO) << "HeapDump: CloseFile";
}

void ChunkWriterImpl::clear() {
  if (fd_ >= 0) {
    ::close(fd_);
    fd_ = -1;
  }
  watcher_.Cancel();
  consumer_.reset();
}

ChunkWriterImpl::~ChunkWriterImpl() {
  clear();
}
