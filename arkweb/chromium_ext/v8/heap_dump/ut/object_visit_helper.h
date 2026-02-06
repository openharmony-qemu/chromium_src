#ifndef OBJECT_VISIT_HELPER_H
#define OBJECT_VISIT_HELPER_H

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
  // will call it. todo(hh): if use DCHECK, it will fail before this visit
  void VisitCodeTarget(i::Tagged<i::InstructionStream> host,
                       i::RelocInfo* rinfo) override {}
  // same as VisitCodeTarget
  void VisitEmbeddedPointer(i::Tagged<i::InstructionStream> host,
                            i::RelocInfo* rinfo) override {}

  // IterateTrustedPointer will call this. IndirectPointerSlot is a index to an
  // entry in a ponter table.
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
    // todo(hh) : if we want to get filed_index, we must pass the
    // fake_object_addr
    i::Tagged<i::HeapObject> heap_object;
    // load the ponter
    auto loaded_value = slot.load(cage_base);
    if (loaded_value.GetHeapObjectIfStrong(&heap_object)) {
      std::cout << "slot addr(strong):" << heap_object.address() << " ";
    } else if (loaded_value.GetHeapObjectIfWeak(&heap_object)) {
      // todo(hh): maybe weak reference
      std::cout << "slot addr(weak):" << heap_object.address() << " ";
    }
    // here we do nothing for off-heap object and smi
  }

  i::PtrComprCageBase cage_base_;
  i::PtrComprCageBase code_cage_base_;
};

}  // namespace dfx
#endif