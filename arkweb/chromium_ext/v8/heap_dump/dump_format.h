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

#ifndef DUMP_FORMAT_H
#define DUMP_FORMAT_H
#if defined(OH_ENABLE_HEAP_DUMP) || defined(ON_ENABLE_HEAP_TRANSLATE)

#include "include/v8-internal.h"
#include "src/heap/heap.h"

namespace dfx {
static_assert(sizeof(uintptr_t) == sizeof(v8::internal::Address));
constexpr v8::internal::Address kRawHeapWeakObjectTag = 0x1;

constexpr uint64_t kRawHeapMagic = 0xabc132;
constexpr uint32_t kRawHeapVersion = 0x1;

struct RawHeapHeader {
  uint64_t magic_;
  uint32_t version_;
  uint32_t flags_;
  uint64_t timestamp_;

  v8::internal::Address cage_base_;
  v8::internal::Address code_cage_base_;

  uint32_t root_table_offset_;
  uint32_t root_table_size_;
  uint32_t root_type_count_;

  uint32_t string_table_offset_;
  uint32_t string_table_size_;
  uint32_t string_count_;

  uint32_t object_table_offset_;
  uint32_t object_table_size_;
  uint32_t object_count_;

  uint32_t metadata_offset_;
  uint32_t metadata_size_;
  // ...
};

struct InstructionStreamInfo {
  v8::internal::Address object_addr_;
  int8_t mode_;
};

struct StringTableElementInfo {
  v8::internal::Address string_address_;
  int len_;
};

}  // namespace dfx
#endif
#endif