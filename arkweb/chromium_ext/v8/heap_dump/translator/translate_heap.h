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

#ifndef TRANSLATE_HEAP_H
#define TRANSLATE_HEAP_H

#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(OH_ENABLE_HEAP_DUMP_TEST))

#include "arkweb/chromium_ext/v8/heap_dump/binary_reader_base.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_format.h"

namespace dfx {
class SnapshotGenerator;
class RootTranslator;
class StringTableTranslator;
class ObjectTranslator;
class BinaryReaderBase;

class HeapTranslator {
 public:
  HeapTranslator(BinaryReaderBase* reader, SnapshotGenerator* generator);
  ~HeapTranslator() = default;

  void TranslateHeap();

 private:
  void TranslateHeader();
  void TranslateRoots();
  void TranslateStringTable();
  void TranslateObjects();

  BinaryReaderBase* reader_{nullptr};
  SnapshotGenerator* generator_{nullptr};
  RawHeapHeader header_;

  std::unique_ptr<RootTranslator> root_translator_;
  std::unique_ptr<StringTableTranslator> string_table_translator_;
  std::unique_ptr<ObjectTranslator> object_translator_;
};
}  // namespace dfx

#endif
#endif
