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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_SKIA_RENDERER_EXT_H_
#define COMPONENTS_VIZ_SERVICE_DISPLAY_SKIA_RENDERER_EXT_H_

#define UPDATE_SWAP_BUFFER_RECT(swap_buffer_rect_) \
    if (!features::IsUsingVulkan()) { \
        swap_buffer_rect_ = current_frame()->damage_rect; \
    } else { \
        swap_buffer_rect_ = current_frame()->root_damage_rect; \
    }

#define UPDATE_SWAP_BUFFER_RECT_FOR_VULKAN(rect, swap_buffer_rect_) \
    if (features::IsUsingVulkan()) { \
        gfx::Rect rect = gfx::Rect(surface_size_for_swap_buffers()); \
        (rect).set_x((swap_buffer_rect_).x()); \
        (rect).set_y((swap_buffer_rect_).y()); \
        (swap_buffer_rect_).Intersect(rect); \
    } else { \
        (swap_buffer_rect_).Intersect(gfx::Rect(surface_size_for_swap_buffers())); \
    }

#endif
