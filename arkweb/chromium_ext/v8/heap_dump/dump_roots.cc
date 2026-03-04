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
#include "arkweb/chromium_ext/v8/heap_dump/dump_roots.h"

#include "src/common/ptr-compr.h"
#include "src/objects/objects-inl.h"
namespace dfx {
using namespace v8;

// NIY: see GlobalObjectsEnumerator in heap-snapshot-generator.cc for Global
// Objects

////////// RootVisitor
// reference to RootsReferencesExtractor
class RootVisitorForDump final : public i::RootVisitor {
 public:
  explicit RootVisitorForDump(RootDumper* dumper) : dumper_(dumper) {}

  void SetVisitingWeakRoots() { visiting_weak_roots_ = true; }

  void VisitRootPointer(i::Root root,
                        const char* description,
                        i::FullObjectSlot p) override {
    i::Tagged<i::Object> object = *p;
#ifdef V8_ENABLE_DIRECT_HANDLE
    if (object.ptr() == i::kTaggedNullAddress) {
      return;
    }
#endif
    // FREE_SPACE_TYPE not existed when CombinedHeapObjectIterator
    if (i::IsSmi(object) || !i::IsHeapObject(object)) {
      return;
    }
    i::Tagged<i::HeapObject> ho = i::Cast<i::HeapObject>(object);
    if (ho->map()->instance_type() == i::InstanceType::FREE_SPACE_TYPE) {
      return;
    }
    i::Address object_address = ho.address();
    dumper_->AddRoot(root, object_address, visiting_weak_roots_);

#ifdef OH_ENABLE_HEAP_DUMP_TEST
    if (i::v8_flags.log_heapdump) {
      std::stringstream ss;
      ss << "[HeapDump] root name:" << i::RootVisitor::RootName(root)
         << " instance type:" << i::ToString(ho->map()->instance_type())
         << " object addr: " << object_address << " object size:" << ho->Size()
         << "\n";
      LogInfo(ss.str());
    }
#endif
  }

  void VisitRootPointers(i::Root root,
                         const char* description,
                         i::FullObjectSlot start,
                         i::FullObjectSlot end) override {
    for (i::FullObjectSlot slot = start; slot < end; ++slot) {
      VisitRootPointer(root, description, slot);
    }
  }

  void VisitRootPointers(i::Root root,
                         const char* description,
                         i::OffHeapObjectSlot start,
                         i::OffHeapObjectSlot end) override {
    DCHECK_EQ(root, i::Root::kStringTable);
    // do nothing
  }

  // Keep this synced with
  // MarkCompactCollector::RootMarkingVisitor::VisitRunningCode.
  void VisitRunningCode(i::FullObjectSlot code_slot,
                        i::FullObjectSlot istream_or_smi_zero_slot) final {
    i::Tagged<i::Object> istream_or_smi_zero = *istream_or_smi_zero_slot;
    if (istream_or_smi_zero != i::Smi::zero()) {
      i::Tagged<i::Code> code = i::Cast<i::Code>(*code_slot);
      code->IterateDeoptimizationLiterals(this);
      VisitRootPointer(i::Root::kStackRoots, nullptr, istream_or_smi_zero_slot);
    }
    VisitRootPointer(i::Root::kStackRoots, nullptr, code_slot);
  }

 private:
  RootDumper* dumper_;
  bool visiting_weak_roots_{false};
};

////////// RootDumper

RootDumper::RootDumper(i::Heap* heap, BinaryWriterBase* writer)
    : heap_(heap), writer_(writer) {}

void RootDumper::AddRoot(i::Root root, i::Address addr, bool is_weak) {
  if (is_weak) {
    addr |= kRawHeapWeakObjectTag;
  }
  if (!roots_info_[root].count(addr)) {
    root_dump_size_ += sizeof(addr);  // dump object address
    roots_info_[root].insert(addr);
  }
}

uint32_t RootDumper::RootKindCount() const {
  return root_kind_count_;
}

uint32_t RootDumper::RootDumpSize() const {
  return root_dump_size_;
}

void RootDumper::PreVisit() {
  RootVisitorForDump objectVisitor(this);
  i::ReadOnlyRoots(heap_).Iterate(&objectVisitor);
  heap_->IterateRoots(&objectVisitor,
                      v8::base::EnumSet<i::SkipRoot>{
                          i::SkipRoot::kWeak, i::SkipRoot::kTracedHandles});
  heap_->IterateWeakRoots(&objectVisitor, {});
  objectVisitor.SetVisitingWeakRoots();
  heap_->IterateWeakGlobalHandles(&objectVisitor);

  root_kind_count_ = static_cast<uint32_t>(roots_info_.size());
  // every kind of root needs dump "kind + count"
  root_dump_size_ += root_kind_count_ * (sizeof(uint32_t) + sizeof(uint32_t));
}

void RootDumper::Dump() {
  for (const auto& [root_type, addresses] : roots_info_) {
    uint32_t type = static_cast<uint32_t>(root_type);
    uint32_t count = static_cast<uint32_t>(addresses.size());
    writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&type), sizeof(type));
    writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&count), sizeof(count));
    if (i::v8_flags.log_heapdump) {
      std::cout << "-----" << i::RootVisitor::RootName(root_type) << "-----"
                << std::endl;
    }
    for (auto addr : addresses) {
      writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&addr), sizeof(addr));
    }
  }
}

}  // namespace dfx
#endif
