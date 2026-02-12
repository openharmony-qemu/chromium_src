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

#ifndef BINARY_WRITER_BASE_H
#define BINARY_WRITER_BASE_H
#include <string>
namespace dfx {
// After OpenFile, all data write to the path
class BinaryWriterBase {
 public:
  virtual ~BinaryWriterBase() = default;
  virtual void OpenFile(const std::string& path) = 0;
  virtual const std::string& GetFilePath() = 0;
  virtual bool WriteBinBlock(const uint8_t* block, uint32_t block_size) = 0;
  virtual void CloseFile() = 0;

 protected:
  virtual bool MaybeWriteChunk() = 0;
  virtual bool WriteChunk() = 0;
};

}  // namespace dfx

#endif
