#if defined(OH_ENABLE_HEAP_DUMP)
#ifndef DUMP_FORMAT_INL_H
#define DUMP_FORMAT_INL_H
#include "dump_format.h"

namespace dfx {
static void PrintHead(RawHeapHeader& head, std::ostream& os) {
  os << " cage_base_ " << head.cage_base_ << " code_cage_base_ "
     << head.code_cage_base_ << " root_table_offset_ "
     << head.root_table_offset_ << " root_table_size_ " << head.root_table_size_
     << " root_count_ " << head.root_count_ << " object_table_offset_ "
     << head.object_table_offset_ << " object_table_size_ "
     << head.object_table_size_ << " object_count_ " << head.object_count_
     << " ";
}
}  // namespace dfx

#endif
#endif