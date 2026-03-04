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
    (defined(USING_OHOS) || defined(USING_OHOS_WEB))

#include "arkweb/chromium_ext/v8/heap_dump/dump_string_tables.h"

#include "src/objects/string-table.h"
namespace dfx {
class VisitStringTableInfo : public v8::internal::RootVisitor {
 public:
  explicit VisitStringTableInfo(v8::internal::Heap* heap,
                                BinaryWriterBase* writer)
      : heap_(heap),
        safepoint_scope_(std::make_unique<v8::internal::SafepointScope>(
            heap->isolate(),
            v8::internal::kGlobalSafepointForSharedSpaceIsolate)),
        writer_(writer) {}

  void VisitRootPointers(v8::internal::Root root,
                         const char* description,
                         v8::internal::FullObjectSlot start,
                         v8::internal::FullObjectSlot end) override {
    UNREACHABLE();
  }

  void VisitRootPointers(v8::internal::Root root,
                         const char* description,
                         v8::internal::OffHeapObjectSlot start,
                         v8::internal::OffHeapObjectSlot end) override {
    if (root != v8::internal::Root::kStringTable) {
      return;
    }
    v8::internal::Isolate* isolate = heap_->isolate();
    v8::internal::PtrComprCageBase cage_base(heap_->isolate());
    for (v8::internal::OffHeapObjectSlot p = start; p < end; ++p) {
      v8::internal::Tagged<v8::internal::Object> obj = p.load(cage_base);
      if (IsHeapObject(obj)) {
        v8::internal::Tagged<v8::internal::HeapObject> heap_object =
            Cast<v8::internal::HeapObject>(obj);
        v8::internal::Address object_address = heap_object->address();
        int len = Cast<v8::internal::String>(*heap_object)->length();
        total_object_size_ += sizeof(object_address) + sizeof(len);
        writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&object_address),
                               sizeof(object_address));
        writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&len), sizeof(len));
      }
    }
  }

  uint32_t GetTotalObjectSize() { return total_object_size_; }

 private:
  v8::internal::Heap* heap_;
  std::unique_ptr<v8::internal::SafepointScope> safepoint_scope_;
  BinaryWriterBase* writer_;
  uint32_t total_object_size_{0};
};

StringTableDumper::StringTableDumper(v8::internal::Heap* heap,
                                     BinaryWriterBase* writer)
    : heap_(heap), writer_(writer) {}

void StringTableDumper::DumpAll() {
  v8::internal::Isolate* isolate = heap_->isolate();
  v8::internal::StringTable* string_table = isolate->string_table();
  VisitStringTableInfo stringTableInfo(heap_, writer_);
  string_table->IterateElements(&stringTableInfo);
}

void StringTableDumper::PreVisit() {
  v8::internal::Isolate* isolate = heap_->isolate();
  v8::internal::StringTable* string_table = isolate->string_table();
  string_count_ = string_table->NumberOfElements();
  string_table_dump_size_ = string_count_ * (SIZE_OF_ADDR + SIZE_OF_LENGTH);
}

uint32_t StringTableDumper::StringCount() const {
  return string_count_;
}

uint32_t StringTableDumper::StringTableDumpSize() const {
  return string_table_dump_size_;
}

}  // namespace dfx
#endif
