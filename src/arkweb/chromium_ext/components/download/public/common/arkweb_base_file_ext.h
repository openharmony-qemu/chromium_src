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

#ifndef COMPONENTS_ARKWEB_DOWNLOAD_PUBLIC_COMMON_BASE_FILE_EXT_H_
#define COMPONENTS_ARKWEB_DOWNLOAD_PUBLIC_COMMON_BASE_FILE_EXT_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <string>

#endif
#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#include "base/check.h"
#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/functional/callback.h"
#include "base/functional/callback_forward.h"
#include "base/gtest_prod_util.h"
#include "base/memory/weak_ptr.h"
#include "base/sequence_checker.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "components/download/public/common/download_export.h"
#include "components/download/public/common/download_interrupt_reasons.h"
#include "components/services/quarantine/public/mojom/quarantine.mojom.h"
#include "crypto/secure_hash.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "net/base/net_errors.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace download {

// File being downloaded and saved to disk. This is a base class
// for DownloadFile and SaveFile, which keep more state information. BaseFile
// considers itself the owner of the physical file and will delete it when the
// BaseFile object is destroyed unless the ownership is revoked via a call to
// Detach().
class COMPONENTS_DOWNLOAD_EXPORT ArkWebBaseFileExt : public BaseFile {
 public:
  // May be constructed on any thread.  All other routines (including
  // destruction) must occur on the same sequence.
  explicit ArkWebBaseFileExt(uint32_t download_id);

  ArkWebBaseFileExt(const ArkWebBaseFileExt&) = delete;
  ArkWebBaseFileExt& operator=(const ArkWebBaseFileExt&) = delete;

  ~ArkWebBaseFileExt() = default;

  ArkWebBaseFileExt* AsArkWebBaseFileExt() override { return this; }


  bool ReadDataFromFile(int64_t offset,
                        char* data,
                        size_t data_len,
                        const base::FilePath& file_path);
  base::FilePath GetSaveDirectory(const base::FilePath& default_directory);
#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
#endif
};

}  // namespace download
