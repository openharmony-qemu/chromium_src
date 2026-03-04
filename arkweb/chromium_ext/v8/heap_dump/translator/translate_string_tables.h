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

#ifndef TRANSLATE_STRING_TABLES_H
#define TRANSLATE_STRING_TABLES_H

#if (defined(ON_ENABLE_HEAP_TRANSLATE) || defined(OH_ENABLE_HEAP_DUMP_TEST))

#include <stdint.h>

#include <vector>

#include "arkweb/chromium_ext/v8/heap_dump/binary_reader_base.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_format.h"

namespace dfx {
class StringTableTranslator {
 public:
  StringTableTranslator(BinaryReaderBase* reader,
                        uint32_t offset,
                        uint32_t size,
                        uint32_t count);
  ~StringTableTranslator() = default;
  void Translate();

  uint32_t GetTableOffset() const { return table_offset_; }

  uint32_t GetTableSize() const { return table_size_; }

  uint32_t GetObjectCount() const { return object_count_; }

 private:
  BinaryReaderBase* reader_;
  const uint32_t table_offset_;
  const uint32_t table_size_;
  const uint32_t object_count_;
  std::vector<StringTableElementInfo> stringTableList_;
};
}  // namespace dfx
#endif
#endif
