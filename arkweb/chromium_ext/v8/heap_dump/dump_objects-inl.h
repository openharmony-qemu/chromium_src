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
#ifndef DUMP_OBJECTS_INL_H
#define DUMP_OBJECTS_INL_H

namespace dfx {
//////////////////// ObjectDetailVisitor
// see IndexedReferencesExtractor in heap-snapshot-generator.cc
class DumpObjectVisitor : public i::ObjectVisitorWithCageBases {
 public:
  explicit DumpObjectVisitor(i::PtrComprCageBase cage_base,
                             i::PtrComprCageBase code_cage_base,
                             dfx::ObjectDumper* dumper)
      : i::ObjectVisitorWithCageBases(cage_base, code_cage_base),
        cage_base_(cage_base),
        code_cage_base_(code_cage_base),
        dumper_(dumper) {}

  void VisitMapPointer(i::Tagged<i::HeapObject> host) override {
    UNREACHABLE();
  }

  void VisitPointers(i::Tagged<i::HeapObject> host,
                     i::ObjectSlot start,
                     i::ObjectSlot end) override {
    UNREACHABLE();
  }

  void VisitPointers(i::Tagged<i::HeapObject> host,
                     i::MaybeObjectSlot start,
                     i::MaybeObjectSlot end) override {
    UNREACHABLE();
  }

  void VisitInstructionStreamPointer(i::Tagged<i::Code> host,
                                     i::InstructionStreamSlot slot) override {
    UNREACHABLE();
  }

  void VisitCodeTarget(i::Tagged<i::InstructionStream> host,
                       i::RelocInfo* rinfo) override {
    dfx::InstructionStreamInfo info;
    i::Tagged<i::InstructionStream> target =
        i::InstructionStream::FromTargetAddress(rinfo->target_address());
    i::Address object_address = target->address();
    info.object_addr_ = object_address;
    i::RelocInfo::Mode mode = rinfo->rmode();
    info.mode_ = static_cast<int8_t>(mode);
    dumper_->AddInstructionStreamInfo(host->address(), info);
  }

  void VisitEmbeddedPointer(i::Tagged<i::InstructionStream> host,
                            i::RelocInfo* rinfo) override {
    dfx::InstructionStreamInfo info;
    i::Tagged<i::HeapObject> object = rinfo->target_object(cage_base_);
    i::Tagged<i::Code> code =
        UncheckedCast<i::Code>(host->raw_code(v8::kAcquireLoad));
    if (code->IsWeakObject(object)) {
      info.object_addr_ = object->address() | dfx::kRawHeapWeakObjectTag;
    } else {
      info.object_addr_ = object->address();
    }
    i::RelocInfo::Mode mode = rinfo->rmode();
    info.mode_ = static_cast<int8_t>(mode);
    dumper_->AddInstructionStreamInfo(host->address(), info);
  }

  void VisitExternalReference(i::Tagged<i::InstructionStream> host,
                              i::RelocInfo* rinfo) override {
    UNREACHABLE();
  }

  void VisitInternalReference(i::Tagged<i::InstructionStream> host,
                              i::RelocInfo* rinfo) override {
    UNREACHABLE();
  }

  void VisitOffHeapTarget(i::Tagged<i::InstructionStream> host,
                          i::RelocInfo* rinfo) override {
    UNREACHABLE();
  }

  void VisitIndirectPointer(i::Tagged<i::HeapObject> host,
                            i::IndirectPointerSlot slot,
                            i::IndirectPointerMode mode) override {
    UNREACHABLE();
  }

  void VisitProtectedPointer(i::Tagged<i::TrustedObject> host,
                             i::ProtectedPointerSlot slot) override {
    const i::PtrComprCageBase unused_cage_base(i::kNullAddress);
    VisitSlotImpl(unused_cage_base, slot);
  }

  void VisitJSDispatchTableEntry(i::Tagged<i::HeapObject> host,
                                 i::JSDispatchHandle handle) override {
    UNREACHABLE();
  }

  // copy from VisitRelocInfo
  uint32_t CountRelocInfo(i::Tagged<i::InstructionStream> host,
                          i::RelocIterator* it) {
    DCHECK(host->IsFullyInitialized());
    uint32_t count = 0;
    for (; !it->done(); it->next()) {
      count++;
      it->rinfo()->Visit(host, this);
    }
    return count;
  }

 private:
  // see heap-snapshot-generator.cc IndexedReferencesExtractor
  template <typename TSlot>
  void VisitSlotImpl(i::PtrComprCageBase cage_base, TSlot slot) {
    // NIY : if we want to get field_index, we must pass the
    // fake_object_addr
    i::Tagged<i::HeapObject> heap_object;
    // load the pointer
    auto loaded_value = slot.load(cage_base);
    if (loaded_value.GetHeapObjectIfStrong(&heap_object)) {
      i::Address object_address = heap_object->address();
      dumper_->getWriter()->WriteBinBlock(
          reinterpret_cast<uint8_t*>(&object_address), sizeof(object_address));
      // std::cout << "slot addr(strong):" << heap_object.address() << " ";
    } else if (loaded_value.GetHeapObjectIfWeak(&heap_object)) {
      i::Address object_address =
          heap_object->address() | dfx::kRawHeapWeakObjectTag;
      dumper_->getWriter()->WriteBinBlock(
          reinterpret_cast<uint8_t*>(&object_address), sizeof(object_address));
    }
    // here we do nothing for off-heap object and smi
  }

  i::PtrComprCageBase cage_base_;
  i::PtrComprCageBase code_cage_base_;
  dfx::ObjectDumper* dumper_;
};

}  // namespace dfx
#endif