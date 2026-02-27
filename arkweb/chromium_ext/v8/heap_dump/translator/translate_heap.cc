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
#include "translate_heap.h"
#include "translate_objects.h"
#include "translate_roots.h"
#include "translate_string_tables.h"
#include "heap_dump/binary_reader_base.h"
#include "heap_dump/dump_format-inl.h"

#include "src/common/ptr-compr-inl.h"
#include "src/common/ptr-compr.h"

#include "arkweb/ohos_nweb_ex/third_party/securec/include/securec.h"

namespace dfx {
HeapTranslator::HeapTranslator(BinaryReaderBase* reader,
                               SnapshotGenerator* generator)
    : reader_(reader), generator_(generator) {
  memset_s(&header_, sizeof(header_), 0, sizeof(header_));
}

void HeapTranslator::TranslateHeader() {
  uint32_t size = static_cast<uint32_t>(sizeof(header_));
  reader_->ReadData(size, reinterpret_cast<uint8_t*>(&header_), size);
  i::V8HeapCompressionScheme::InitBase(header_.cage_base_);
  i::ExternalCodeCompressionScheme::InitBase(header_.code_cage_base_);
  std::cout << "translate header_:" << std::endl;
  PrintHead(std::cout, header_);
}

void HeapTranslator::TranslateHeap() {
  TranslateHeader();
  TranslateRoots();
  TranslateStringTable();
  TranslateObjects();
}

void HeapTranslator::TranslateRoots() {
  root_translator_ = std::make_unique<RootTranslator>(header_.root_table_offset_,
      header_.root_type_count_, header_.root_table_size_, reader_, generator_);
  root_translator_->Translate();
}

void HeapTranslator::TranslateStringTable() {}

void HeapTranslator::TranslateObjects() {
  object_translator_ = std::make_unique<ObjectTranslator>(
      reader_, header_.object_table_offset_, header_.object_table_size_,
      header_.object_count_, v8::internal::PtrComprCageBase(header_.cage_base_),
      v8::internal::PtrComprCageBase(header_.code_cage_base_), generator_);
  object_translator_->Translate();
}
}  // namespace dfx

#endif
