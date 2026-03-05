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

#ifndef TRANSLATE_OBJECTS_H
#define TRANSLATE_OBJECTS_H
#if (defined(ON_ENABLE_HEAP_TRANSLATE) || defined(OH_ENABLE_HEAP_DUMP_TEST))
#include <set>
#include <unordered_map>

#include "arkweb/chromium_ext/v8/heap_dump/binary_reader_base.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_format.h"
#include "arkweb/chromium_ext/v8/heap_dump/translator/snapshot_generator.h"
#include "include/v8-internal.h"
#include "src/common/ptr-compr.h"

namespace dfx {

class SnapshotGenerator;

class ObjectTranslator {
 public:
  explicit ObjectTranslator(BinaryReaderBase* reader,
                            uint32_t offset,
                            uint32_t size,
                            uint32_t count,
                            v8::internal::PtrComprCageBase cage_base,
                            v8::internal::PtrComprCageBase code_cage_base,
                            SnapshotGenerator* generator);
  ~ObjectTranslator() = default;
  void Translate();

  void RecordAsMap(v8::internal::Address addr);
  SnapshotGenerator* generator() { return generator_; }

 private:
  struct RawHeapObjectHead {
    uint32_t offset_;     // offset in raw heap
    uint32_t size_ : 31;  // size in raw heap
#if defined(OH_ENABLE_HEAP_DUMP_TEST)
    uint32_t is_map_ : 1;  // indicate this heap object a map or not
#endif
  };
  void PreVisit();
  void TranslateHeapObject(v8::internal::Address addr, uint32_t size);
  void TranslateInstructionStream(v8::internal::Address addr, uint32_t size);
  bool IsStrong(v8::internal::Address addr);
  Node* AddNode(i::Tagged<i::Map> map,
                i::Tagged<i::HeapObject> object,
                i::Address address,
                uint32_t size);
  std::string GetClassName(i::Tagged<i::Map> map,
                           i::Tagged<i::HeapObject> object,
                           i::Address address);
  bool IsShared(v8::internal::InstanceType instance_type);
  bool IsBoolean(v8::internal::InstanceType instance_type,
                 v8::internal::Tagged<v8::internal::Object> obj);

  BinaryReaderBase* reader_;
  const uint32_t table_offset_;
  const uint32_t table_size_;
  const uint32_t object_count_;
  v8::internal::PtrComprCageBase cage_base_;
  v8::internal::PtrComprCageBase code_cage_base_;

  // record related
  // record all address
  std::unordered_map<v8::internal::Address, RawHeapObjectHead> objects_head_;
  SnapshotGenerator* generator_;
};
}  // namespace dfx
#endif
#endif
