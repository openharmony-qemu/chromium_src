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

#ifndef BASE_I18N_ICU_UTIL_OHOS_H_
#define BASE_I18N_ICU_UTIL_OHOS_H_

#include "arkweb/build/features/features.h"
#include "base/i18n/icu_util.h"
#if BUILDFLAG(IS_ARKWEB)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

#if BUILDFLAG(IS_ARKWEB) && (BUILDFLAG(ARKWEB_HAP_DECOMPRESSED) || BUILDFLAG(ARKWEB_MEM))
const char kIcuDataFileNameHap[] = "resources/rawfile/icudtl.dat";
const int ERROR_CODE_THREE = 3;

void InitializeExternalTimeZoneData()
{
#if BUILDFLAG(IS_FUCHSIA)
  // Set the environment variable to override the location used by ICU.
  // Loading can still fail if the directory is empty or its data is invalid.
  std::unique_ptr<base::Environment> env = base::Environment::Create();
  if (!base::DirectoryExists(base::FilePath(g_icu_time_zone_data_dir))) {
    PLOG(FATAL) << "Could not open directory: '" << g_icu_time_zone_data_dir
                << "'";
  }
  env->SetVar(kIcuTimeZoneEnvVariable, g_icu_time_zone_data_dir);
#endif  // BUILDFLAG(IS_FUCHSIA)
}

int LoadIcuDataByHap(base::PlatformFile data_fd,
                     const base::MemoryMappedFile::Region& data_region,
                     std::unique_ptr<base::MemoryMappedFile>* out_mapped_data_file,
                     UErrorCode* out_error_code)
{
  auto resourceInstance =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().GetResourceAdapter();
  std::shared_ptr<OHOS::NWeb::OhosFileMapper> fileMapper =
        resourceInstance->GetRawFileMapper(kIcuDataFileNameHap, true);
  if (!fileMapper) {
    LOG(ERROR) << "Couldn't mmap icu data file by hap: " << kIcuDataFileNameHap;
    return 1;
  }
  LOG(INFO) << "icu data file length: " << fileMapper->GetDataLen();

  *out_mapped_data_file = std::make_unique<base::MemoryMappedFile>();
  (*out_error_code) = U_ZERO_ERROR;
  InitializeExternalTimeZoneData();
  (*out_mapped_data_file)->SetOhosFileMapper(fileMapper);

  udata_setCommonData(const_cast<uint8_t*>((*out_mapped_data_file)->data()),
                      out_error_code);
  if (U_FAILURE(*out_error_code)) {
    LOG(ERROR) << "Failed to initialize ICU with data file: "
               << u_errorName(*out_error_code);
    return ERROR_CODE_THREE;
  }
  return 0;
}
#endif

#if BUILDFLAG(IS_ARKWEB) && (BUILDFLAG(ARKWEB_HAP_DECOMPRESSED) || BUILDFLAG(ARKWEB_MEM))
#define LOAD_ICU_DATA(data_fd, data_region, mapped_file, err) \
    if (data_fd == kInvalidPlatformFile) { \
        g_debug_icu_load = \
            LoadIcuDataByHap(data_fd, data_region, &mapped_file, &err); \
    } else { \
        g_debug_icu_load = LoadIcuData(data_fd, data_region, &mapped_file, &err); \
    }
#endif

#define CREATE_TIME_ZONE() \
  { \
    LOG(DEBUG) << "InitializeIcuTimeZone in OHOS ARKWEB."; \
    auto tzid = OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDateTimeFormatAdapter()->GetTimezone(); \
    icu::TimeZone::createDefault(); \
    std::unique_ptr<icu::TimeZone> timezone(icu::TimeZone::createTimeZone( \
        icu::UnicodeString::fromUTF8(tzid))); \
    icu::TimeZone::adoptDefault(timezone.release()); \
  }
#endif  // BASE_I18N_ICU_UTIL_OHOS_H_
