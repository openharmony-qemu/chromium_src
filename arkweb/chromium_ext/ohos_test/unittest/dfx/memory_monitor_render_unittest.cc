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
#include "third_party/libc++/src/include/__ranges/lazy_split_view.h"
#define private public
#include "arkweb/chromium_ext/content/browser/dfx/memory_monitor_render_impl.h"
#undef private
#include "mojo/core/embedder/embedder.h"
#include "base/task/single_thread_task_executor.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

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
    std::shared_ptr<MemoryMonitorImpl> instance1 = MemoryMonitorImpl::GetInstance();
	std::shared_ptr<MemoryMonitorImpl> instance2 = MemoryMonitorImpl::GetInstance();
	EXPECT_EQ(instance1, instance2);
	EXPECT_NE(instance1, nullptr);
}

TEST_F(MemoryMonitorRenderTest, MemoryMonitorRenderTest002) {
	std::unique_ptr<base::SingleThreadTaskExecutor> task_executor =
		std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();
	bool initFlag = instance->IsInitialized();
	EXPECT_TRUE(!initFlag);
	
	instance->Trigger("www.baidu.com");
	initFlag = instance->IsInitialized();
	EXPECT_TRUE(initFlag);

	instance->Trigger("www.baidu.com");
	initFlag = instance->IsInitialized();
	EXPECT_TRUE(initFlag);
}

TEST_F(MemoryMonitorRenderTest, MemoryMonitorRenderTest003) {
	std::unique_ptr<base::SingleThreadTaskExecutor> task_executor =
		std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();

	MemoryMonitorImpl::DfxMemInfo mem_info;
	mem_info.pid = 0;
	instance->UpdateProcessBasicMemoryInfo(mem_info);
	EXPECT_NE(mem_info.pid, 0);

	mem_info.pid = 1;
	instance->UpdateProcessBasicMemoryInfo(mem_info);
	EXPECT_NE(mem_info.pid, 0);
}

TEST_F(MemoryMonitorRenderTest, MemoryMonitorRenderTest004) {
	std::unique_ptr<base::SingleThreadTaskExecutor> task_executor =
		std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();

	instance->mem_info_.pid = 1;
	instance->DfxMemSysParamObserve();

	instance->mem_info_.pid = 0;
	bool result = instance->DfxMemSysParamObserve();
	EXPECT_TRUE(!result);
}

TEST_F(MemoryMonitorRenderTest, MemoryMonitorRenderTest005) {
	std::unique_ptr<base::SingleThreadTaskExecutor> task_executor =
		std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();

	instance->mem_status_.error_threshold_counter = 60;
	instance->CollectAndReport();

	instance->mem_status_.error_threshold_counter = 0;
	instance->mem_status_.warning_threshold_counter = 60;
	instance->CollectAndReport();

	instance->mem_status_.error_threshold_counter = 0;
	instance->mem_status_.warning_threshold_counter = 0;
	instance->CollectAndReport();
	EXPECT_TRUE(!instance->mem_status_.error_reported);
	EXPECT_TRUE(!instance->mem_status_.warning_reported);
}

TEST_F(MemoryMonitorRenderTest, MemoryMonitorRenderTest006) {
	std::unique_ptr<base::SingleThreadTaskExecutor> task_executor =
		std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();

	instance->ReportToBrowser(true, MemoryMonitorImpl::MEM_LEAK_DETECTED);
	instance->ReportToBrowser(false, MemoryMonitorImpl::PAGE_MEM_LEAK_ERROR);
	EXPECT_NE(instance->mem_info_.pid, 0);
}

TEST_F(MemoryMonitorRenderTest, MemoryMonitorRenderTest007) {
	std::unique_ptr<base::SingleThreadTaskExecutor> task_executor =
		std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();

	instance->mem_status_.read_global_param = true;
	instance->MemoryAllocReport();

	instance->mem_status_.read_global_param = false;
	instance->mem_status_.upto_error_level = true;
	instance->MemoryAllocReport();
	
	instance->mem_status_.read_global_param = false;
	instance->mem_status_.upto_error_level = false;
	instance->MemoryAllocReport();
	EXPECT_NE(instance->mem_info_.pid, 0);
}

TEST_F(MemoryMonitorRenderTest, MemoryMonitorRenderTest008) {
	std::unique_ptr<base::SingleThreadTaskExecutor> task_executor =
		std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();

	EXPECT_NO_FATAL_FAILURE(instance->StartCollectBasicRenderMemory(true));
}

TEST_F(MemoryMonitorRenderTest, MemoryMonitorRenderTest009) {
	std::unique_ptr<base::SingleThreadTaskExecutor> task_executor =
		std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();

	EXPECT_NO_FATAL_FAILURE(instance->StartCollectBasicRenderMemory(false));
}

TEST_F(MemoryMonitorRenderTest, MemoryMonitorRenderTest010) {
	std::unique_ptr<base::SingleThreadTaskExecutor> task_executor =
		std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();

	EXPECT_NO_FATAL_FAILURE(instance->CollectBasicRenderMemory());
}

TEST_F(MemoryMonitorRenderTest, MemoryMonitorRenderTest011) {
	std::unique_ptr<base::SingleThreadTaskExecutor> task_executor =
		std::make_unique<base::SingleThreadTaskExecutor>(base::MessagePumpType::DEFAULT);
    std::shared_ptr<MemoryMonitorImpl> instance = MemoryMonitorImpl::GetInstance();

	MemoryMonitorImpl::RenderMemInfo render_info;
	render_info.pid = 1234;
	render_info.rss = 1024;
	render_info.pss = 2048;
	render_info.swap_pss = 512;
	render_info.fd_num = 100;
	render_info.oom_score_adj = -1000;
	render_info.js_heap_total = 4096;
	render_info.js_heap_used = 2048;
	render_info.pa = 8192;
	render_info.gpu_mem = 2048;

	EXPECT_EQ(render_info.pid, 1234);
	EXPECT_EQ(render_info.rss, 1024);
	EXPECT_EQ(render_info.pss, 2048);
	EXPECT_EQ(render_info.swap_pss, 512);
	EXPECT_EQ(render_info.fd_num, 100);
	EXPECT_EQ(render_info.oom_score_adj, -1000);
	EXPECT_EQ(render_info.js_heap_total, 4096);
	EXPECT_EQ(render_info.js_heap_used, 2048);
	EXPECT_EQ(render_info.pa, 8192);
	EXPECT_EQ(render_info.gpu_mem, 2048);
}
}//namespace content
