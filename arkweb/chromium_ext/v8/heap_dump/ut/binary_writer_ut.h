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

#ifndef BINARY_WRITER_UT_H
#define BINARY_WRITER_UT_H

#if defined(OH_ENABLE_HEAP_DUMP)

#include <string>
#include <vector>

#include "src/base/logging.h"
#include "heap_dump/binary_writer_base.h"

namespace dfx {

class BinaryWriterTest : public BinaryWriterBase {
 public:
  BinaryWriterTest() = default;
  ~BinaryWriterTest() override {}
  void OpenFile(const std::string& path) override {}
  const std::string& GetFilePath() override { return path_; }

  bool WriteBinBlock(const uint8_t* block, uint32_t block_size) override {
    data_.insert(data_.end(), block, block + block_size);
    return true;
  }

  void CloseFile() override {}

  uint8_t* Data() { return data_.data(); }
  uint32_t Size() { return static_cast<uint32_t>(data_.size()); }

 private:
  bool MaybeWriteChunk() override { return true; }
  bool WriteChunk() override { return true; }

  std::vector<uint8_t> data_;
  std::string path_{""};
};

}  // namespace dfx

#endif  // OH_ENABLE_HEAP_DUMP
#endif  // BINARY_WRITER_UT_H
