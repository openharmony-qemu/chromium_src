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
#include "arkweb/chromium_ext/v8/heap_dump/dump_heap.h"

#include "arkweb/chromium_ext/v8/heap_dump/dump_format-inl.h"
#include "arkweb/chromium_ext/v8/v8_ohlog.h"
#include "src/common/ptr-compr.h"
#include "src/heap/heap-inl.h"
namespace dfx {
HeapDumper::HeapDumper(v8::internal::Heap* heap, BinaryWriterBase* writer)
    : heap_(heap),
      safepoint_scope_(std::make_unique<v8::internal::SafepointScope>(
          heap->isolate(),
          v8::internal::kGlobalSafepointForSharedSpaceIsolate)),
      writer_(writer),
      root_dumper_(heap, writer),
      string_table_dumper_(heap, writer),
      object_dumper_(heap, writer) {
  heap_->MakeHeapIterable();
}

void HeapDumper::DumpHeap() {
  v8::base::ElapsedTimer timer;
  timer.Start();
  // PreVisit to get header message
  PreVisit();
  DumpHeader();
  root_dumper_.Dump();
  object_dumper_.Dump();
  double elapsed = timer.Elapsed().InMillisecondsF();
  LogInfo("[HeapDump]elapsed time: " + std::to_string(elapsed) + " ms\n");
  timer.Stop();
}

void HeapDumper::DumpHeader() {
  memset(&header_, 0, sizeof(header_));
  header_.magic_ = kRawHeapMagic;
  header_.version_ = kRawHeapVersion;

  header_.cage_base_ = heap_->isolate()->cage_base();
  header_.code_cage_base_ = heap_->isolate()->code_cage_base();

  header_.root_table_offset_ = sizeof(RawHeapHeader);
  header_.root_table_size_ = root_dumper_.RootDumpSize();
  header_.root_type_count_ = root_dumper_.RootKindCount();

  header_.string_table_offset_ =
      header_.root_table_offset_ + header_.root_table_size_;
  header_.string_table_size_ = string_table_dumper_.StringTableDumpSize();
  header_.string_count_ = string_table_dumper_.StringCount();

  header_.object_table_offset_ =
      header_.string_table_offset_ + header_.string_table_size_;
  header_.object_table_size_ = object_dumper_.ObjectDumpSize();
  header_.object_count_ = object_dumper_.ObjectCount();

  header_.metadata_offset_ =
      header_.object_table_offset_ + header_.object_table_size_;

  std::stringstream ss;
  ss << "HeapDump dump header_:" << std::endl;
  PrintHead(ss, header_);
  LogInfo(ss.str());

  writer_->WriteBinBlock(reinterpret_cast<uint8_t*>(&header_), sizeof(header_));
}

void HeapDumper::PreVisit() {
  root_dumper_.PreVisit();
  object_dumper_.PreVisit();
}
}  // namespace dfx
#endif
