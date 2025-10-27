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
#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_COMPOSITING_LAYER_TREE_SETTINGS_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_COMPOSITING_LAYER_TREE_SETTINGS_UTILS_H_

#include "third_party/blink/renderer/platform/widget/compositing/layer_tree_settings.h"

namespace blink {

void AdjustMemoryLimitBasedOnScreenWidth(cc::ManagedMemoryPolicy& actual,
                                         const gfx::Size& initial_screen_size,
                                         float initial_device_scale_factor);
size_t EstimatePhysicalMemory();
void SetMaxVisibleBytes(cc::ManagedMemoryPolicy& actual);
void ConfigureOverlayScrollbarSettings(cc::LayerTreeSettings& settings);
void AdjustGraphicsSettings(const gfx::Size& screen_size,
                            cc::LayerTreeSettings& settings,
                            bool using_low_memory_policy);
void SetEnableDeleteUnusedResourcesDelay(cc::LayerTreeSettings& settings);
}  // namespace blink

#endif
