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
    (defined(USING_OHOS) || defined(OH_ENABLE_HEAP_DUMP_TEST))
#include "translate_objects.h"
#include "heap_dump/dump_format.h"

#include <iostream>
#include <limits>

#include "src/base/logging.h"
#include "src/codegen/reloc-info.h"
#include "src/common/globals.h"
#include "src/common/ptr-compr.h"
#include "src/heap/heap-visitor-inl.h"
#include "src/heap/heap-visitor.h"
#include "src/objects/heap-object.h"
#include "src/objects/instruction-stream.h"
#include "src/objects/objects-inl.h"
#include "src/objects/objects.h"
#include "src/objects/slots.h"
#include "src/objects/visitors.h"

#include "v8_ohlog.h"
#if defined(OH_ENABLE_HEAP_DUMP_TEST)
#include "src/d8/d8.h"
#endif

namespace v8::internal {
// see ObjectVisitorForwarder in visit-object.cc for more info
class HeapObjectVisitor final : public HeapVisitor<HeapObjectVisitor> {
 public:
  explicit HeapObjectVisitor(i::ObjectVisitor* visitor,
                             i::PtrComprCageBase cage_base,
                             i::PtrComprCageBase code_cage_base)
      : HeapVisitor(cage_base, code_cage_base), visitor_(visitor) {}

  // the rest is copied from ObjectVisitorForwarder
  static constexpr bool ShouldVisitMapPointer() { return false; }
  static constexpr bool ShouldUseUncheckedCast() { return true; }
  static constexpr bool ShouldVisitFullJSObject() { return true; }

  void VisitPointers(i::Tagged<i::HeapObject> host,
                     i::ObjectSlot start,
                     i::ObjectSlot end) override {
    visitor_->VisitPointers(host, start, end);
  }

  void VisitPointers(i::Tagged<i::HeapObject> host,
                     i::MaybeObjectSlot start,
                     i::MaybeObjectSlot end) override {
    visitor_->VisitPointers(host, start, end);
  }

  void VisitInstructionStreamPointer(i::Tagged<i::Code> host,
                                     i::InstructionStreamSlot slot) override {
    visitor_->VisitInstructionStreamPointer(host, slot);
  }

  void VisitCustomWeakPointers(i::Tagged<i::HeapObject> host,
                               i::ObjectSlot start,
                               i::ObjectSlot end) override {
    visitor_->VisitCustomWeakPointers(host, start, end);
  }

  void VisitPointer(i::Tagged<i::HeapObject> host,
                    i::ObjectSlot slot) override {
    visitor_->VisitPointers(host, slot, slot + 1);
  }

  void VisitPointer(i::Tagged<i::HeapObject> host,
                    i::MaybeObjectSlot slot) override {
    visitor_->VisitPointers(host, slot, slot + 1);
  }

  void VisitCustomWeakPointer(i::Tagged<i::HeapObject> host,
                              i::ObjectSlot slot) override {
    visitor_->VisitCustomWeakPointer(host, slot);
  }

  void VisitCodeTarget(i::Tagged<i::InstructionStream> host,
                       i::RelocInfo* rinfo) override {
    visitor_->VisitCodeTarget(host, rinfo);
  }

  void VisitEmbeddedPointer(i::Tagged<i::InstructionStream> host,
                            i::RelocInfo* rinfo) override {
    visitor_->VisitEmbeddedPointer(host, rinfo);
  }

  void VisitExternalReference(i::Tagged<i::InstructionStream> host,
                              i::RelocInfo* rinfo) override {
    visitor_->VisitExternalReference(host, rinfo);
  }

  void VisitInternalReference(i::Tagged<i::InstructionStream> host,
                              i::RelocInfo* rinfo) override {
    visitor_->VisitInternalReference(host, rinfo);
  }

  void VisitOffHeapTarget(i::Tagged<i::InstructionStream> host,
                          i::RelocInfo* rinfo) override {
    visitor_->VisitOffHeapTarget(host, rinfo);
  }

  void VisitExternalPointer(i::Tagged<i::HeapObject> host,
                            i::ExternalPointerSlot slot) override {
    visitor_->VisitExternalPointer(host, slot);
  }

  void VisitCppHeapPointer(i::Tagged<i::HeapObject> host,
                           i::CppHeapPointerSlot slot) override {
    visitor_->VisitCppHeapPointer(host, slot);
  }

