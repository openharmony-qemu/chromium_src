// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CDM_WISEPLAY_CDM_COMMON_H_
#define MEDIA_CDM_WISEPLAY_CDM_COMMON_H_

#include "build/build_config.h"
#include "media/cdm/cdm_type.h"
#include "media/media_buildflags.h"

namespace media {

inline constexpr char kWiseplayKeySystem[] = "com.wiseplay.drm";
inline constexpr char kWiseplayCdmDisplayName[] =
    "Wiseplay Content Decryption Module";
inline constexpr media::CdmType kWiseplayCdmType{0x07e9461ea1caae58ull,
                                                 0xce85e30dd258547aull};

inline const uint8_t kWiseplayUuid[16] = {0x3D, 0x5E, 0x6D, 0x35, 0x9B, 0x9A,
                                          0x41, 0xE8, 0xB8, 0x43, 0xDD, 0x3C,
                                          0x6E, 0x72, 0xC4, 0x2C};
}  // namespace media

#endif  // MEDIA_CDM_WISEPLAY_CDM_COMMON_H_
