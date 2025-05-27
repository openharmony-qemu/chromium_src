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
#ifndef GPU_IPC_SERVICE_GPU_INIT_EXT_H_
#define GPU_IPC_SERVICE_GPU_INIT_EXT_H_

#define CHECK_IS_USING_VULKAN_AND_INITIAL() \
    if (features::IsUsingVulkan()) { \
        bool result = InitializeVulkan(); \
        CHECK(result); \
    } else {

#endif
