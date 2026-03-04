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

#ifndef DUMP_HEAP_H
#define DUMP_HEAP_H

#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(USING_OHOS_WEB))

#include "arkweb/chromium_ext/v8/heap_dump/binary_writer_base.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_format.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_objects.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_roots.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_string_tables.h"
#include "arkweb/chromium_ext/v8/v8_ohlog.h"
#include "src/base/platform/elapsed-timer.h"
#include "src/codegen/assembler-inl.h"
#include "src/heap/combined-heap.h"
#include "src/heap/heap-visitor-inl.h"
#include "src/heap/heap-visitor.h"
#include "src/heap/heap.h"
#include "src/heap/visit-object.h"

namespace dfx {

class HeapDumper {
 public:
  HeapDumper(v8::internal::Heap* heap, BinaryWriterBase* writer);
  ~HeapDumper() = default;

  void DumpHeap();

 private:
  DISALLOW_GARBAGE_COLLECTION(no_heap_allocation_)

  void PreVisit();
  void DumpHeader();

  // v8 related
  v8::internal::Heap* heap_{nullptr};
  std::unique_ptr<v8::internal::SafepointScope> safepoint_scope_;

  // dump related
  BinaryWriterBase* writer_{nullptr};
  RawHeapHeader header_;
  RootDumper root_dumper_;
  StringTableDumper string_table_dumper_;
  ObjectDumper object_dumper_;
};
}  // namespace dfx
#endif
#endif  // DUMP_HEAP_H
