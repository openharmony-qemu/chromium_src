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

#ifndef GPU_VULKAN_VULKAN_SURFACE_EXT_H_
#define GPU_VULKAN_VULKAN_SURFACE_EXT_H_

#define SWAP_CHAIN_DESTROY(swap_chain) \
  Finish(); \
  if (swap_chain) { \
    (swap_chain)->Destroy(); \
    swap_chain = nullptr; \
  }

#define GET_IMAGE_COUNT(min_image_count, window_) \
  uint32_t imageCount = OHOS::NWeb::WindowAdapterNdkImpl::GetInstance() \
      .GetNativeWindowQueueSize((window_).get()); \
  min_image_count = std::max((min_image_count), imageCount); \
  LOG(INFO) << "VulkanSurface::CreateSwapChain min_image_count = " \
            << (min_image_count)

#define UNREF_VULKAN_WINDOW(window_) \
  OHOS::NWeb::WindowAdapterNdkImpl::GetInstance() \
      .NativeWindowUnRef((window_).get()); \
  LOG(INFO) << "VulkanSurface ~VulkanSurface unref window ref."; \
  window_ = nullptr

#define ADDREF_VULKAN_WINDOW(swap_chain_, window_, accelerated_widget) \
  swap_chain_ = nullptr; \
  window_ = \
        NWebNativeWindowTracker::GetInstance()->GetNativeWindow(accelerated_widget); \
  OHOS::NWeb::WindowAdapterNdkImpl::GetInstance() \
      .AddNativeWindowRef((window_).get()); \
  LOG(INFO) << "VulkanSurface::VulkanSurface add window ref."

#endif  // GPU_VULKAN_VULKAN_SURFACE_EXT_H_