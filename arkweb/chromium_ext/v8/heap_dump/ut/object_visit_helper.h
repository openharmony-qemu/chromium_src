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

#ifndef OBJECT_VISIT_HELPER_H
#define OBJECT_VISIT_HELPER_H
#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(OH_ENABLE_HEAP_DUMP_TEST))

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
#include "src/objects/visitors.h"

namespace dfx {
//////////////////// ObjectDetailVisitor
// see IndexedReferencesExtractor in heap-snapshot-generator.cc
class ObjectDetailVisitor : public i::ObjectVisitorWithCageBases {
 public:
  explicit ObjectDetailVisitor(i::PtrComprCageBase cage_base,
                               i::PtrComprCageBase code_cage_base)
      : i::ObjectVisitorWithCageBases(cage_base, code_cage_base),
        cage_base_(cage_base),
        code_cage_base_(code_cage_base) {}

  void VisitMapPointer(i::Tagged<i::HeapObject> object) override {}

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
  void VisitCodeTarget(i::Tagged<i::InstructionStream> host,
                       i::RelocInfo* rinfo) override {}
  // same as VisitCodeTarget
  void VisitEmbeddedPointer(i::Tagged<i::InstructionStream> host,
                            i::RelocInfo* rinfo) override {}

  // IterateTrustedPointer will call this. IndirectPointerSlot is a index to an
  // entry in a pointer table.
  void VisitIndirectPointer(i::Tagged<i::HeapObject> host,
                            i::IndirectPointerSlot slot,
                            i::IndirectPointerMode mode) override {
    // handle is a uint32_t type index
    i::IndirectPointerHandle handle = slot.Relaxed_LoadHandle();
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
      std::cout << "slot addr(strong):" << heap_object.address() << " ";
    } else if (loaded_value.GetHeapObjectIfWeak(&heap_object)) {
      std::cout << "slot addr(weak):" << heap_object.address() << " ";
    }
    // here we do nothing for off-heap object and smi
  }

  i::PtrComprCageBase cage_base_;
  i::PtrComprCageBase code_cage_base_;
};

}  // namespace dfx
#endif
#endif
