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

#ifndef DUMP_FORMAT_INL_H
#define DUMP_FORMAT_INL_H
#if defined(OH_ENABLE_HEAP_DUMP) || defined(ON_ENABLE_HEAP_TRANSLATE)
#include "dump_format.h"

namespace dfx {
static void PrintHead(std::ostream& os, const RawHeapHeader& head) {
  os << "cage_base_: " << head.cage_base_
     << ", code_cage_base_: " << head.code_cage_base_
     << ", root_table_offset_: " << head.root_table_offset_
     << ", root_table_size_: " << head.root_table_size_
     << ", root_type_count_: " << head.root_type_count_
     << ", object_table_offset_: " << head.object_table_offset_
     << ", object_table_size_: " << head.object_table_size_
     << ", object_count_: " << head.object_count_;
}
}  // namespace dfx

#endif
#endif
