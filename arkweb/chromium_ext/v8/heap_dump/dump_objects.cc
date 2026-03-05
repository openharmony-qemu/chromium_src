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
#include "arkweb/chromium_ext/v8/heap_dump/dump_objects.h"

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
#include "src/heap/memory-chunk.h"
#endif

// dump_objects-inl.h must be the last one
#include "dump_objects-inl.h"

namespace dfx {

static uint32_t GetDumpSize(i::Tagged<i::HeapObject> obj) {
  switch (obj->map()->instance_type()) {
    case i::InstanceType::FREE_SPACE_TYPE:
    case i::InstanceType::FILLER_TYPE:
      return 0;
    default:
      // NIY:for Data Only Object, the dump format is
      /* type          |          description
        Address        |      address of heapobject
        uint32_t       |           dump size
        tagged_t       |           map pointer
        uint32_t       |           real size
      */
      if (i::Map::ObjectFieldsFrom(obj->map()->visitor_id()) ==
          i::ObjectFields::kDataOnly) {
        return sizeof(i::Tagged_t);  // copy map pointer
      }
      return obj->Size();
  }
}

ObjectDumper::ObjectDumper(v8::internal::Heap* heap, BinaryWriterBase* writer)
    : safepoint_scope_(std::make_unique<v8::internal::SafepointScope>(
          heap->isolate(),
          v8::internal::kGlobalSafepointForSharedSpaceIsolate)),
      heap_(heap),
      writer_(writer) {
  heap_->MakeHeapIterable();
}

void ObjectDumper::WriteInstructionStreamInfo(
    v8::internal::Tagged<v8::internal::HeapObject> object) {
  DumpObjectVisitor visitor(
      v8::internal::PtrComprCageBase(heap_->isolate()->cage_base()),
      v8::internal::PtrComprCageBase(heap_->isolate()->code_cage_base()), this);
  v8::internal::Tagged<v8::internal::TrustedObject> host =
      Cast<v8::internal::TrustedObject>(object);
  // same as PreVisitInstructionStream
  visitor.VisitProtectedPointer(
      host, host->RawProtectedPointerField(
                v8::internal::InstructionStream::kCodeOffset));
  visitor.VisitProtectedPointer(
      host, host->RawProtectedPointerField(
                v8::internal::InstructionStream::kRelocationInfoOffset));
  v8::internal::Address object_address = object->address();
  auto it = InstructionStreamMap_.find(object_address);
  if (it != InstructionStreamMap_.end()) {
    const std::vector<InstructionStreamInfo>& vec = it->second;
    for (const auto& info : vec) {
      v8::internal::Address addr = info.object_addr_;
      int8_t mode = info.mode_;
      writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&addr), sizeof(addr));
      writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&mode), sizeof(mode));
    }
  }
}

void ObjectDumper::IterateInstructionStream(
    v8::internal::Tagged<v8::internal::HeapObject> object) {
  // address
  v8::internal::Address object_address = object->address();
  writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&object_address),
                         sizeof(object_address));
  // size
  //  mapPointer + 2 * protectedPointer (objects-body-descriptors-inl.h: class
  //  InstructionStream::BodyDescripter)
  uint32_t prefix_size =
      sizeof(v8::internal::Tagged_t) + 2 * sizeof(v8::internal::Address);
  uint32_t objcet_size =
      static_cast<uint32_t>(InstructionStreamMap_[object_address].size()) *
      (sizeof(v8::internal::Address) + sizeof(int8_t));
  objcet_size += prefix_size;
  writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&objcet_size),
                         sizeof(objcet_size));
  // map pointer
  writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(object_address),
                         sizeof(v8::internal::Tagged_t));
  // content
  WriteInstructionStreamInfo(object);
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
    switch (instance_type) {
      case v8::internal::InstanceType::INSTRUCTION_STREAM_TYPE: {
        IterateInstructionStream(obj);
        break;
      }
      default:
        ////////// dump
        uint32_t dump_size = GetDumpSize(obj);
        if (dump_size == 0) {
          continue;
        }
        v8::internal::Address object_address = obj->address();

        ////////// dump
        // address
        writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&object_address),
                               sizeof(object_address));
        // size
        writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&dump_size),
                               sizeof(dump_size));
        // object
        writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(object_address),
                               static_cast<uint32_t>(dump_size));

#ifdef OH_ENABLE_HEAP_DUMP_TEST
        if (auto map_obj = obj->map(cage_base);
            !heap_objects_.count(map_obj.address()) ||
            !heap_objects_[map_obj.address()]) {
          std::stringstream ss;
          heap_objects_[map_obj.address()] = true;
          ss << "--------this map not in Combined heap.";
          ss << "[HeapDump]address:" << object_address
            << " dump_size:" << dump_size
            << " object end:" << (object_address + dump_size) << " ";

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
        break;
    }
  }
