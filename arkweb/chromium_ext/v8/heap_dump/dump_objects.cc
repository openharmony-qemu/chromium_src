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
#include "dump_objects.h"

#include <atomic>
#include <chrono>
#include <cinttypes>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "src/common/ptr-compr.h"
#include "src/heap/memory-chunk.h"

#ifdef OH_ENABLE_HEAP_DUMP_TEST
#include "ut/object_visit_helper.h"
#endif

namespace dfx {
ObjectDumper::ObjectDumper(v8::internal::Heap* heap, BinaryWriterBase* writer)
    : safepoint_scope_(std::make_unique<v8::internal::SafepointScope>(
          heap->isolate(),
          v8::internal::kGlobalSafepointForSharedSpaceIsolate)),
      heap_(heap),
      writer_(writer) {
  heap_->MakeHeapIterable();
}

void ObjectDumper::Dump() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start_ =
      std::chrono::high_resolution_clock::now();

  v8::internal::CombinedHeapObjectIterator iterator(heap_);

  v8::internal::PtrComprCageBase cage_base(heap_->isolate());
  for (i::Tagged<i::HeapObject> obj = iterator.Next(); !obj.is_null();
       obj = iterator.Next()) {
    v8::internal::InstanceType instance_type =
        obj->map(cage_base)->instance_type();
    dfx::Address object_address = obj->address();
    uint32_t object_size = obj->Size();

    switch (instance_type) {
      case v8::internal::InstanceType::FREE_SPACE_TYPE:
      case v8::internal::InstanceType::WASM_NULL_TYPE:
        object_size = sizeof(v8::internal::Tagged_t);  // copy map pointer
        break;
      default:
        break;
    }
    ////////// dump
    // address
    writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&object_address),
                           sizeof(object_address));
    // size
    writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&object_size),
                           sizeof(object_size));
    // object
    writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(object_address),
                           static_cast<uint32_t>(object_size));

#ifdef OH_ENABLE_HEAP_DUMP_TEST
    if (auto map_obj = obj->map(cage_base);
        !heap_objects_.count(map_obj.address()) ||
        !heap_objects_[map_obj.address()]) {
      std::stringstream ss;
      heap_objects_[map_obj.address()] = true;
      ss << "--------this map not in Combined heap.";
      ss << "[HeapDump]address:" << object_address
         << " dump_size:" << object_size
         << " object end:" << (object_address + object_size) << " ";

      ss << "map.ptr():" << obj->map(cage_base).ptr() << " "
         << "instance type:"
         << i::ToString(obj->map(cage_base)->instance_type()) << " ";
      v8::internal::MemoryChunk* memory_chunk =
          v8::internal::MemoryChunk::FromAddress(map_obj.address());
      v8::internal::AllocationSpace allocation_space =
          v8::internal::MutablePageMetadata::cast(memory_chunk->Metadata())
              ->owner_identity();
      ss << " allocation space:" << i::ToString(allocation_space);
      LogInfo(ss.str());
    }
#endif
  }

#ifdef OH_ENABLE_HEAP_DUMP_TEST
  CHECK(object_cnt_ == heap_objects_.size());
#endif

  auto end = std::chrono::high_resolution_clock::now();
  auto duration_ns = duration_cast<nanoseconds>(end - start_).count();
  std::stringstream ss;
  ss << "ObjectDumper used time: " << duration_ns
     << " object_cnt: " << object_cnt_
     << " total object size: " << total_object_size_ << std::endl;
  LogInfo(ss.str());
}

void ObjectDumper::PreVisit() {
  v8::internal::CombinedHeapObjectIterator iterator(heap_);
  v8::internal::PtrComprCageBase cage_base(heap_->isolate());

  for (i::Tagged<i::HeapObject> obj = iterator.Next(); !obj.is_null();
       obj = iterator.Next()) {
    v8::internal::InstanceType instance_type =
        obj->map(cage_base)->instance_type();
    object_cnt_++;
    if (instance_type == v8::internal::InstanceType::WASM_NULL_TYPE) {
      total_object_size_ += sizeof(v8::internal::Tagged_t);
    } else {
      total_object_size_ += obj->Size();
    }

#ifdef OH_ENABLE_HEAP_DUMP_TEST
    // only for tests
    heap_objects_[obj.address()] = true;
#endif
  }
  std::stringstream ss;
  ss << "ObjectDumper PreVisit: "
     << " object_cnt: " << object_cnt_
     << " total object size: " << ObjectDumpSize() << std::endl;
  LogInfo(ss.str());
}

uint32_t ObjectDumper::ObjectCount() const {
  return object_cnt_;
}

uint32_t ObjectDumper::ObjectDumpSize() const {
  return total_object_size_ +
         object_cnt_ * (sizeof(dfx::Address) + sizeof(uint32_t));
}

}  // namespace dfx
#endif
