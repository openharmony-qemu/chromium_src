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

#ifndef BINARY_READER_H
#define BINARY_READER_H
#if defined(ON_ENABLE_HEAP_TRANSLATE)
#include <fstream>
#include <iostream>
#include <string>

#include "arkweb/chromium_ext/v8/heap_dump/binary_reader_base.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_format.h"

namespace dfx {
class BinaryReader : public dfx::BinaryReaderBase {
 public:
  BinaryReader(std::string path);
  ~BinaryReader();
  bool ReadHeader(dfx::RawHeapHeader& header);
  bool ReadData(uint32_t read_size, uint8_t* out, uint32_t out_size) override;
  bool ReadDataAt(uint32_t offset,
                  uint32_t read_size,
                  uint8_t* out,
                  uint32_t out_size) override;

  uint32_t BinarySize() const override;
  uint32_t CurrentPosition() override;
  void SetPosition(uint32_t pos) override;

  bool IsValid() const;

 private:
  std::string path_;
  std::ifstream file_;
  uint32_t file_size_{0};
};
}  // namespace dfx
#endif
#endif
