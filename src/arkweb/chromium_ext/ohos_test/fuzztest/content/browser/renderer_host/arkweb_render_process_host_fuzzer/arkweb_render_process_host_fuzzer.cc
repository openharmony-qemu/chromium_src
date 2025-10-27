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

#include <fuzzer/FuzzedDataProvider.h>

#include "content/browser/storage_partition_impl.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/test_browser_context.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "base/files/file_path.h"
#include "base/test/task_environment.h"
#define private public
#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_render_process_host_impl_utils.h"
#include "content/browser/renderer_host/render_process_host_impl.h"

namespace content {

class TestRenderProcessHostImpl : public RenderProcessHostImpl {
 public:
  TestRenderProcessHostImpl(BrowserContext* browser_context,
                            StoragePartitionImpl* storage_partition,
                            int flags)
      : RenderProcessHostImpl(browser_context, storage_partition, flags) {}

  const base::TimeTicks& ProcessBackgroundTime() override {
    static base::TimeTicks dummy_time;
    return dummy_time;
  }

  void ReportEngineEvent(const std::string& mod,
                         const std::string& resource,
                         const std::string& error_code,
                         const std::string& error_msg) override {
  }
};

class ArkwebRenderProcessUtilsFuzzer {
 public:
  ArkwebRenderProcessUtilsFuzzer()
      : task_environment_(base::test::TaskEnvironment::MainThreadType::UI) {
    browser_context_ = std::make_unique<content::TestBrowserContext>();
    browser_context_->set_is_off_the_record(true);

    StoragePartition* partition =
        browser_context_->GetDefaultStoragePartition();
    storage_partition_ = static_cast<content::StoragePartitionImpl*>(partition);
  }

  std::unique_ptr<TestRenderProcessHostImpl> CreateRenderProcessHost() {
    return std::make_unique<TestRenderProcessHostImpl>(browser_context_.get(),
                                                       storage_partition_,
                                                       0  // flags
    );
  }

  ArkwebRenderProcessHostImplUtils* GetUtils(TestRenderProcessHostImpl* host) {
    return host->arkweb_render_process_host_impl_utils_.get();
  }

 private:
  base::test::TaskEnvironment task_environment_;
  std::unique_ptr<content::TestBrowserContext> browser_context_;
  content::StoragePartitionImpl* storage_partition_ = nullptr;
};

void FuzzArkwebRenderProcessUtils(FuzzedDataProvider* fdp) {
  ArkwebRenderProcessUtilsFuzzer test_env;
  auto host1 = test_env.CreateRenderProcessHost();
  auto host2 = test_env.CreateRenderProcessHost();
  auto* utils1 = host1->arkweb_render_process_host_impl_utils_.get();
  auto* utils2 = host2->arkweb_render_process_host_impl_utils_.get();

  if (!utils1 || !utils2) {
    return;
  }

  std::string token1 = fdp->ConsumeRandomLengthString(32);
  std::string token2 = fdp->ConsumeRandomLengthString(32);
  std::string invalid_token = fdp->ConsumeRandomLengthString(32);
  std::string token3 = fdp->ConsumeRandomLengthString(32);

  utils1->RegisteProcessForSharedToken(token1, host1.get());
  utils1->GetProcessForSharedToken(token1);

  utils2->RegisteProcessForSharedToken(token2, host2.get());
  utils1->GetProcessForSharedToken(token2);

  utils1->GetProcessForSharedToken(invalid_token);

  utils1->RemoveFromSharedRenderProcessMap(host1.get());
  utils1->GetProcessForSharedToken(token1);

  utils2->RemoveFromSharedRenderProcessMap(host2.get());
  utils1->GetProcessForSharedToken(token2);

  utils1->RegisteProcessForSharedToken(token3, host1.get());
  utils2->RegisteProcessForSharedToken(token3, host2.get());
  utils1->GetProcessForSharedToken(token3);

  utils1->RemoveFromSharedRenderProcessMap(host2.get());
}

}  // namespace content

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }
  FuzzedDataProvider fdp(data, size);
  content::FuzzArkwebRenderProcessUtils(&fdp);
  return 0;
}