  void VisitEphemeron(i::Tagged<i::HeapObject> host,
                      int index,
                      i::ObjectSlot key,
                      i::ObjectSlot value) override {
    visitor_->VisitEphemeron(host, index, key, value);
  }

  void VisitIndirectPointer(i::Tagged<i::HeapObject> host,
                            i::IndirectPointerSlot slot,
                            i::IndirectPointerMode mode) override {
    visitor_->VisitIndirectPointer(host, slot, mode);
  }

  void VisitProtectedPointer(i::Tagged<i::TrustedObject> host,
                             i::ProtectedPointerSlot slot) override {
    visitor_->VisitProtectedPointer(host, slot);
  }

  void VisitTrustedPointerTableEntry(i::Tagged<i::HeapObject> host,
                                     i::IndirectPointerSlot slot) override {
    visitor_->VisitTrustedPointerTableEntry(host, slot);
  }

  void VisitJSDispatchTableEntry(i::Tagged<i::HeapObject> host,
                                 i::JSDispatchHandle handle) override {
    visitor_->VisitJSDispatchTableEntry(host, handle);
  }

  void VisitMapPointer(i::Tagged<i::HeapObject> host) override {
    UNREACHABLE();
  }

 private:
  ObjectVisitor* const visitor_;
};
}  // namespace v8::internal

namespace dfx {

using namespace v8;

//////////////////// ObjectDetailVisitor
// see IndexedReferencesExtractor in heap-snapshot-generator.cc
class ObjectDetailVisitor : public i::ObjectVisitorWithCageBases {
 public:
  explicit ObjectDetailVisitor(ObjectTranslator* translator,
                               i::Address parent_address,
                               i::PtrComprCageBase cage_base,
                               i::PtrComprCageBase code_cage_base)
      : i::ObjectVisitorWithCageBases(cage_base, code_cage_base),
        translator_(translator),
        parent_address_(parent_address),
        cage_base_(cage_base),
        code_cage_base_(code_cage_base) {}

  void VisitMapPointer(i::Tagged<i::HeapObject> host) override {}

  void VisitPointers(i::Tagged<i::HeapObject> host,
                     i::ObjectSlot start,
                     i::ObjectSlot end) override {
    VisitPointers(host, i::MaybeObjectSlot(start), i::MaybeObjectSlot(end));
  }

  void VisitPointers(i::Tagged<i::HeapObject> host,
                     i::MaybeObjectSlot start,
                     i::MaybeObjectSlot end) override {
    for (i::MaybeObjectSlot slot = start; slot < end; ++slot) {
      VisitSlotImpl(host, cage_base_, slot);
    }
  }

  // code.h
  void VisitInstructionStreamPointer(i::Tagged<i::Code> host,
                                     i::InstructionStreamSlot slot) override {
    // slot maybe off heap, in heap-snapshot-generator.cc, VisitSlotImpl will
    // ignore the off-heap object
    VisitSlotImpl(host, code_cage_base_, slot);
  }

  // class InstructionStream::BodyDescriptor in objects-body-descriptors-inl.h
  // will call it.
  void VisitCodeTarget(i::Tagged<i::InstructionStream> host,
                       i::RelocInfo* rinfo) override {
    UNREACHABLE();
  }
  // same as VisitCodeTarget
  void VisitEmbeddedPointer(i::Tagged<i::InstructionStream> host,
                            i::RelocInfo* rinfo) override {
    UNREACHABLE();
  }

  // IterateTrustedPointer will call this. IndirectPointerSlot is a index to an
  // entry in a pointer table.
  void VisitIndirectPointer(i::Tagged<i::HeapObject> host,
                            i::IndirectPointerSlot slot,
                            i::IndirectPointerMode mode) override {
    // handle is a uint32_t type index
    i::IndirectPointerHandle handle = slot.Relaxed_LoadHandle();
    ResolveHandle(handle, slot.tag());
  }

  void VisitProtectedPointer(i::Tagged<i::TrustedObject> host,
                             i::ProtectedPointerSlot slot) override {}

  void VisitJSDispatchTableEntry(i::Tagged<i::HeapObject> host,
                                 i::JSDispatchHandle handle) override {}

 private:
  // see heap-snapshot-generator.cc IndexedReferencesExtractor
  template <typename TSlot>
  void VisitSlotImpl(i::Tagged<i::HeapObject> host,
                     i::PtrComprCageBase cage_base,
                     TSlot slot) {
    // fake_object_addr
    i::Tagged<i::HeapObject> heap_object;
    // load the pointer
    auto loaded_value = slot.load(cage_base);
    if (loaded_value.GetHeapObjectIfStrong(&heap_object)) {
      VisitHeapObjectImpl(heap_object.address());
    } else if (loaded_value.GetHeapObjectIfWeak(&heap_object)) {
    }
    // here we do nothing for off-heap object and smi
  }

