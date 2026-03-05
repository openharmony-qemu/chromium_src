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

#include "translate_roots.h"

#include "snapshot_generator.h"
#include "src/base/logging.h"

// see more in RootsReferencesExtrator in heap-snapshot-generator.cc

namespace dfx {
RootTranslator::RootTranslator(uint32_t table_offset,
                               uint32_t type_count,
                               uint32_t dump_size,
                               BinaryReaderBase* reader,
                               SnapshotGenerator* generator)
    : table_offset_(table_offset),
      type_count_(type_count),
      dump_size_(dump_size),
      reader_(reader),
      generator_(generator) {
  CHECK(reader_->CurrentPosition() == table_offset);
  CHECK(dump_size < std::numeric_limits<uint32_t>::max() - table_offset &&
        dump_size + table_offset <= reader->BinarySize());
}

void RootTranslator::Translate() {
  std::cout << "-----start root translate-----" << std::endl;
  generator_->AddSyntheticRootNodes();
  uint32_t type{0};
  uint32_t count{0};
  for (uint32_t i = 0; i < type_count_; ++i) {
    CHECK(reader_->ReadData(sizeof(type), reinterpret_cast<uint8_t*>(&type),
                            sizeof(type)));
    CHECK(reader_->ReadData(sizeof(count), reinterpret_cast<uint8_t*>(&count),
                            sizeof(count)));
    TranslateRoot(type, count);
  }
  CHECK(dump_size_ + table_offset_ == reader_->CurrentPosition());
  std::cout << "-----end translate-----" << std::endl;
}

void RootTranslator::TranslateRoot(uint32_t type, uint32_t count) {
  v8::internal::Root current_root = static_cast<v8::internal::Root>(type);
  if (i::v8_flags.log_heapdump) {
    std::cout << "-----" << v8::internal::RootVisitor::RootName(current_root)
              << "-----" << std::endl;
  }
  for (uint32_t i = 0; i < count; ++i) {
    CHECK(reader_->CurrentPosition() <=
          reader_->BinarySize() - sizeof(i::Address));
    i::Address root_addr;
    CHECK(reader_->ReadData(sizeof(root_addr),
                            reinterpret_cast<uint8_t*>(&root_addr),
                            sizeof(root_addr)));
#ifdef OH_ENABLE_HEAP_DUMP_TEST
    if (i::v8_flags.log_heapdump) {
      roots_info_[current_root].push_back(root_addr);
      std::cout << "object_address:" << root_addr << std::endl;
    }
#endif
    // NIY:description
    bool is_weak = false;
    if (root_addr & kRawHeapWeakObjectTag) {
      root_addr = root_addr & ~kRawHeapWeakObjectTag;
      is_weak = true;
    }
    generator_->SetGcSubrootReference(current_root, "" /*description*/, is_weak,
                                      root_addr);
  }
}
}  // namespace dfx
#endif
