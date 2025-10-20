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

#ifndef COMPONENTS_OS_CRYPT_SYNC_OS_CRYPT_FOR_INCLUDE_H_
#define COMPONENTS_OS_CRYPT_SYNC_OS_CRYPT_FOR_INCLUDE_H_

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
#include "base/feature_list.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/ohos/nweb_engine_event_logger.h"
#include "base/ohos/nweb_engine_event_logger_code.h"
#include "base/path_service.h"
#include "base/strings/string_number_conversions.h"
#include "cef/libcef/browser/prefs/browser_prefs.h"
#include "chrome/browser/browser_process.h"
#include "components/prefs/pref_service.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#endif

#if BUILDFLAG(ARKWEB_ENCRYPT)
#include "arkweb/chromium_ext/components/os_crypt/sync/ohos_crypto.h"
#endif

namespace {

#if BUILDFLAG(ARKWEB_ENCRYPT)
// Size of initialization vectore for GCM
const size_t kIVSizeAESGCM = 12;

constexpr char kDataKeyAlias[] = "nweb_data_key";
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
enum WebStorageMigrationErrorCode {
  MIGRATE_SUCCESS = 0,
  DIRECTORY_OR_FILE_NOT_EXIST,
  ASSET_QUERY_FAILED,
  LOGIN_DATA_OPEN_FAILED,
  PASSWORD_VAULT_CONNECT_FAILED,
  PASSWORD_IMPORT_FAILED
};
#endif
}  // namespace

#if BUILDFLAG(ARKWEB_TEST)
#include "crypto/symmetric_key.h"
bool DecryptWithIvForInclude(const std::string& raw_ciphertext,
                            crypto::SymmetricKey* encryption_key,
                            crypto::SymmetricKey* encryption_key_ota,
                            std::string* plaintext,
                            std::string& iv,
                            bool& result);
#endif

#endif  // COMPONENTS_OS_CRYPT_SYNC_OS_CRYPT_FOR_INCLUDE_H_
