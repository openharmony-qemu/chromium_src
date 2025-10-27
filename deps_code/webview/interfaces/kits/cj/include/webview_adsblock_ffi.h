/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WEBVIEW_ADSBLOCK_FFI_H
#define WEBVIEW_ADSBLOCK_FFI_H

#include "ffi_remote_data.h"
#include "webview_utils.h"
#include "cj_common_ffi.h"

extern "C" {
    // AdsBlockManager
    FFI_EXPORT int32_t FfiAdsBlockManagerSetAdsBlockRules(const char *rulesFile, bool replace);
    FFI_EXPORT int32_t FfiAdsBlockManagerAddAdsBlockDisallowedList(CArrString domainSuffixes);
    FFI_EXPORT int32_t FfiAdsBlockManagerRemoveAdsBlockDisallowedList(CArrString domainSuffixes);
    FFI_EXPORT int32_t FfiAdsBlockManagerAddAdsBlockAllowedList(CArrString domainSuffixes);
    FFI_EXPORT int32_t FfiAdsBlockManagerRemoveAdsBlockAllowedList(CArrString domainSuffixes);
    FFI_EXPORT void FfiAdsBlockManagerClearAdsBlockAllowedList();
    FFI_EXPORT void FfiAdsBlockManagerClearAdsBlockDisallowedList();
}

#endif // WEBVIEW_ADSBLOCK_FFI_H