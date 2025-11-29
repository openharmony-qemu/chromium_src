// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_PROCESS_H
#define OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_PROCESS_H

#include <cstdint>
#include <future>

namespace ohos::adapter::multiprocess {

class GpuNativeProcess {
 public:
  int GetPid();

  void AddWindow(const std::string& window_id, void* window);

  void RemoveWindow(const std::string& window_id);

  void SetWindowWidget(void* window, int32_t widget_id);

  void SetInitializeResult(bool result);

  bool GetInitializeResult();

  static GpuNativeProcess& GetInstance();

 private:
  GpuNativeProcess() = default;

  ~GpuNativeProcess() = default;

  GpuNativeProcess(const GpuNativeProcess&);

  GpuNativeProcess& operator=(const GpuNativeProcess&);

  /*
   * Indicate whether WindowAdapter initialize success.
   * If WindowAdapter initialize failed,
   * GPU process will exit after telling the parent process
   * an invalid PID.
   */
  std::promise<bool> initialize_success_;
};

}  // namespace ohos::adapter::multiprocess

#endif  // OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_PROCESS_H
