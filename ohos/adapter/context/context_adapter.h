// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 
#ifndef OHOS_ADAPTER_CONTEXT_CONTEXT_ADAPTER_H_
#define OHOS_ADAPTER_CONTEXT_CONTEXT_ADAPTER_H_
 
#include "ohos/adapter/export.h"
 
#include <atomic>
#include <cstdint>
 
#include "ohos/adapter/export.h"
 
namespace ohos::adapter {
 
enum class DeviceMode {
  kPcMode = 0,
  kNormalWindowMode,
  kFreeWindowsMode,
};
 
class ADAPTER_EXPORT_API ContextAdapter {
 public:
  static ContextAdapter& GetInstance();
  ContextAdapter(const ContextAdapter&) = delete;
  ContextAdapter(ContextAdapter&&) = delete;
  ContextAdapter& operator=(const ContextAdapter&) = delete;
  virtual ~ContextAdapter() = default;
 
  bool IsPcMode() {
    return device_mode_.load(std::memory_order_acquire) == DeviceMode::kPcMode;
  }
 
  void SetDeviceMode(DeviceMode device_mode) {
    device_mode_.store(device_mode, std::memory_order_release);
  }
 
  bool IsNormalWindowMode() {
    return device_mode_.load(std::memory_order_acquire) ==
           DeviceMode::kNormalWindowMode;
  }
  
  bool IsFreeWindowsMode() {
    return device_mode_.load(std::memory_order_acquire) ==
           DeviceMode::kFreeWindowsMode;
  }

  void SetLastActiveWidgetId(uint32_t last_active_widget_id) {
    last_active_widget_id_.store(last_active_widget_id, std::memory_order_relaxed);
  }
 
  uint32_t GetLastActiveWidgetId() {
    return last_active_widget_id_.load(std::memory_order_relaxed);
  }
 
 private:
  ContextAdapter();
  // Indicates whether the device is free-windows mode, normal-window mode, or
  // PC mode.
  std::atomic<DeviceMode> device_mode_{DeviceMode::kPcMode};
  std::atomic<uint32_t> last_active_widget_id_{0};
};
}  // namespace ohos::adapter
 
#endif  // OHOS_ADAPTER_CONTEXT_CONTEXT_ADAPTER_H_