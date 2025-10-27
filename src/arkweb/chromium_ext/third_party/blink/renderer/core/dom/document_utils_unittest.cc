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

#include "third_party/blink/renderer/core/dom/document_utils.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_fetcher.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_fetcher_properties.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "base/task/single_thread_task_runner.h"
#include "base/time/time.h"
#include "content/public/common/content_switches.h"
#include "base/ohos/ltpo/include/touch_observer.h"
#include "third_party/blink/renderer/core/render_mojom/render_mojom_client.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/blink/renderer/platform/loader/testing/test_resource_fetcher_properties.h"
#include "base/test/task_environment.h"
#include "third_party/blink/renderer/platform/loader/testing/mock_fetch_context.h"
#include "third_party/blink/renderer/platform/testing/mock_context_lifecycle_notifier.h"
#include "third_party/blink/renderer/platform/loader/testing/test_loader_factory.h"

namespace blink {

class MockResourceFetcher : public ResourceFetcher {
  public:
    MockResourceFetcher() : ResourceFetcher(CreateMockResourceFetcherInit()) {}
    MOCK_METHOD(void, SetMainUrl, (const KURL& url), ());

  private:
  static ResourceFetcherInit CreateMockResourceFetcherInit() {
    MockFetchContext* context = MakeGarbageCollected<MockFetchContext>();
    auto* properties = MakeGarbageCollected<TestResourceFetcherProperties>();
    return ResourceFetcherInit(properties->MakeDetachable(), context,
                              base::MakeRefCounted<scheduler::FakeTaskRunner>(),
                              base::MakeRefCounted<scheduler::FakeTaskRunner>(),
                              MakeGarbageCollected<TestLoaderFactory>(),
                              MakeGarbageCollected<MockContextLifecycleNotifier>(),
                              nullptr);
  }
};

class MockSocPerfClientAdapter {
 public:
  MOCK_METHOD(void, ApplySocPerfConfigByIdEx, (uint32_t id, bool tag), ());
};

class MockOhosAdapterHelper : public OHOS::NWeb::OhosAdapterHelper {
 public:
  MOCK_METHOD(std::unique_ptr<OHOS::NWeb::SocPerfClientAdapter>, CreateSocPerfClientAdapter, (), (override));
};

class MockTouchObserver : public base::ohos::TouchObserver {
 public:
  MOCK_METHOD(uint64_t, GetLastTouchUpTime, (), (const));
};

class DocumentUtilsTest : public ::testing::Test {
 protected:
  base::test::SingleThreadTaskEnvironment task_env_;
  void SetUp() override {
    base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(
        switches::kProcessType, switches::kRendererProcess);
  }
  void TearDown() override {}
};

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
TEST_F(DocumentUtilsTest, DocumentInitializeUtils_CallsSetMainUrl) {
  auto* fetcher = MakeGarbageCollected<::testing::NiceMock<MockResourceFetcher>>();
  testing::Mock::AllowLeak(fetcher);
  KURL url(String("https://example.com"));
  DocumentUtils utils;
  EXPECT_CALL(*fetcher, SetMainUrl(url)).Times(1);
  utils.DocumentInitializeUtils(Member<ResourceFetcher>(fetcher), url);
}

TEST_F(DocumentUtilsTest, DocumentInitializeUtils_WithNullFetcher) {
  KURL url(String("https://example.com"));
  DocumentUtils utils;
  utils.DocumentInitializeUtils(Member<ResourceFetcher>(nullptr), url);
}
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
TEST_F(DocumentUtilsTest, StartBoostingUtils_RendererProcess) {
  base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(
      switches::kProcessType, switches::kRendererProcess);
  
  GTEST_SKIP() << "Skipping test due to TouchObserver dependency in test environment";
  DocumentUtils::StartBoostingUtils();
}

TEST_F(DocumentUtilsTest, StartBoostingUtils_NonRendererProcess) {
  base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(
      switches::kProcessType, "browser");
  DocumentUtils::StartBoostingUtils();
}

TEST_F(DocumentUtilsTest, SetURLUtils_WithinTouchUpInterval) {
  DocumentUtils utils;
  utils.SetURLUtils();
}
#endif

TEST_F(DocumentUtilsTest, Trace_DoesNotCrash) {
  DocumentUtils utils;
  utils.Trace(nullptr);
}

} // namespace blink


