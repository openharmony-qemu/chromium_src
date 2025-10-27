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

#include "nweb_enhance_surface_adapter.h"

#include <securec.h>
#include <sync_fence.h>

#include "graphic_common.h"
#include "graphic_common_c.h"
#include "nweb_log.h"
#include "surface_type.h"

namespace OHOS::NWeb {
class NWebDefaultOutputFrameCallback : public NWebOutputFrameCallback {
public:
    NWebDefaultOutputFrameCallback() = default;
    ~NWebDefaultOutputFrameCallback() = default;

    bool Handle(const char* buffer, uint32_t width, uint32_t height) override
    {
        return true;
    }
};

NWebEnhanceSurfaceAdapter &NWebEnhanceSurfaceAdapter::Instance()
{
    static NWebEnhanceSurfaceAdapter surfaceAdapter;
    return surfaceAdapter;
}

std::shared_ptr<NWebCreateInfoImpl> NWebEnhanceSurfaceAdapter::GetCreateInfo(void *enhanceSurfaceInfo,
    std::shared_ptr<NWebEngineInitArgs> initArgs, uint32_t width, uint32_t height, bool incognitoMode)
{
    std::shared_ptr<NWebCreateInfoImpl> createInfo = std::make_shared<NWebCreateInfoImpl>();
    createInfo->SetWidth(width);
    createInfo->SetHeight(height);
    createInfo->SetEngineInitArgs(initArgs);
    createInfo->SetIsIncognitoMode(incognitoMode);
    createInfo->SetEnhanceSurfaceInfo(enhanceSurfaceInfo);
    createInfo->SetOutputFrameCallback(std::make_shared<NWebDefaultOutputFrameCallback>());
    return createInfo;
}
} // namespace OHOS::NWeb
