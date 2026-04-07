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
#include "arkweb/ohos_nweb/src/sysevent/oh_web_performance_timing.h"

namespace OHOS::NWeb {
namespace {
    constexpr uint32_t MAX_UINT32_SIZE = 255;
    constexpr int32_t MAX_INT32_SIZE = 255;
    constexpr int32_t MIN_INT32_SIZE = -255;
    constexpr int64_t MAX_INT64_SIZE = 9999999999;
}

void OhWebPerformanceTimingFuzz001Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    OhWebPerformanceTiming timing;
    
    // Test Reset() method
    timing.Reset();
    
    // Test field assignment with random values
    timing.navigation_id = dataProvider.ConsumeIntegral<int64_t>();
    timing.navigation_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.redirect_count = dataProvider.ConsumeIntegral<uint32_t>();
    timing.redirect_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.redirect_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.fetch_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.worker_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.domain_lookup_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.domain_lookup_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.connect_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.secure_connect_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.connect_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.request_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.response_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.response_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.dom_interactive = dataProvider.ConsumeIntegral<int64_t>();
    timing.dom_content_loaded_event_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.dom_content_loaded_event_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.load_event_start = dataProvider.ConsumeIntegral<int64_t>();
    timing.load_event_end = dataProvider.ConsumeIntegral<int64_t>();
    timing.first_paint = dataProvider.ConsumeIntegral<int64_t>();
    timing.first_contentful_paint = dataProvider.ConsumeIntegral<int64_t>();
    timing.largest_contentful_paint = dataProvider.ConsumeIntegral<int64_t>();
    timing.render_init_block = dataProvider.ConsumeIntegral<int64_t>();
    timing.input_time = dataProvider.ConsumeIntegral<int64_t>();
    timing.is_paint_done = dataProvider.ConsumeBool();
    timing.first_meaningful_paint = dataProvider.ConsumeIntegral<int64_t>();
    
    // Test Reset() again after modifications
    timing.Reset();
}

void OhWebPerformanceTimingFuzz002Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test multiple instances
    for (int i = 0; i < 5; i++) {
        OhWebPerformanceTiming timing;
        timing.Reset();
        
        // Assign some values
        timing.navigation_id = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
        timing.navigation_start = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
        timing.first_paint = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
        timing.is_paint_done = dataProvider.ConsumeBool();
    }
}

void OhWebPerformanceTimingFuzz003Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    OhWebPerformanceTiming timing;
    timing.Reset();
    
    // Test with boundary values
    timing.navigation_id = INT64_MAX;
    timing.navigation_start = 0;
    timing.redirect_count = UINT32_MAX;
    timing.is_paint_done = true;
    
    // Reset and test again
    timing.Reset();
    
    timing.navigation_id = INT64_MIN;
    timing.navigation_start = INT64_MAX;
    timing.redirect_count = 0;
    timing.is_paint_done = false;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    OhWebPerformanceTimingFuzz001Test(data, size);
    OhWebPerformanceTimingFuzz002Test(data, size);
    OhWebPerformanceTimingFuzz003Test(data, size);
    return 0;
}

}   // namespace OHOS::NWeb