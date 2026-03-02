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
#include "arkweb/chromium_ext/v8/heap_dump/translator/binary_reader.h"
#include "arkweb/chromium_ext/v8/heap_dump/dump_format.h"

#include <fcntl.h>
#include <unistd.h>

#include <cstdint>
#include <vector>

#include "src/base/logging.h"
#include "arkweb/chromium_ext/v8/v8_ohlog.h"

namespace dfx {
BinaryReader::BinaryReader(std::string path)
    : path_(path), file_(path, std::ios::binary | std::ios::in) {
  if (file_.is_open()) {
    file_.seekg(0, std::ios::end);
    CHECK(file_.good());
    std::streampos tmp_size = file_.tellg();
    CHECK(tmp_size >= 0);
    CHECK(tmp_size <= std::numeric_limits<uint32_t>::max());
    file_size_ = static_cast<uint32_t>(tmp_size);
    file_.seekg(0, std::ios::beg);
    CHECK(file_.good());
  } else {
    std::string errorMsg = "Failed to open file!";
    LogInfo(errorMsg);
    CHECK(false);
  }
}

bool BinaryReader::IsValid() const {
  return file_.is_open();
}

uint32_t BinaryReader::BinarySize() const {
  return file_size_;
}

bool BinaryReader::ReadHeader(dfx::RawHeapHeader& header) {
  uint32_t size = sizeof(dfx::RawHeapHeader);
  if (!ReadData(size, reinterpret_cast<uint8_t*>(&header), size)) {
    LogInfo("Fail to read header info!");
    return false;
  }
  return true;
}

bool BinaryReader::ReadData(uint32_t read_size, uint8_t* out, uint32_t out_size) {
  if (!IsValid() || !file_.good() || out_size < read_size) {
    return false;
  }
  std::streampos offset = file_.tellg();
  if (offset < 0 || (static_cast<uint32_t>(offset) <= file_size_ &&
    read_size > file_size_ - static_cast<uint32_t>(offset))) {
    return false;
  }

  file_.read(reinterpret_cast<char*>(out), read_size);
  if (file_.gcount() != static_cast<std::streamsize>(read_size)) {
    return false;
  }
  return true;
}

bool BinaryReader::ReadDataAt(uint32_t offset,
                              uint32_t read_size,
                              uint8_t* out,
                              uint32_t out_size) {
  if (!IsValid() || !file_.good()) {
    return false;
  }
  // record
  std::streampos cur_pos = file_.tellg();
  if (cur_pos < 0 || offset > file_size_ || read_size > file_size_ - offset) {
    return false;
  }
  if (out_size < read_size) {
    return false;
  }

  file_.seekg(offset, std::ios::beg);
  CHECK(file_.good());
  file_.read(reinterpret_cast<char*>(out), read_size);
  if (file_.gcount() != static_cast<std::streamsize>(read_size)) {
    return false;
  }
  // restore
  file_.seekg(cur_pos, std::ios::beg);
  CHECK(file_.good());
  return true;
}

uint32_t BinaryReader::CurrentPosition() {
  std::streampos tmp_size = file_.tellg();
  CHECK(tmp_size >= 0);
  CHECK(tmp_size <= std::numeric_limits<uint32_t>::max());
  return static_cast<uint32_t>(tmp_size);
}

void BinaryReader::SetPosition(uint32_t pos) {
  file_.seekg(pos, std::ios::beg);
  CHECK(file_.good());
}

BinaryReader::~BinaryReader() {
  if (IsValid()) {
    file_.close();
  }
}
}  // namespace dfx
#endif
