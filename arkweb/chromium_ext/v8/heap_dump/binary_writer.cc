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

#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(OH_ENABLE_HEAP_DUMP_TEST))

#include "arkweb/chromium_ext/v8/heap_dump/binary_writer.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <cerrno>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>

#include "arkweb/ohos_nweb_ex/third_party/securec/include/securec.h"
#include "arkweb/chromium_ext/v8/v8_ohlog.h"

namespace dfx {
BinaryWriter::BinaryWriter() : BinaryWriter(kChunkSize) {}
BinaryWriter::BinaryWriter(const uint32_t chunk_size)
    : chunk_size_(chunk_size), chunk_(chunk_size) {}

BinaryWriter::~BinaryWriter() {
  CloseFile();
}

bool BinaryWriter::WriteBinBlock(const uint8_t* block, uint32_t block_size) {
  if (fd_ < 0) {
    LogInfo("fd_ didn't open");
    return false;
  }
  uint32_t cursor = 0;
  while (block_size) {
    if (!MaybeWriteChunk()) {
      return false;
    }
    uint32_t dst_size = chunk_size_ - cur_size_;
    uint32_t write_size = std::min(dst_size, block_size);
    errno_t ret = memcpy_s(chunk_.data() + cur_size_, dst_size, block + cursor, write_size);
    if (ret != 0) {
      LogInfo("memcpy_s failed, ret=" + std::to_string(ret));
      return false;
    }
    cursor += write_size;
    block_size -= write_size;
    cur_size_ += write_size;
  }
  return true;
}

bool BinaryWriter::MaybeWriteChunk() {
  CHECK(cur_size_ <= chunk_size_);
  if (V8_UNLIKELY(cur_size_ == chunk_size_)) {
    return WriteChunk();
  }
  return true;
}

bool BinaryWriter::Write() {
  uint8_t* data = chunk_.data();
  while (cur_size_) {
    ssize_t r = ::write(fd_, data, cur_size_);
    CHECK(r > 0);
    data += static_cast<size_t>(r);
    cur_size_ -= static_cast<size_t>(r);
  }
  return true;
}

bool BinaryWriter::WriteChunk() {
  if (fd_ < 0) {
    LogInfo("fd_ didn't open");
    return false;
  }
  CHECK(cur_size_ == chunk_size_);
  return Write();
}

void BinaryWriter::OpenFile(const std::string& path) {
  if (fd_ >= 0) {
    LogInfo("HeapDump already opened!");
    return;
  }
  path_ = path;
  fd_ = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW | O_CLOEXEC, S_IRUSR | S_IWUSR);
  if (fd_ < 0) {
    std::stringstream ss;
    ss << "HeapDump open file failed!" << std::endl;
    std::string message = ss.str();
    LogInfo(message);
    CHECK(false);
  }
} 

const std::string& BinaryWriter::GetFilePath() {
  return path_;
}

void BinaryWriter::CloseFile() {
  if (fd_ < 0) {
    LogInfo("fd_ didn't open");
    return;
  }
  Flush();
  ::close(fd_);
  fd_ = -1;
}

void BinaryWriter::Flush() {
  // write rest
  if (cur_size_) {
    Write();
  }
}

}  // namespace dfx

#endif