#ifdef OH_ENABLE_HEAP_DUMP_TEST
  CHECK(object_cnt_ == heap_objects_.size());
#endif
  auto end = std::chrono::high_resolution_clock::now();
  auto duration_ns = duration_cast<nanoseconds>(end - start_).count();
  std::stringstream ss;
  ss << "[HeapDump]ObjectDumper used time: " << duration_ns << " ns "
     << " object_cnt: " << object_cnt_
     << " total object size: " << total_object_size_ << std::endl;
  LogInfo(ss.str());
}

void ObjectDumper::AddInstructionStreamInfo(v8::internal::Address addr,
                                            InstructionStreamInfo info) {
  InstructionStreamMap_[addr].push_back(info);
}

uint32_t ObjectDumper::PreVisitInstructionStream(
    v8::internal::Tagged<v8::internal::HeapObject> object) {
  // mapPointer + 2 * protectedPointer (objects-body-descriptors-inl.h: class
  // InstructionStream::BodyDescripter)
  uint32_t prefix_size =
      sizeof(v8::internal::Tagged_t) + 2 * sizeof(v8::internal::Address);
  int kRelocModeMask =
      v8::internal::RelocInfo::ModeMask(v8::internal::RelocInfo::CODE_TARGET) |
      v8::internal::RelocInfo::ModeMask(
          v8::internal::RelocInfo::RELATIVE_CODE_TARGET) |
      v8::internal::RelocInfo::ModeMask(
          v8::internal::RelocInfo::FULL_EMBEDDED_OBJECT) |
      v8::internal::RelocInfo::ModeMask(
          v8::internal::RelocInfo::COMPRESSED_EMBEDDED_OBJECT) |
      v8::internal::RelocInfo::ModeMask(
          v8::internal::RelocInfo::EXTERNAL_REFERENCE) |
      v8::internal::RelocInfo::ModeMask(
          v8::internal::RelocInfo::INTERNAL_REFERENCE) |
      v8::internal::RelocInfo::ModeMask(
          v8::internal::RelocInfo::INTERNAL_REFERENCE_ENCODED) |
      v8::internal::RelocInfo::ModeMask(
          v8::internal::RelocInfo::OFF_HEAP_TARGET) |
      v8::internal::RelocInfo::ModeMask(
          v8::internal::RelocInfo::WASM_STUB_CALL);
  v8::internal::Tagged<v8::internal::InstructionStream> istream =
      UncheckedCast<v8::internal::InstructionStream>(object);
  if (!istream->IsFullyInitialized()) {
    return prefix_size;
  }
  v8::internal::PtrComprCageBase cage_base(heap_->isolate());
  DumpObjectVisitor visitor(
      cage_base,
      v8::internal::PtrComprCageBase(heap_->isolate()->code_cage_base()), this);
  v8::internal::RelocIterator it(istream, kRelocModeMask);
  uint32_t cnt = visitor.CountRelocInfo(istream, &it);
  uint32_t total_object_size =
      cnt * (sizeof(v8::internal::Address) + sizeof(int8_t));
  return prefix_size + total_object_size;
}

void ObjectDumper::PreVisit() {
  v8::internal::CombinedHeapObjectIterator iterator(heap_);
  v8::internal::PtrComprCageBase cage_base(heap_->isolate());

  for (i::Tagged<i::HeapObject> obj = iterator.Next(); !obj.is_null();
       obj = iterator.Next()) {
    v8::internal::InstanceType instance_type =
        obj->map(cage_base)->instance_type();

    switch (instance_type) {
      case v8::internal::InstanceType::INSTRUCTION_STREAM_TYPE:
        total_object_size_ += PreVisitInstructionStream(obj);
        object_cnt_++;
        break;
      default:
        if (uint32_t size = GetDumpSize(obj); size) {
          object_cnt_++;
          total_object_size_ += size;
        }
    }
    if (i::v8_flags.log_heapdump) {
      std::stringstream ss;
      ss << "ObjectDumper PreVisit: "
         << " object_cnt: " << object_cnt_
         << " total object size: " << ObjectDumpSize() << std::endl;
      LogInfo(ss.str());
    }
#ifdef OH_ENABLE_HEAP_DUMP_TEST
    // only for tests
    heap_objects_[obj.address()] = true;
#endif
  }
}

uint32_t ObjectDumper::ObjectCount() const {
  return object_cnt_;
}

uint32_t ObjectDumper::ObjectDumpSize() const {
  // every object has address and size field, size field should not be 0.
  return total_object_size_ +
         object_cnt_ * (sizeof(v8::internal::Address) + sizeof(uint32_t));
}

}  // namespace dfx
#endif
