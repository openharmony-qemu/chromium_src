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

#ifndef TRANSLATE_ROOTS_H
#define TRANSLATE_ROOTS_H

#if (defined(ON_ENABLE_HEAP_TRANSLATE) || defined(OH_ENABLE_HEAP_DUMP_TEST))

#include <unordered_map>
#include <vector>

#include "arkweb/chromium_ext/v8/heap_dump/binary_reader_base.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_format.h"
#include "include/v8-internal.h"
#include "src/objects/visitors.h"

namespace dfx {

class SnapshotGenerator;

class RootTranslator {
 public:
  explicit RootTranslator(uint32_t table_offset,
                          uint32_t type_count,
                          uint32_t dump_size,
                          BinaryReaderBase* reader,
                          SnapshotGenerator* generator);
  void Translate();
  void TranslateRoot(uint32_t type, uint32_t count);

 private:
  const uint32_t table_offset_;
  const uint32_t type_count_;
  const uint32_t dump_size_;
  BinaryReaderBase* reader_{nullptr};
  SnapshotGenerator* generator_{nullptr};

#ifdef OH_ENABLE_HEAP_DUMP_TEST
  // output
  std::unordered_map<v8::internal::Root, std::vector<v8::internal::Address>>
      roots_info_;
#endif
};
}  // namespace dfx

#endif
#endif
