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

#ifndef BASE_FILES_MEMORY_MAPPED_FILE_EXT_H_
#define BASE_FILES_MEMORY_MAPPED_FILE_EXT_H_

#include <stddef.h>
#include <stdint.h>

#include <utility>
#include <functional>
#include <span>

#include "arkweb/build/features/features.h"
#include "base/base_export.h"
#if BUILDFLAG(ARKWEB_UNITTESTS)
#undef private
#endif  // ARKWEB_UNITTESTS
#include "base/containers/span.h"
#include "base/files/file.h"
#include "base/memory/raw_ptr_exclusion.h"
#include "build/build_config.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#if BUILDFLAG(ARKWEB_UNITTESTS)
#define private public
#endif  // ARKWEB_UNITTESTS
namespace base {
class MemoryMappedFileExt {
public:
  MemoryMappedFileExt() = default;
  MemoryMappedFileExt(const MemoryMappedFileExt&) = delete;
  MemoryMappedFileExt& operator=(const MemoryMappedFileExt&) = delete;
  MemoryMappedFileExt(const MemoryMappedFileExt&&) = delete;
  MemoryMappedFileExt& operator=(const MemoryMappedFileExt&&) = delete;
  ~MemoryMappedFileExt();
  void ClearData(std::function<void()> cb);
  void SetOhosFileMapper(
    std::shared_ptr<OHOS::NWeb::OhosFileMapper>& mapper, std::function<void()> cb, span<uint8_t>& bytes);

private:
  bool customizeData_ = false;
  std::shared_ptr<OHOS::NWeb::OhosFileMapper> mapper_;
  size_t length_ = 0;
};

#if BUILDFLAG(ARKWEB_UNITTESTS)
#undef private
#endif  // ARKWEB_UNITTESTS

}  // namespace base

#endif  // BASE_FILES_MEMORY_MAPPED_FILE_EXT_H_
