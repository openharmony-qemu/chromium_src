#ifndef DUMP_FORMAT_H
#define DUMP_FORMAT_H

#include "include/v8-internal.h"
#include "src/heap/heap.h"

namespace dfx {

constexpr v8::internal::Address kRawHeapWeakObjectTag = 0x1;

constexpr uint64_t kRawHeapMagic = 0xabc132;
constexpr uint32_t kRawHeapVersion_V1 = 0x1;

struct RawHeapHeader {
  uint64_t magic_;
  uint32_t version_;
  uint32_t flags_;
  uint64_t timestamp_;

  v8::internal::Address cage_base_;
  v8::internal::Address code_cage_base_;

  uint32_t root_table_offset_;
  uint32_t root_table_size_;
  uint32_t root_count_;

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

struct StringTableElementInfo {
  v8::internal::Address string_address;
  int len;
};

}  // namespace dfx
#endif
