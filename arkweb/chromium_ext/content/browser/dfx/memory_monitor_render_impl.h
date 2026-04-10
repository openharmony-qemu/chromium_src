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

#ifndef CONTENT_RENDERER_MEMORY_MEMORY_MONITOR_IMPL_H_
#define CONTENT_RENDERER_MEMORY_MEMORY_MONITOR_IMPL_H_

#include <cstdint>
#include <memory>
#include <string>

#include "arkweb/chromium_ext/content/browser/dfx/mojom/dfx_reporting.mojom.h"
#include "base/process/process_metrics.h"
#include "base/timer/timer.h"
#include "mojo/public/cpp/bindings/remote.h"

namespace content {

class MemoryMonitorImpl {
public:
  MemoryMonitorImpl() = default;
  ~MemoryMonitorImpl()
  {
    timer_.Stop();
  }

  static constexpr char MEM_LEAK_DETECTED[] = "MEM_LEAK_DETECTED";
  static constexpr char PAGE_MEM_LEAK_ERROR[] = "PAGE_MEM_LEAK_ERROR";
  static constexpr char PAGE_MEM_LEAK_WARNING[] = "PAGE_MEM_LEAK_WARNING";

  static std::shared_ptr<MemoryMonitorImpl> GetInstance();

  void StartCollectBasicRenderMemory(bool is_hidden);
  void Trigger(std::string url)
  {
    if (!has_initialized_) {
      Init();
    }
    mem_info_.url = url;
  }
  bool IsInitialized()
  {
    return has_initialized_;
  }
  void GpuMemSetToMemInfo(size_t gpu_mem);
  
  mojo::PendingReceiver<dfx::mojom::DfxReporter> &GetPendingReceiver()
  {
    return receiver_;
  }

  struct DfxMemInfo {
    pid_t pid = 0;  // Process ID of the monitored process
    size_t rss = 0;  // Resident Set Size (RSS) in KB
    size_t pss = 0;  // Proportional Set Size (PSS) in KB
    uint32_t fd_num = 0;  // Number of file descriptors opened by the process
    size_t js_heap_total = 0;  // Total JavaScript heap size in KB
    size_t js_heap_used = 0;  // Used JavaScript heap size in KB
    size_t gpu_mem = 0;  // GPU memory usage in KB
    std::string url; // URL
  };

  struct RenderMemInfo {
    pid_t pid = 0;  // Process ID of the monitored process
    size_t rss = 0;  // Resident Set Size (RSS) in KB
    size_t pss = 0;  // Proportional Set Size (PSS) in KB
    size_t swap_pss = 0;  // Proportional Set Size (SwapPSS) in KB
    uint32_t fd_num = 0;  // Number of file descriptors opened by the process
    int32_t oom_score_adj = 0;  // OOM killer adjustment score
    size_t js_heap_total = 0;  // Total JavaScript heap size in KB
    size_t js_heap_used = 0;  // Used JavaScript heap size in KB
    size_t pa = 0;  // PartitionAlloc memory usage in KB
    size_t gpu_mem = 0;  // GPU memory usage in KB
  };

  struct DfxMemStatus {
    uint8_t warning_threshold_counter = 0;  // Counter for warning threshold crossings
    uint8_t error_threshold_counter = 0;  // Counter for error threshold crossings
    bool warning_reported = false;  // Indicates if a warning has been reported
    bool error_reported = false;  // Indicates if an error has been reported
    bool upto_error_level = false;  // Indicates if memory usage has reached error level
    bool read_global_param = false;  // Indicates if global parameter has been read
    std::string lastTime;  // Last time of global parameter
  };

private:
  void StartMonitoring(base::TimeDelta interval);
  void CollectAndReport();
  void Init();
  void MemoryAllocReport();
  void ReportToBrowser(const bool reportSysEvent, const std::string& type);
  bool DfxMemSysParamObserve();
  void UpdateProcessBasicMemoryInfo(DfxMemInfo &mem_info);
  void UpdateProcessMemoryInfo(DfxMemInfo &mem_info);
  void CollectBasicRenderMemory();

  mojo::Remote<dfx::mojom::DfxReporter> remote_;
  mojo::PendingReceiver<dfx::mojom::DfxReporter> receiver_ =
      remote_.BindNewPipeAndPassReceiver();
  base::RepeatingTimer timer_;
  DfxMemInfo mem_info_;
  DfxMemStatus mem_status_;
  bool has_initialized_ = false;
};

} // namespace content
#endif // CONTENT_RENDERER_MEMORY_MEMORY_MONITOR_IMPL_H_
