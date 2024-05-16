// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_CUSTOM_MEDIA_URL_PARAMS_H_
#define MEDIA_BASE_CUSTOM_MEDIA_URL_PARAMS_H_

#include "media/base/media_export.h"
#include "net/cookies/site_for_cookies.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace media {

struct MEDIA_EXPORT CustomMediaUrlParams {
  CustomMediaUrlParams();
  CustomMediaUrlParams(uint32_t preload_type,
                       uint32_t media_source_type);
  CustomMediaUrlParams(const CustomMediaUrlParams& other);
  ~CustomMediaUrlParams();

  uint32_t preload_type;
  uint32_t media_source_type;
};

}  // namespace media

#endif  // MEDIA_BASE_CUSTOM_MEDIA_URL_PARAMS_H_
