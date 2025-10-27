/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_ADAPTER_COMMON_H
#define NWEB_ADAPTER_COMMON_H

#include <memory>
#include <surface.h>
#include "nweb.h"

namespace OHOS::NWeb {
struct NWebWindowInfo {
    uint32_t width = 0;
    uint32_t height = 0;
    sptr<SurfaceBuffer> cachedSurfaceBuffer = nullptr;
};

struct VsyncCbInfo {
    std::weak_ptr<NWeb> nwebWeak;
};
} // namespace OHOS

#endif // NWEB_ADAPTER_COMMON_H