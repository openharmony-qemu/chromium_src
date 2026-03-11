/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <fuzzer/FuzzedDataProvider.h>
#include "third_party/libc++/src/include/__ranges/lazy_split_view.h"
#define private public
#include "arkweb/chromium_ext/content/browser/dfx/memory_monitor_render_impl.h"
#undef private

#include "base/at_exit.h"
#include "base/functional/bind.h"
#include "base/task/single_thread_task_executor.h"
#include "mojo/core/embedder/embedder.h"

namespace content {
namespace {
  constexpr size_t MAX_STRING_LENGTH = 255;

void FuzzMemoryMonitor(FuzzedDataProvider* dataProvider) {
    std::unique_ptr<base::SingleThreadTaskExecutor> task_executor =
        std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    // 临时创建一个实例来测试
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();
    if (instance) {
        std::string url = dataProvider->ConsumeRandomLengthString(MAX_STRING_LENGTH);
        // Test Trigger with random URL
        instance->Trigger(url);

        // Test initializedInstance() method
        bool initializedInstance = instance->IsInitialized();
        if (!initializedInstance) {
            return;
        }

        MemoryMonitorImpl::DfxMemInfo mem_info;
        mem_info.pid = dataProvider->ConsumeIntegral<uint32_t>();
        instance->UpdateProcessBasicMemoryInfo(mem_info);

        // Test initializedInstance after Trigger
        initializedInstance = instance->IsInitialized();
        // 补充其他方法
        instance->mem_info_.pid = 1;
        instance->mem_status_.lastTime = "2026Y";
        instance->DfxMemSysParamObserve();
        
        instance->mem_info_.pss = 1500 * 1024 + 1;
        instance->CollectAndReport();
        instance->ReportToBrowser(dataProvider->ConsumeBool(), MemoryMonitorImpl::MEM_LEAK_DETECTED);
        instance->ReportToBrowser(dataProvider->ConsumeBool(), url);
        instance->mem_status_.read_global_param = dataProvider->ConsumeBool();
        instance->MemoryAllocReport();
        instance->mem_info_.pid = 0;
        instance->ReadProcFile("/proc/self/mock", "NSpid:", instance->mem_info_.pid);
    }
}
}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }
  // 确保环境初始化
  static bool initialized = false;
  if (!initialized) {
    mojo::core::Init();
    initialized = true;
  }
  FuzzedDataProvider dataProvider(data, size);
  FuzzMemoryMonitor(&dataProvider);
  return 0;
}

}   // namespace content