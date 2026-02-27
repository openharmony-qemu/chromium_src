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

#ifndef DUMP_ROOTS_H
#define DUMP_ROOTS_H

#if defined(OH_ENABLE_HEAP_DUMP) && \
    (defined(USING_OHOS) || defined(USING_OHOS_WEB))
#include "binary_writer_base.h"
#include "dump_format.h"

#include <set>

#include "src/heap/combined-heap.h"
#include "src/heap/heap-visitor-inl.h"
#include "src/heap/heap.h"
#include "src/objects/visitors.h"

#include "v8_ohlog.h"

namespace dfx {

class RootDumper {
 public:
  explicit RootDumper(v8::internal::Heap* heap, BinaryWriterBase* writer);
  ~RootDumper() = default;
  inline void AddRoot(v8::internal::Root root,
                      dfx::Address addr,
                      bool is_weak);

  void PreVisit();

  void Dump();

  uint32_t RootKindCount() const;  // root_kind_count_
  uint32_t RootDumpSize() const;   // root_dump_size_

 private:
  uint32_t root_kind_count_{0};
  uint32_t root_dump_size_{0};

  v8::internal::Heap* heap_;
  BinaryWriterBase* writer_{nullptr};
  std::unordered_map<v8::internal::Root, std::set<dfx::Address>>
      roots_info_;
};
}  // namespace dfx
#endif

#endif
