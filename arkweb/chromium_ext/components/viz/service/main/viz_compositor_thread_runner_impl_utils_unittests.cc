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

#include "arkweb/chromium_ext/components/viz/service/main/viz_compositor_thread_runner_impl_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace viz {

class VizCompositorThreadRunnerImplUtilsTest : public testing::Test {
public:
    VizCompositorThreadRunnerImplUtilsTest() = default;
    ~VizCompositorThreadRunnerImplUtilsTest() = default;
};

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
TEST_F(VizCompositorThreadRunnerImplUtilsTest, ReportThreadForInit) {
    VizCompositorThreadRunnerImplUtils util(nullptr);
    std::unique_ptr<base::Thread> thread = std::make_unique<base::Thread>("test");
    base::Thread::Options thread_options;
    thread_options.message_pump_type = base::MessagePumpType::IO;
    thread->StartWithOptions(std::move(thread_options));

    ASSERT_NO_FATAL_FAILURE(ReportThreadForInit(thread));
}

TEST_F(VizCompositorThreadRunnerImplUtilsTest, ReportThreadForDestroy) {
    VizCompositorThreadRunnerImplUtils util(nullptr);
    std::unique_ptr<base::Thread> thread = std::make_unique<base::Thread>("test");
    base::Thread::Options thread_options;
    thread_options.message_pump_type = base::MessagePumpType::IO;
    thread->StartWithOptions(std::move(thread_options));

    ASSERT_NO_FATAL_FAILURE(util.ReportThreadForDestroy(thread));
}
#endif
} //namespace viz