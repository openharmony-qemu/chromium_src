// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/custom_media_url_params.h"

namespace media {

CustomMediaUrlParams::CustomMediaUrlParams()
    : preload_type(0)
    , media_source_type(0) {}


CustomMediaUrlParams::CustomMediaUrlParams(uint32_t preload_type,
                                           uint32_t media_source_type)
    : preload_type(preload_type)
    , media_source_type(media_source_type) {}

CustomMediaUrlParams::CustomMediaUrlParams(
    const CustomMediaUrlParams& other) = default;

CustomMediaUrlParams::~CustomMediaUrlParams() = default;

} // namespace media
