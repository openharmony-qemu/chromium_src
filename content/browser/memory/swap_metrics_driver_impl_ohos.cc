/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "content/browser/memory/swap_metrics_driver_impl_ohos.h"

#include <memory>

#include "base/memory/ptr_util.h"
#include "base/process/process_metrics.h"
#include "base/time/time.h"
#include "content/public/browser/swap_metrics_driver.h"

namespace content {

namespace {
using SwapMetricsUpdateResult = SwapMetricsDriver::SwapMetricsUpdateResult;
bool HasSwap() {
  base::SystemMemoryInfoKB memory_info;
  if (!base::GetSystemMemoryInfo(&memory_info)) {
    return false;
  }
  return memory_info.swap_total > 0;
}

}  // namespace

// static
std::unique_ptr<SwapMetricsDriver> SwapMetricsDriver::Create(
    std::unique_ptr<Delegate> delegate,
    const base::TimeDelta update_interval) {
  return HasSwap() ? base::WrapUnique<SwapMetricsDriver>(
                         new SwapMetricsDriverImplOhos(std::move(delegate),
                                                        update_interval))
                   : std::unique_ptr<SwapMetricsDriver>();
}

SwapMetricsDriverImplOhos::SwapMetricsDriverImplOhos(
    std::unique_ptr<Delegate> delegate,
    const base::TimeDelta update_interval)
    : SwapMetricsDriverImpl(std::move(delegate), update_interval) {}

SwapMetricsDriverImplOhos::~SwapMetricsDriverImplOhos() = default;

SwapMetricsUpdateResult SwapMetricsDriverImplOhos::UpdateMetricsInternal(
    base::TimeDelta interval) {
  base::VmStatInfo vmstat;
  if (!base::GetVmStatInfo(&vmstat)) {
    return SwapMetricsDriver::SwapMetricsUpdateResult::kSwapMetricsUpdateFailed;
  }

  uint64_t in_counts = vmstat.pswpin - last_pswpin_;
  uint64_t out_counts = vmstat.pswpout - last_pswpout_;
  last_pswpin_ = vmstat.pswpin;
  last_pswpout_ = vmstat.pswpout;

  if (interval.is_zero())
    return SwapMetricsDriver::SwapMetricsUpdateResult::
        kSwapMetricsUpdateSuccess;

  delegate_->OnSwapInCount(in_counts, interval);
  delegate_->OnSwapOutCount(out_counts, interval);

  return SwapMetricsDriver::SwapMetricsUpdateResult::kSwapMetricsUpdateSuccess;
}

}  // namespace content
