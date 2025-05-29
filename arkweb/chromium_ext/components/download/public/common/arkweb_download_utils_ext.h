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

#ifndef COMPONENTS_ARKWEB_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_UTILS_EXT_H_
#define COMPONENTS_ARKWEB_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_UTILS_EXT_H_

#include <stddef.h>

#include <memory>
#include <optional>

#include "components/download/database/download_db_entry.h"
#include "components/download/database/in_progress/download_entry.h"
#include "components/download/public/common/download_export.h"
#include "components/download/public/common/download_interrupt_reasons.h"
#include "components/download/public/common/download_item.h"
#include "components/download/public/common/download_item_impl.h"
#include "components/download/public/common/download_source.h"
#include "components/download/public/common/resume_mode.h"
#include "net/base/net_errors.h"
#include "net/cert/cert_status_flags.h"
#include "net/http/http_response_headers.h"
#include "services/network/public/mojom/url_response_head.mojom.h"

namespace download {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
enum FileRenameOptions {
  RENAME_AND_UNIQUIFY_MODE = 1,
  OVERWRITE_MODE = 2,
};
#endif  // BUILDFLAG(ARKWEB_EX_DOWNLOAD)

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
COMPONENTS_DOWNLOAD_EXPORT FileRenameOptions GetFileRenameOptions();

COMPONENTS_DOWNLOAD_EXPORT void SetFileRenameOptions(FileRenameOptions options);
#endif  // BUILDFLAG(ARKWEB_EX_DOWNLOAD)
}  // namespace download

#endif  // COMPONENTS_ARKWEB_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_UTILS_EXT_H_