  void VisitHeapObjectImpl(i::Address child) {
    translator_->generator()->SetHiddenReference(parent_address_, next_index_++,
                                                 child);
  }

  // see ResolveHandle in slots-inl.h
  void ResolveHandle(i::IndirectPointerHandle handle,
                     i::IndirectPointerTag tag) {}

  ObjectTranslator* translator_;
  i::Address parent_address_;
  i::PtrComprCageBase cage_base_;
  i::PtrComprCageBase code_cage_base_;

  uint32_t next_index_{0};
};

//////////////////// FakeHeapObject
class FakeHeapObject {
 public:
  explicit FakeHeapObject(BinaryReaderBase* reader,
                          uint32_t offset,
                          uint32_t size) {
    data_ =
        reinterpret_cast<uint8_t*>(::operator new(size, std::align_val_t{8}));
    reader->ReadDataAt(offset, size, data_, size);
  }
  explicit FakeHeapObject(BinaryReaderBase* reader, uint32_t size) {
    data_ =
        reinterpret_cast<uint8_t*>(::operator new(size, std::align_val_t{8}));
    reader->ReadData(size, data_, size);
  }
  ~FakeHeapObject() {
    if (data_ != nullptr) {
      ::operator delete(data_, std::align_val_t{8});
      data_ = nullptr;
    }
  }
  i::Address fake_address() {
    i::Address addr = reinterpret_cast<i::Address>(data_);
    CHECK(!(addr & 0x7));
    return addr;
  }
  i::Tagged<i::HeapObject> fake_object() {
    return i::HeapObject::FromAddress(fake_address());
  }

