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
#ifndef THIRD_PARTY_SKIA_SRC_GPU_GANESH_VK_GRVKRENDERPASS_EXT_H_
#define THIRD_PARTY_SKIA_SRC_GPU_GANESH_VK_GRVKRENDERPASS_EXT_H_

#define CHECK_IS_FOR_OHOS(skipSetSubpassDep, currentDependency, createInfo) \
    if ((skipSetSubpassDep) && (currentDependency) == 1) { \
        (createInfo).dependencyCount = 0; \
        (createInfo).pDependencies = nullptr; \
    } else { \
        (createInfo).dependencyCount = currentDependency; \
        (createInfo).pDependencies = dependencies; \
    }

#endif
