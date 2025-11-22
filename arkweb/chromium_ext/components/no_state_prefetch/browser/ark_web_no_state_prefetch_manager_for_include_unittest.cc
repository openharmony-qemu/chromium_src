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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <set>
#include <utility>

#include "arkweb/build/features/features.h"
#include "base/sequence_checker.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "base/time/time.h"
#define private public
#include "components/no_state_prefetch/browser/no_state_prefetch_handle.h"
#include "components/no_state_prefetch/browser/no_state_prefetch_manager.h"
#undef private
#include "content/browser/preloading/prefetch/prefetch_status.h"
#include "content/browser/preloading/preloading_attempt_impl.h"
#include "content/public/browser/preloading.h"
#include "content/public/browser/preloading_data.h"
#include "content/public/common/content_features.h"
#include "content/public/common/content_switches.h"
#include "content/public/test/test_browser_context.h"
#include "content/test/test_web_contents.h"

namespace content {
class MockPreloadingAttemptImpl : public PreloadingAttemptImpl {
 public:
  ~MockPreloadingAttemptImpl() override {}
  MockPreloadingAttemptImpl(
      const PreloadingPredictor& creating_predictor,
      const PreloadingPredictor& enacting_predictor,
      PreloadingType preloading_type,
      ukm::SourceId triggered_primary_page_source_id,
      PreloadingURLMatchCallback url_match_predicate,
      std::optional<PreloadingType> planned_max_preloading_type,
      uint32_t sampling_seed)
      : PreloadingAttemptImpl(creating_predictor,
                              enacting_predictor,
                              preloading_type,
                              triggered_primary_page_source_id,
                              std::move(url_match_predicate),
                              planned_max_preloading_type,
                              sampling_seed) {}
  void SetEligibility(PreloadingEligibility eligibility) {
    eligibility_ = eligibility;
  }
  void SetHoldbackStatus(PreloadingHoldbackStatus holdback_status) override {}
  bool ShouldHoldback() override {}
  void SetTriggeringOutcome(
      PreloadingTriggeringOutcome triggering_outcome) override {}
  void SetFailureReason(PreloadingFailureReason reason) override {
    failure_reason_ = reason;
  }
  base::WeakPtr<PreloadingAttempt> GetWeakPtr() override {
    return this->weak_factory_.GetWeakPtr();
  }
  PreloadingFailureReason GetFailureReason() { return failure_reason_; }

 private:
  PreloadingEligibility eligibility_;
  PreloadingFailureReason failure_reason_;
  base::WeakPtrFactory<PreloadingAttemptImpl> weak_factory_{this};
};
}  // namespace content

namespace prerender {
#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
class MockNoStatePrefetchManager : public NoStatePrefetchManager {
 public:
  MockNoStatePrefetchManager(
      content::BrowserContext* browser_context,
      std::unique_ptr<NoStatePrefetchManagerDelegate> delegate)
      : NoStatePrefetchManager(browser_context, std::move(delegate)) {}

  MockNoStatePrefetchManager(const MockNoStatePrefetchManager&) = delete;
  MockNoStatePrefetchManager& operator=(const MockNoStatePrefetchManager&) =
      delete;

  ~MockNoStatePrefetchManager() = default;
  bool MayHitOmniboxUrl(const GURL& url,
                        Origin origin,
                        base::WeakPtr<content::PreloadingAttempt> attempt) {
    return NoStatePrefetchManager::MayHitOmniboxUrl(url, origin, attempt);
  }
  std::set<GURL>& GetPrefetchUrls() { return oh_prefetch_urls_; }
};

class NoStatePrefetchManagerTest : public content::RenderViewHostTestHarness {
 public:
  NoStatePrefetchManagerTest()
      : RenderViewHostTestHarness(
            base::test::TaskEnvironment::TimeSource::MOCK_TIME),
        url_("https://example.com"),
        attempt_(content::preloading_predictor::kUrlPointerDownOnAnchor,
                 content::preloading_predictor::kUrlPointerDownOnAnchor,
                 content::PreloadingType::kPreconnect,
                 ukm::AssignNewSourceId(),
                 content::PreloadingData::GetSameURLMatcher(url_),
                 /*planned_max_preloading_type=*/std::nullopt,
                 /*sampling_seed=*/1ul) {}

