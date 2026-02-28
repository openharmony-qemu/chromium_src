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
#include "translate_string_tables.h"

#include <sstream>

#include "arkweb/chromium_ext/v8/v8_ohlog.h"

namespace dfx {
StringTableTranslator::StringTableTranslator(BinaryReaderBase* reader,
                                             uint32_t offset,
                                             uint32_t size,
                                             uint32_t count)
    : reader_(reader),
      table_offset_(offset),
      table_size_(size),
      object_count_(count) {
  CHECK(reader->CurrentPosition() == offset);
  CHECK(offset + size > offset && size + offset <= reader->BinarySize());
}

void StringTableTranslator::Translate() {
  StringTableElementInfo info;
  for (uint32_t i = 0; i < GetObjectCount(); ++i) {
    bool ret = reader_->ReadData(sizeof(info.string_address_),
        reinterpret_cast<uint8_t*>(&info.string_address_), sizeof(info.string_address_));
    if (!ret) {
      LogInfo("fail to read addr");
      CHECK(ret);
    }
    ret = reader_->ReadData(sizeof(info.len), reinterpret_cast<uint8_t*>(&info.len), sizeof(info.len));
    if (!ret) {
      LogInfo("fail to read length");
      CHECK(ret);
    }
    stringTableList_.push_back(info);
  }
}
}  // namespace dfx

#endif
