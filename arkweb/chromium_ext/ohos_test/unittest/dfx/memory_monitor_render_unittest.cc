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
#include "arkweb/chromium_ext/content/browser/dfx/memory_monitor_render_impl.h"
#include "mojo/core/embedder/embedder.h"
#include "base/task/single_thread_task_executor.h"

#include <gtest/gtest.h>

namespace content {

class MemoryMonitorRenderTest : public testing ::Test {
public: 
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {
		mojo::core::Init();
	}
    void TearDown() {}
};

TEST_F(MemoryMonitorRenderTest, MemoryMonitorRenderTest001) {
	std::unique_ptr<base::SingleThreadTaskExecutor> task_executor = 
		std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();
	bool initFlag = instance->IsInitialized();
	EXPECT_TRUE(!initFlag);
	
	instance->Trigger("www.baidu.com");
	initFlag = instance->IsInitialized();
	EXPECT_TRUE(initFlag);
}
}//namespace content