 protected:
  void SetUp() {
    content::RenderViewHostTestHarness::SetUp();
    base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
    command_line->AppendSwitch(switches::kSingleProcess);
    no_state_prefetch_manager_ = std::make_unique<MockNoStatePrefetchManager>(
        browser_context(), nullptr);
  }

  void TearDown() {
    no_state_prefetch_manager_.reset();
    content::RenderViewHostTestHarness::TearDown();
  }

  GURL url_;
  content::MockPreloadingAttemptImpl attempt_;
  scoped_refptr<content::SessionStorageNamespace> session_storage_namespace_;
  std::unique_ptr<MockNoStatePrefetchManager> no_state_prefetch_manager_;
};

TEST_F(NoStatePrefetchManagerTest, NoStatePrefetchManager_NoUrl) {
  gfx::Size size;
  // empty url
  auto handle = no_state_prefetch_manager_->StartOhPrefetchingFromOmnibox(
      GURL(), session_storage_namespace_.get(), size, &attempt_, "header");
  EXPECT_EQ(handle, nullptr);
  EXPECT_TRUE(no_state_prefetch_manager_->GetPrefetchUrls().empty());

  // invalid url
  handle = no_state_prefetch_manager_->StartOhPrefetchingFromOmnibox(
      GURL("ghht/xxxxx"), session_storage_namespace_.get(), size, &attempt_,
      "header");
  EXPECT_EQ(handle, nullptr);
  EXPECT_TRUE(no_state_prefetch_manager_->GetPrefetchUrls().empty());

  // not found
  Origin origin = ORIGIN_LINK_REL_NEXT;
  auto ret =
      no_state_prefetch_manager_->MayHitOmniboxUrl(url_, origin, nullptr);
  EXPECT_FALSE(ret);

  // valid url, no PreloadingAttempt
  handle = no_state_prefetch_manager_->StartOhPrefetchingFromOmnibox(
      url_, session_storage_namespace_.get(), size, nullptr, "header");
  EXPECT_EQ(handle, nullptr);
  EXPECT_FALSE(no_state_prefetch_manager_->GetPrefetchUrls().empty());
}

TEST_F(NoStatePrefetchManagerTest, NoStatePrefetchManager_Url) {
  gfx::Size size;
  auto handle = no_state_prefetch_manager_->StartOhPrefetchingFromOmnibox(
      url_, session_storage_namespace_.get(), size, &attempt_, "header");
  EXPECT_EQ(handle, nullptr);
  EXPECT_EQ(no_state_prefetch_manager_->GetPrefetchUrls().size(), 1u);
  handle = no_state_prefetch_manager_->StartOhPrefetchingFromOmnibox(
      GURL("http://example2.com"), session_storage_namespace_.get(), size,
      &attempt_, "header");
  EXPECT_EQ(handle, nullptr);
  EXPECT_EQ(no_state_prefetch_manager_->GetPrefetchUrls().size(), 2u);

  // found
  Origin origin = ORIGIN_LINK_REL_NEXT;
  auto ret =
      no_state_prefetch_manager_->MayHitOmniboxUrl(url_, origin, nullptr);
  EXPECT_TRUE(ret);
  EXPECT_EQ(no_state_prefetch_manager_->GetPrefetchUrls().size(), 1u);
  // not found, and no attempt
  ret = no_state_prefetch_manager_->MayHitOmniboxUrl(url_, origin, nullptr);
  EXPECT_FALSE(ret);
  // has attempt
  ret = no_state_prefetch_manager_->MayHitOmniboxUrl(url_, origin,
                                                     attempt_.GetWeakPtr());
  EXPECT_FALSE(ret);
  auto reason = static_cast<int>(content::PreloadingFailureReason::
                                     kPreloadingFailureReasonContentEnd) +
                static_cast<int>(FINAL_STATUS_TOO_MANY_PROCESSES);
  EXPECT_EQ(static_cast<int>(attempt_.GetFailureReason()), reason);
  EXPECT_EQ(no_state_prefetch_manager_->GetPrefetchUrls().size(), 1u);
}
#endif
}  // namespace prerender