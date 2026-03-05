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
#include "heap_dump.h"

#include "arkweb/chromium_ext/v8/heap_dump/binary_writer_base.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_heap.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_objects.h"
#include "arkweb/chromium_ext/v8/heap_dump/translator/translate_heap.h"
#include "arkweb/chromium_ext/v8/heap_dump/translator/translate_objects.h"
#include "arkweb/chromium_ext/v8/heap_dump/ut/binary_reader_ut.h"
#include "arkweb/chromium_ext/v8/heap_dump/ut/binary_writer_ut.h"
#include "arkweb/chromium_ext/v8/v8_ohlog.h"

namespace dfx {
static void DumpRawHeap(v8::internal::Heap* heap, std::string file) {
  dfx::BinaryWriterBase* writer = nullptr;
#ifdef USING_OHOS_WEB
  writer = v8::internal::V8::GetCurrentPlatform()->GetBinaryWriter().get();
#endif
  if (writer == nullptr) {
    return;
  }
  writer->OpenFile(file);
  dfx::HeapDumper dumper(heap, writer);
  dumper.DumpHeap();
  writer->CloseFile();
}

static const std::string path = "data/storage/el2/base/";

void DumpHeapAfterGC(v8::internal::Heap* heap) {
  int64_t time =
      v8::internal::V8::GetCurrentPlatform()->CurrentClockTimeMilliseconds();
  std::string file = path + "js-" + std::to_string(time) + ".rawheap";
  DumpRawHeap(heap, file);
}

void DumpHeapAfterOOM(v8::internal::Heap* heap) {
  int64_t time =
      v8::internal::V8::GetCurrentPlatform()->CurrentClockTimeMilliseconds();
  std::string file = path + "oom-" + std::to_string(time) + ".rawheap";
  DumpRawHeap(heap, file);
}
}  // namespace dfx

#endif
