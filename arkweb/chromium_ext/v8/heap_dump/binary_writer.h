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

#ifndef BINARY_WRITER_H
#define BINARY_WRITER_H

#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(OH_ENABLE_HEAP_DUMP_TEST))

#include <string>
#include <vector>

#include "binary_writer_base.h"
#include "src/base/logging.h"

namespace dfx {

class BinaryWriter : public BinaryWriterBase {
 public:
  explicit BinaryWriter();
  explicit BinaryWriter(const uint32_t chunk_size);
  ~BinaryWriter() override;
  void OpenFile(const std::string& path) override;
  const std::string& GetFilePath() override;

  bool WriteBinBlock(const uint8_t* block, uint32_t block_size) override;

  void CloseFile() override;

 private:
  void Flush();
  bool MaybeWriteChunk() override;
  bool Write();
  bool WriteChunk() override;

  const uint32_t chunk_size_;
  uint32_t cur_size_{0};
  std::vector<uint8_t> chunk_;
  int fd_{-1};
  std::string path_;
  static constexpr size_t kChunkSize = 4 * 1024 * 1024;
};

}  // namespace dfx

#endif  // OH_ENABLE_HEAP_DUMP
#endif  // BINARY_WRITER_H
