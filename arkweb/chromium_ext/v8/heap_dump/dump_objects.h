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

#ifndef DUMP_OBJECTS_H
#define DUMP_OBJECTS_H

#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(USING_OHOS_WEB))
#include "binary_writer_base.h"
#include "dump_format.h"

#include <atomic>
#include <chrono>
#include <cinttypes>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "src/codegen/assembler-inl.h"
#include "src/heap/combined-heap.h"
#include "src/heap/heap-visitor-inl.h"
#include "src/heap/heap-visitor.h"
#include "src/heap/heap.h"
#include "src/heap/visit-object.h"

#include "v8_ohlog.h"

namespace dfx {
using namespace std::chrono;
using namespace v8;

class ObjectDumper {
 public:
  explicit ObjectDumper(v8::internal::Heap* heap, BinaryWriterBase* writer);
  ~ObjectDumper() = default;

  void DumpReadOnly();

  void Dump();

  void PreVisit();

  uint32_t ObjectCount() const;
  uint32_t ObjectDumpSize() const;

 private:
  DISALLOW_GARBAGE_COLLECTION(no_heap_allocation_)

  std::unique_ptr<v8::internal::SafepointScope> safepoint_scope_;

  i::Heap* heap_{ nullptr };
  uint32_t object_cnt_{0};
  uint32_t total_object_size_{0};
  BinaryWriterBase* writer_{ nullptr };

#ifdef OH_ENABLE_HEAP_DUMP_TEST
  // only for test
  std::unordered_map<dfx::Address, bool> heap_objects_;
#endif
};
}  // namespace dfx

#endif  // OH_ENABLE_HEAP_DUMP_H
#endif
