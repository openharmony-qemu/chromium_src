/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/base/files/memory_mapped_file_ext.h"

#include <utility>

#include "arkweb/build/features/features.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/notreached.h"
#include "base/numerics/safe_math.h"
#include "base/system/sys_info.h"
#include "build/build_config.h"

namespace base {

MemoryMappedFileExt::~MemoryMappedFileExt()
{
  if ((data_ != nullptr) && (mapper_ == nullptr)) {
    delete[] data_;
    data_ = nullptr;
  }
  length_ = 0;
  mapper_.reset();
}

void MemoryMappedFileExt::ClearData(std::function<void()> cb)
{
  if (!customizeData_) {
    cb();
    return;
  }

  if ((data_ != nullptr) && (mapper_ == nullptr)) {
    delete[] data_;
    data_ = nullptr;
  }
  length_ = 0;
  mapper_.reset();
}

void MemoryMappedFileExt::SetOhosFileMapper(
    std::shared_ptr<OHOS::NWeb::OhosFileMapper>& mapper, std::function<void()> cb, span<uint8_t>& bytes)
{
  if (!bytes.empty()) {
    if (customizeData_) {
      if (mapper_ == nullptr) {
        delete[] data_;
        data_ = nullptr;
      } else {
        mapper_ = nullptr;
      }
    } else {
      cb();
    }
  }
  if (mapper == nullptr) {
    return;
  }
  customizeData_ = true;

  if (!mapper->IsCompressed()) {
    mapper_ = std::move(mapper);
    uint8_t* tempData = reinterpret_cast<uint8_t*>(mapper_->GetDataPtr());
    size_t tempLength = mapper_->GetDataLen();
    bytes = span<uint8_t>(tempData, tempLength);
  } else {
    uint8_t* tmp = nullptr;
    size_t tmpLength = bytes.size();
    mapper->UnzipData(&tmp, tmpLength);
    bytes = span<uint8_t>(tmp, tmpLength);
  }
}

}  // namespace base
