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

#if (defined(ON_ENABLE_HEAP_TRANSLATE) || defined(OH_ENABLE_HEAP_DUMP_TEST))

#include "arkweb/chromium_ext/v8/heap_dump/translator/translate_objects.h"

#include <iostream>
#include <limits>

#include "arkweb/chromium_ext/v8/heap_dump/dump_format.h"
#include "arkweb/chromium_ext/v8/v8_ohlog.h"
#include "src/base/logging.h"
#include "src/codegen/reloc-info.h"
#include "src/common/globals.h"
#include "src/common/ptr-compr.h"
#include "src/heap/heap-visitor-inl.h"
#include "src/heap/heap-visitor.h"
#include "src/init/heap-symbols.h"
#include "src/objects/elements-kind.h"
#include "src/objects/heap-object.h"
#include "src/objects/hole.h"
#include "src/objects/instance-type-inl.h"
#include "src/objects/instruction-stream.h"
#include "src/objects/js-regexp.h"
#include "src/objects/objects-inl.h"
#include "src/objects/objects.h"
#include "src/objects/oddball.h"
#include "src/objects/slots.h"
#include "src/objects/visitors.h"
#include "src/roots/roots.h"

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

  void VisitProtectedPointer(i::Tagged<i::TrustedObject> host,
                             i::ProtectedMaybeObjectSlot slot) override {
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

#define STRING_METHOD(_, name, value) \
  std::string name() const { return value; }

  INTERNALIZED_STRING_LIST_GENERATOR(STRING_METHOD, _)
#undef STRING_METHOD

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

  void VisitMapPointer(i::Tagged<i::HeapObject> host) override {
    VisitSlotImpl(host, cage_base_, host->map_slot());
  }

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
  // NIY: if use DCHECK, it will fail before this visit
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
    // NIY : if we want to get field_index, we must pass the
    // fake_object_addr
    i::Tagged<i::HeapObject> heap_object;
    // load the pointer
    auto loaded_value = slot.load(cage_base);
    if (loaded_value.GetHeapObjectIfStrong(&heap_object)) {
      VisitHeapObjectImpl(heap_object.address());
    } else if (loaded_value.GetHeapObjectIfWeak(&heap_object)) {
      translator_->generator()->SetWeakReference(parent_address_, next_index_++,
                                                 heap_object.address(), {});
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
    CHECK(data_);
    CHECK(reader->ReadDataAt(offset, size, data_, size));
  }
  explicit FakeHeapObject(BinaryReaderBase* reader, uint32_t size) {
    data_ =
        reinterpret_cast<uint8_t*>(::operator new(size, std::align_val_t{8}));
    CHECK(data_);
    CHECK(reader->ReadData(size, data_, size));
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
    CHECK(reader_->ReadData(sizeof(address),
                            reinterpret_cast<uint8_t*>(&address),
                            sizeof(address)));
    uint32_t dump_size{0};
    CHECK(reader_->ReadData(sizeof(dump_size),
                            reinterpret_cast<uint8_t*>(&dump_size),
                            sizeof(dump_size)));
    if (auto [it, inserted] = objects_head_.try_emplace(
            address,
            RawHeapObjectHead{static_cast<uint32_t>(reader_->CurrentPosition()),
                              dump_size
#ifdef OH_ENABLE_HEAP_DUMP_TEST
                              ,
                              false
#endif
            });
        !inserted) {
      std::cout << "HeapDump: same address insert twice addr " << address
                << " size " << dump_size << " offset "
                << reader_->CurrentPosition() << std::endl;
      UNREACHABLE();
    }
    CHECK(dump_size <= table_end - reader_->CurrentPosition());
    reader_->SetPosition(reader_->CurrentPosition() + dump_size);
  }

#if defined(OH_ENABLE_HEAP_DUMP_TEST)
  if (i::v8_flags.log_heapdump) {
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
    CHECK(reader_->ReadData(sizeof(address),
                            reinterpret_cast<uint8_t*>(&address),
                            sizeof(address)));
    uint32_t dump_size{0};
    CHECK(reader_->ReadData(sizeof(dump_size),
                            reinterpret_cast<uint8_t*>(&dump_size),
                            sizeof(dump_size)));

    CHECK(dump_size <= table_end - reader_->CurrentPosition());
    FakeHeapObject fake_ho(reader_, dump_size);
    i::Tagged<i::HeapObject> fake_heap_object = fake_ho.fake_object();

    ////////// map
    i::Tagged<i::Map> map = fake_heap_object->map(cage_base_);

#ifdef OH_ENABLE_HEAP_DUMP_TEST
    if (auto [it, inserted] = objects_head_.try_emplace(
            map.address(), RawHeapObjectHead{0, 0, true});
        !inserted) {
      it->second.is_map_ = true;
    } else {
      std::cout << "HeapDump map not in heap: address " << map.address()
                << std::endl;
      UNREACHABLE();
    }
#endif
    FakeHeapObject fake_map_ho(reader_, objects_head_[map.address()].offset_,
                               objects_head_[map.address()].size_);
    i::Tagged<i::Map> fake_map = UncheckedCast<i::Map>(fake_map_ho.fake_object());
    // NIY: dump_size is dump size, does it same as self size?
    // NIY: maybe we should visit from root
    if (!AddNode(fake_map, fake_heap_object, address, dump_size)) {
      generator_->AddNode(address, Node::Type::kObject, "<dummy>", dump_size);
    }
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
    if (i::v8_flags.log_heapdump) {
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
  i::Tagged<i::Map> fake_map = UncheckedCast<i::Map>(fake_map_obj.fake_object());
#if defined(OH_ENABLE_HEAP_DUMP_TEST)
  if (i::v8_flags.log_heapdump) {
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
      // NIY
      return;
    case i::VisitorId::kVisitInstructionStream: {
      TranslateInstructionStream(addr, size);
      return;
    }
    default:
      break;
  }

  ObjectDetailVisitor visitor(this, addr, cage_base_, code_cage_base_);
  i::HeapObjectVisitor heap_object_visitor(&visitor, cage_base_,
                                           code_cage_base_);
  // Note: not all heap object can use Visit
  heap_object_visitor.Visit(fake_map, fake_heap_object);
}

bool ObjectTranslator::IsStrong(v8::internal::Address addr) {
  if (addr & dfx::kRawHeapWeakObjectTag) {
    return false;
  }
  return true;
}

void ObjectTranslator::TranslateInstructionStream(v8::internal::Address addr,
                                                  uint32_t size) {
  uint32_t next_index = 0;
  // accroding to objects-body-descriptors-inl.h: class
  // InstructionStream::BodyDescripter total size: mapPointer + 2 *
  // protectedPointer + n * relocInfo (addr + mode)
  RawHeapObjectHead object_head = objects_head_[addr];
  v8::internal::Address protect_pointer_addr{0};
  uint32_t offset = object_head.offset_ + sizeof(v8::internal::Tagged_t);
  reader_->ReadDataAt(offset, sizeof(v8::internal::Address),
                      reinterpret_cast<uint8_t*>(&protect_pointer_addr),
                      sizeof(v8::internal::Address));
  this->generator()->SetHiddenReference(addr, next_index++,
                                        protect_pointer_addr);

  protect_pointer_addr = 0;
  offset += sizeof(v8::internal::Address);
  reader_->ReadDataAt(offset, sizeof(v8::internal::Address),
                      reinterpret_cast<uint8_t*>(&protect_pointer_addr),
                      sizeof(v8::internal::Address));
  this->generator()->SetHiddenReference(addr, next_index++,
                                        protect_pointer_addr);

  offset += sizeof(v8::internal::Address);
  int reloc_info_cnt = (size - sizeof(v8::internal::Tagged_t) -
                        2 * sizeof(v8::internal::Address)) /
                       (sizeof(v8::internal::Address) + sizeof(int8_t));
  for (int i = 0; i < reloc_info_cnt; i++) {
    InstructionStreamInfo info;
    reader_->ReadDataAt(offset, sizeof(v8::internal::Address),
                        reinterpret_cast<uint8_t*>(&info.object_addr_),
                        sizeof(v8::internal::Address));
    if (IsStrong(info.object_addr_)) {
      this->generator()->SetHiddenReference(addr, next_index++,
                                            info.object_addr_);
    } else {
      // NIY (weak reference)
      this->generator()->SetHiddenReference(
          addr, next_index++, info.object_addr_ ^ dfx::kRawHeapWeakObjectTag);
    }

    offset += sizeof(v8::internal::Address);
    reader_->ReadDataAt(offset, sizeof(info.mode_),
                        reinterpret_cast<uint8_t*>(&info.mode_),
                        sizeof(info.mode_));
    offset += sizeof(int8_t);
  }
}

// NIY
Node* ObjectTranslator::AddNode(i::Tagged<i::Map> map,
                                i::Tagged<i::HeapObject> object,
                                i::Address address,
                                uint32_t size) {
  i::InstanceType instance_type = map->instance_type();
  if (i::InstanceTypeChecker::IsJSObject(instance_type)) {
    if (i::InstanceTypeChecker::IsJSFunction(instance_type)) {
      i::Tagged<i::JSFunction> func = UncheckedCast<i::JSFunction>(object);
      i::Tagged<i::SharedFunctionInfo> shared = func->shared();
      // NIY:Name()
      return nullptr;

    } else if (i::InstanceTypeChecker::IsJSBoundFunction(instance_type)) {
      return generator_->AddNode(address, Node::Type::kClosure, "native_bind",
                                 size);
    }
    if (i::InstanceTypeChecker::IsJSRegExp(instance_type)) {
      i::Tagged<i::JSRegExp> re = UncheckedCast<i::JSRegExp>(object);
      // NIY:re->source()
      return generator_->AddNode(address, Node::Type::kRegExp, "source", size);
    }
    // TODO(v8:12674) Fix and run full gcmole.
    i::DisableGCMole no_gcmole;
    std::string name = GetClassName(map, object, address);
    if (name.empty()) {
      return nullptr;
    }
    if (i::InstanceTypeChecker::IsJSGlobalObject(instance_type)) {
      // NIY
      return nullptr;
    }
    return generator_->AddNode(address, Node::Type::kObject, name, size);

  } else if (i::InstanceTypeChecker::IsString(instance_type)) {
    i::Tagged<i::String> string = UncheckedCast<i::String>(object);
    if (i::InstanceTypeChecker::IsConsString(instance_type)) {
      return generator_->AddNode(address, Node::Type::kConsString,
                                 "(concatenated string)", size);
    } else if (i::InstanceTypeChecker::IsSlicedString(instance_type)) {
      return generator_->AddNode(address, Node::Type::kSlicedString,
                                 "(sliced string)", size);
    } else {
      // NIY: get string name
      return generator_->AddNode(address, Node::Type::kString, "(dummy string)",
                                 size);
    }
  } else if (i::InstanceTypeChecker::IsSymbol(instance_type)) {
    if (UncheckedCast<i::Symbol>(object)->is_private()) {
      return generator_->AddNode(address, Node::Type::kHidden, "private symbol",
                                 size);
    } else {
      return generator_->AddNode(address, Node::Type::kSymbol, "symbol", size);
    }

  } else if (i::InstanceTypeChecker::IsBigInt(instance_type)) {
    return generator_->AddNode(address, Node::Type::kBigInt, "bigint", size);
  } else if (i::InstanceTypeChecker::IsInstructionStream(instance_type) ||
             i::InstanceTypeChecker::IsCode(instance_type)) {
    return generator_->AddNode(address, Node::Type::kCode, "", size);
  } else if (i::InstanceTypeChecker::IsSharedFunctionInfo(instance_type)) {
    // NIY
    return nullptr;
  } else if (i::InstanceTypeChecker::IsScript(instance_type)) {
    // NIY
    return nullptr;
  } else if (i::InstanceTypeChecker::IsNativeContext(instance_type)) {
    return generator_->AddNode(address, Node::Type::kHidden,
                               "system / NativeContext", size);
  } else if (i::InstanceTypeChecker::IsContext(instance_type)) {
    return generator_->AddNode(address, Node::Type::kObject, "system / Context",
                               size);
  } else if (i::InstanceTypeChecker::IsHeapNumber(instance_type)) {
    return generator_->AddNode(address, Node::Type::kHeapNumber, "heap number",
                               size);
  }
#if V8_ENABLE_WEBASSEMBLY
  if (i::InstanceTypeChecker::IsWasmObject(instance_type)) {
    // NIY
    return nullptr;
  }
  if (i::InstanceTypeChecker::IsWasmNull(instance_type)) {
    // Inlined copies of {GetSystemEntryType}, {GetSystemEntryName}, and
    // {AddEntry}, allowing us to override the size.
    // The actual object's size is fairly large (at the time of this writing,
    // just over 64 KB) and mostly includes a guard region. We report it as
    // much smaller to avoid confusion.
    static constexpr size_t kSize = i::WasmNull::kHeaderSize;
    return generator_->AddNode(address, Node::Type::kHidden,
                               "system / WasmNull", kSize);
  }
#endif  // V8_ENABLE_WEBASSEMBLY

  if (i::InstanceTypeChecker::IsForeign(instance_type)) {
    // NIY
    return nullptr;
  }

  // NIY
  return nullptr;
}

// According to file src\objects\js-objects.cc:JSReceiver::class_name()
std::string ObjectTranslator::GetClassName(i::Tagged<i::Map> map,
                                           i::Tagged<i::HeapObject> object,
                                           i::Address address) {
  i::InstanceType instance_type = map->instance_type();
  ObjectDetailVisitor visitor(this, address, cage_base_, code_cage_base_);
  i::HeapObjectVisitor heap_object_visitor(&visitor, cage_base_,
                                           code_cage_base_);

  if (i::InstanceTypeChecker::IsJSFunctionOrBoundFunctionOrWrappedFunction(
          instance_type)) {
    return heap_object_visitor.Function_string();
  }
  if (i::InstanceTypeChecker::IsJSArgumentsObject(instance_type)) {
    return heap_object_visitor.Arguments_string();
  }
  if (i::InstanceTypeChecker::IsJSArray(instance_type)) {
    return heap_object_visitor.Array_string();
  }
  if (i::InstanceTypeChecker::IsJSArrayBuffer(instance_type)) {
    // NIY:is_shared()
    return heap_object_visitor.ArrayBuffer_string();
  }
  if (i::InstanceTypeChecker::IsJSArrayIterator(instance_type)) {
    return heap_object_visitor.ArrayIterator_string();
  }
  if (i::InstanceTypeChecker::IsJSDate(instance_type)) {
    return heap_object_visitor.Date_string();
  }
  if (i::InstanceTypeChecker::IsJSError(instance_type)) {
    return heap_object_visitor.Error_string();
  }
  if (i::InstanceTypeChecker::IsJSGeneratorObject(instance_type)) {
    return heap_object_visitor.Generator_string();
  }
  if (i::InstanceTypeChecker::IsJSMap(instance_type)) {
    return heap_object_visitor.Map_string();
  }
  if (i::InstanceTypeChecker::IsJSMapIterator(instance_type)) {
    return heap_object_visitor.MapIterator_string();
  }
  if (i::InstanceTypeChecker::IsJSProxy(instance_type)) {
    return map->is_callable() ? heap_object_visitor.Function_string()
                              : heap_object_visitor.Object_string();
  }
  if (i::InstanceTypeChecker::IsJSRegExp(instance_type)) {
    return heap_object_visitor.RegExp_string();
  }
  if (i::InstanceTypeChecker::IsJSSet(instance_type)) {
    return heap_object_visitor.Set_string();
  }
  if (i::InstanceTypeChecker::IsJSSetIterator(instance_type)) {
    return heap_object_visitor.SetIterator_string();
  }
  if (i::InstanceTypeChecker::IsJSTypedArray(instance_type)) {
#define SWITCH_STRING(Type, type, TYPE, ctype)       \
  if (map->elements_kind() == i::TYPE##_ELEMENTS) {  \
    return heap_object_visitor.Type##Array_string(); \
  }
    TYPED_ARRAYS(SWITCH_STRING)
#undef SWITCH_STRING
  }

  if (i::InstanceTypeChecker::IsJSPrimitiveWrapper(instance_type)) {
    i::Tagged<i::Object> value =
        i::TaggedField<i::Tagged<i::JSPrimitiveWrapper>>::load(
            cage_base_, object, i::JSPrimitiveWrapper::kValueOffset);
    if (i::IsSmi(value)) {
      return heap_object_visitor.Number_string();
    }
    if (i::IsHeapObject(value)) {
      i::Tagged<i::HeapObject> heap_object = Cast<i::HeapObject>(value);
      FakeHeapObject fObject(reader_,
                             objects_head_[heap_object.address()].offset_,
                             objects_head_[heap_object.address()].size_);
      i::Tagged<i::Map> fmap = UncheckedCast<i::Map>(fObject.fake_object());

      FakeHeapObject fake_map_ho(reader_, objects_head_[fmap.address()].offset_,
                                 objects_head_[fmap.address()].size_);
      i::Tagged<i::Map> fake_map = UncheckedCast<i::Map>(fake_map_ho.fake_object());
      i::InstanceType type = fake_map->instance_type();
      if (IsBoolean(type, value)) {
        return heap_object_visitor.Boolean_string();
      }
      if (i::InstanceTypeChecker::IsString(type)) {
        return heap_object_visitor.String_string();
      }
      if (i::InstanceTypeChecker::IsHeapNumber(type)) {
        return heap_object_visitor.Number_string();
      }
      if (i::InstanceTypeChecker::IsBigInt(type)) {
        return heap_object_visitor.BigInt_string();
      }
      if (i::InstanceTypeChecker::IsSymbol(type)) {
        return heap_object_visitor.Symbol_string();
      }
      if (i::InstanceTypeChecker::IsScript(type)) {
        return heap_object_visitor.Script_string();
      }
    }
    // need fix to UNREACHABLE();
    return "";
  }

  if (i::InstanceTypeChecker::IsJSWeakMap(instance_type)) {
    return heap_object_visitor.WeakMap_string();
  }
  if (i::InstanceTypeChecker::IsJSWeakSet(instance_type)) {
    return heap_object_visitor.WeakSet_string();
  }
  if (i::InstanceTypeChecker::IsJSGlobalProxy(instance_type)) {
    return heap_object_visitor.global_string();
  }
  if (IsShared(instance_type)) {
    if (i::InstanceTypeChecker::IsJSSharedStruct(instance_type)) {
      return heap_object_visitor.SharedStruct_string();
    }
    if (i::InstanceTypeChecker::IsJSSharedArray(instance_type)) {
      return heap_object_visitor.SharedArray_string();
    }
    if (i::InstanceTypeChecker::IsJSAtomicsMutex(instance_type)) {
      return heap_object_visitor.AtomicsMutex_string();
    }
    if (i::InstanceTypeChecker::IsJSAtomicsCondition(instance_type)) {
      return heap_object_visitor.AtomicsCondition_string();
    }
    // need fix to UNREACHABLE();
    return "";
  }
  return heap_object_visitor.Object_string();
}

bool ObjectTranslator::IsBoolean(
    v8::internal::InstanceType instance_type,
    v8::internal::Tagged<v8::internal::Object> obj) {
  return i::InstanceTypeChecker::IsOddball(instance_type) &&
         ((Cast<i::Oddball>(obj)->kind() & i::Oddball::kNotBooleanMask) == 0);
}

bool ObjectTranslator::IsShared(v8::internal::InstanceType instance_type) {
  // NIY IsReadOnlySpaceShared

  if (i::InstanceTypeChecker::IsAlwaysSharedSpaceJSObject(instance_type)) {
    return true;
  }
  switch (instance_type) {
    case i::SHARED_SEQ_TWO_BYTE_STRING_TYPE:
    case i::SHARED_SEQ_ONE_BYTE_STRING_TYPE:
    case i::SHARED_EXTERNAL_TWO_BYTE_STRING_TYPE:
    case i::SHARED_EXTERNAL_ONE_BYTE_STRING_TYPE:
    case i::SHARED_UNCACHED_EXTERNAL_TWO_BYTE_STRING_TYPE:
    case i::SHARED_UNCACHED_EXTERNAL_ONE_BYTE_STRING_TYPE:
      return true;
    case i::INTERNALIZED_TWO_BYTE_STRING_TYPE:
    case i::INTERNALIZED_ONE_BYTE_STRING_TYPE:
    case i::EXTERNAL_INTERNALIZED_TWO_BYTE_STRING_TYPE:
    case i::EXTERNAL_INTERNALIZED_ONE_BYTE_STRING_TYPE:
    case i::UNCACHED_EXTERNAL_INTERNALIZED_TWO_BYTE_STRING_TYPE:
    case i::UNCACHED_EXTERNAL_INTERNALIZED_ONE_BYTE_STRING_TYPE:
      if (i::v8_flags.shared_string_table) {
        return true;
      }
      return false;
    // NIY HEAP_NUMBER_TYPE-InWritableSharedSpace
    default:
      return false;
  }
}

}  // namespace dfx

#endif