 private:
  uint8_t* data_{nullptr};
};

//////////////////// ObjectTranslator
ObjectTranslator::ObjectTranslator(BinaryReaderBase* reader,
                                   uint32_t offset,
                                   uint32_t size,
                                   uint32_t count,
                                   i::PtrComprCageBase cage_base,
                                   i::PtrComprCageBase code_cage_base,
                                   SnapshotGenerator* generator)
    : reader_(reader),
      table_offset_(offset),
      table_size_(size),
      object_count_(count),
      cage_base_(cage_base),
      code_cage_base_(code_cage_base),
      generator_(generator) {
  CHECK(reader->CurrentPosition() == offset);
  CHECK(size < std::numeric_limits<uint32_t>::max() - offset &&
        size + offset <= reader->BinarySize());
}

void ObjectTranslator::PreVisit() {
  std::cout << "PreVisit\n";
  // a raw object = address + size + map_pointer + [payload]
  const uint32_t MinSizeOfRawObject =
      sizeof(i::Address) + sizeof(uint32_t) + sizeof(v8::internal::Tagged_t);
  CHECK(object_count_ == 0 || table_size_ >= MinSizeOfRawObject);
  uint32_t table_end = table_size_ + table_offset_;

  // objects
  for (uint32_t i = 0; i < object_count_; i++) {
    CHECK(reader_->CurrentPosition() <= table_end - MinSizeOfRawObject);
    // read address and size
    i::Address address{0};
    reader_->ReadData(sizeof(address), reinterpret_cast<uint8_t*>(&address), sizeof(address));
    uint32_t dump_size{0};
    reader_->ReadData(sizeof(dump_size),
                      reinterpret_cast<uint8_t*>(&dump_size),
                      sizeof(dump_size));
    if (auto [it, inserted] = objects_head_.try_emplace(
            address,
            RawHeapObjectHead{static_cast<uint32_t>(reader_->CurrentPosition()),
                              dump_size, false});
        !inserted) {
      std::cout << "addr " << address << " size " << dump_size << " offset "
                << reader_->CurrentPosition() << std::endl;
      UNREACHABLE();
    }
    CHECK(dump_size <= table_end - reader_->CurrentPosition());
    reader_->SetPosition(reader_->CurrentPosition() + dump_size);
  }

#if defined(OH_ENABLE_HEAP_DUMP_TEST)
  if (Shell::options.log_heapdump) {
    std::cout << "HeapDump after PreVisit: objects_head_.size"
              << objects_head_.size() << "\n";
    std::cout << "HeapDump after PreVisit: object_count_" << object_count_
              << "\n";
    std::cout << "HeapDump after PreVisit: table_end" << table_end << "\n";
    std::cout << "HeapDump after PreVisit: reader_->CurrentPosition()"
              << reader_->CurrentPosition() << "\n";
  }
#endif

  CHECK(objects_head_.size() == object_count_);
  CHECK(table_end == reader_->CurrentPosition());

  // maps
  reader_->SetPosition(table_offset_);
  for (uint32_t i = 0; i < object_count_; i++) {
    // read address and size
    i::Address address{0};
    reader_->ReadData(sizeof(address), reinterpret_cast<uint8_t*>(&address), sizeof(address));
    uint32_t dump_size{0};
    reader_->ReadData(sizeof(dump_size),
                      reinterpret_cast<uint8_t*>(&dump_size),
                      sizeof(dump_size));

    CHECK(dump_size <= table_end - reader_->CurrentPosition());
    FakeHeapObject fake_ho(reader_, dump_size);
    i::Tagged<i::HeapObject> fake_heap_object = fake_ho.fake_object();

    ////////// map
    i::Tagged<i::Map> map = fake_heap_object->map(cage_base_);

    if (auto [it, inserted] = objects_head_.try_emplace(
            map.address(), RawHeapObjectHead{0, 0, true});
        !inserted) {
      it->second.is_map_ = true;
    } else {
      std::cout << "map not in heap: address " << map.address() << std::endl;
      UNREACHABLE();
    }

    Node* n =
        generator_->AddNode(address, Node::Type::kObject, "<dummy>", dump_size);
  }
}

void ObjectTranslator::Translate() {
  PreVisit();
  reader_->SetPosition(table_offset_);

  std::cout << "Translate\n";
  for (uint32_t i = 0; i < object_count_; i++) {
    // has beed checked in PreVisit()
    i::Address address{0};
    CHECK(reader_->ReadData(sizeof(address),
                            reinterpret_cast<uint8_t*>(&address),
                            sizeof(address)));
    uint32_t dump_size{0};
    CHECK(reader_->ReadData(sizeof(dump_size),
                            reinterpret_cast<uint8_t*>(&dump_size),
                            sizeof(dump_size)));
#if defined(OH_ENABLE_HEAP_DUMP_TEST)
    if (Shell::options.log_heapdump) {
      std::cout << "address:" << address << " dump_size:" << dump_size
                << " object end:" << (address + dump_size) << std::endl;
    }
#endif
    CHECK(objects_head_.count(address));
    CHECK(objects_head_[address].offset_ == reader_->CurrentPosition());
    CHECK(objects_head_[address].size_ == dump_size);

    TranslateHeapObject(address, dump_size);
  }
}

void ObjectTranslator::TranslateHeapObject(i::Address addr, uint32_t size) {
  FakeHeapObject fake_ho(reader_, size);
  i::Tagged<i::HeapObject> fake_heap_object = fake_ho.fake_object();

  ////////// map
  i::Tagged<i::Map> map = fake_heap_object->map(cage_base_);

  CHECK(objects_head_.count(map.address()));
  RawHeapObjectHead map_head = objects_head_[map.address()];

#if defined(OH_ENABLE_HEAP_DUMP_TEST)
  CHECK(map_head.is_map_);
#endif

  FakeHeapObject fake_map_obj(reader_, map_head.offset_, map_head.size_);
  i::Tagged<i::Map> fake_map = i::Cast<i::Map>(fake_map_obj.fake_object());
#if defined(OH_ENABLE_HEAP_DUMP_TEST)
  if (Shell::options.log_heapdump) {
    std::cout << "instance type:" << i::ToString(fake_map->instance_type())
              << " " << std::endl;
  }
#endif

  ////////// visit object
  i::VisitorId visitor_id = fake_map->visitor_id();
  if (i::Map::ObjectFieldsFrom(visitor_id) == i::ObjectFields::kDataOnly) {
    return;
  }
  switch (visitor_id) {
    case i::VisitorId::kVisitExternalString:
    case i::VisitorId::kVisitWasmNull:
    case i::VisitorId::kVisitInstructionStream:
      return;
    default:
      break;
  }

  ObjectDetailVisitor visitor(this, addr, cage_base_, code_cage_base_);
  i::HeapObjectVisitor heap_object_visitor(&visitor, cage_base_,
                                           code_cage_base_);
  heap_object_visitor.Visit(fake_map, fake_heap_object);
}

}  // namespace dfx

#endif
