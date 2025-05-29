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

#ifndef COMPONENTS_ARKWEB_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_EXT_H_
#define COMPONENTS_ARKWEB_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_EXT_H_

#include <stdint.h>

#include <memory>
#include <string>

#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "base/functional/callback_forward.h"
#include "build/build_config.h"

class GURL;

namespace download {

// These objects live exclusively on the download sequence and handle the
// writing operations for one download. These objects live only for the duration
// that the download is 'in progress': once the download has been completed or
// cancelled, the DownloadFile is destroyed.
class COMPONENTS_DOWNLOAD_EXPORT ArkWebDownloadFileExt {
 public:
  virtual ~ArkWebDownloadFileExt() = default;

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  virtual void RunCallbackIfDataReady() = 0;

  virtual void RegisterReadDownloadCallback(base::OnceCallback<void()> callback,
                                            uint32_t size) = 0;

  virtual bool ReadDownloadDataFromFile(int64_t offset,
                                        char* data,
                                        size_t size) = 0;

  virtual uint32_t GetNoHoleDownloadDataSize() = 0;
#endif
};

}  // namespace download

#endif  // COMPONENTS_ARKWEB_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_EXT_H_
