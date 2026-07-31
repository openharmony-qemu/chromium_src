// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "headless/test/headless_devtooled_browsertest.h"

#include <memory>

#include "base/check_deref.h"
#include "base/run_loop.h"
#include "base/values.h"
#include "build/build_config.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "headless/lib/browser/headless_web_contents_impl.h"
#include "headless/public/headless_browser_context.h"

namespace headless {

HeadlessDevTooledBrowserTest::HeadlessDevTooledBrowserTest() = default;
HeadlessDevTooledBrowserTest::~HeadlessDevTooledBrowserTest() = default;

void HeadlessDevTooledBrowserTest::RunTest() {
  HeadlessBrowserContext::Builder builder =
      browser()->CreateBrowserContextBuilder();
  CustomizeHeadlessBrowserContext(builder);
  browser_context_ = builder.Build();
  browser()->SetDefaultBrowserContext(browser_context_);

  browser_devtools_client_.AttachToBrowser();

  HeadlessWebContents::Builder web_contents_builder =
      browser_context_->CreateWebContentsBuilder();
  web_contents_builder.SetEnableBeginFrameControl(GetEnableBeginFrameControl());
  CustomizeHeadlessWebContents(web_contents_builder);
  web_contents_ = web_contents_builder.Build();
  Observe(HeadlessWebContentsImpl::From(web_contents_)->web_contents());

  PreRunAsynchronousTest();
  RunAsynchronousTest();
  PostRunAsynchronousTest();

  devtools_client_.DetachClient();
  Observe(nullptr);
  {
    // Keep raw_ptr<> happy and clear it before WC dies.
    HeadlessWebContents& wc = *web_contents_;
    web_contents_ = nullptr;
    wc.Close();
  }
  {
    HeadlessBrowserContext& bc = *browser_context_;
    browser_context_ = nullptr;
    browser_devtools_client_.DetachClient();
    bc.Close();
  }

  // Let the tasks that might have beein scheduled during web contents
  // being closed run (see https://crbug.com/1036627 for details).
  base::RunLoop().RunUntilIdle();
}

void HeadlessDevTooledBrowserTest::RenderViewReady() {
  if (had_render_view_ready_) {
    return;
  }
  had_render_view_ready_ = true;

  CHECK(HeadlessWebContentsImpl::From(web_contents_)
            ->web_contents()
            ->GetPrimaryMainFrame()
            ->IsRenderFrameLive());

  DevToolsTargetReady();

  devtools_client_.AttachToWebContents(
      HeadlessWebContentsImpl::From(web_contents_)->web_contents());

  RunDevTooledTest();
}

void HeadlessDevTooledBrowserTest::PrimaryMainFrameRenderProcessGone(
    base::TerminationStatus status) {
  if (status == base::TERMINATION_STATUS_NORMAL_TERMINATION)
    return;

  FinishAsynchronousTest();
  FAIL() << "Abnormal renderer termination (status=" << status << ")";
}

bool HeadlessDevTooledBrowserTest::GetEnableBeginFrameControl() {
  return false;
}

void HeadlessDevTooledBrowserTest::CustomizeHeadlessBrowserContext(
    HeadlessBrowserContext::Builder& builder) {}

void HeadlessDevTooledBrowserTest::CustomizeHeadlessWebContents(
    HeadlessWebContents::Builder& builder) {}

class HeadlessCreatedTargetIsUsedProcessTest
    : public HeadlessDevTooledBrowserTest,
      public testing::WithParamInterface<bool> {
 public:
  HeadlessCreatedTargetIsUsedProcessTest() = default;

  bool IsHiddenTarget() const { return GetParam(); }

 private:
  void RunDevTooledTest() override {
    base::DictValue params;
    params.Set("url", "");
    params.Set("hidden", IsHiddenTarget());
    browser_devtools_client_.SendCommand(
        "Target.createTarget", std::move(params),
        base::BindOnce(&HeadlessCreatedTargetIsUsedProcessTest::OnTargetCreated,
                       base::Unretained(this)));
  }

  void OnTargetCreated(base::DictValue result) {
    std::string target_id = DictString(result, "result.targetId");
    ASSERT_FALSE(target_id.empty());

    scoped_refptr<content::DevToolsAgentHost> agent_host =
        content::DevToolsAgentHost::GetForId(target_id);
    ASSERT_TRUE(agent_host);

    content::WebContents* web_contents = agent_host->GetWebContents();
    ASSERT_TRUE(web_contents);

    bool is_used =
        !web_contents->GetPrimaryMainFrame()->GetProcess()->IsUnused();
    EXPECT_EQ(is_used, IsHiddenTarget());

    FinishAsynchronousTest();
  }
};

INSTANTIATE_TEST_SUITE_P(
    /* no prefix */,
    HeadlessCreatedTargetIsUsedProcessTest,
    testing::Bool(),
    [](const testing::TestParamInfo<bool>& info) {
      return info.param ? "hidden" : "normal";
    });

HEADLESS_DEVTOOLED_TEST_P(HeadlessCreatedTargetIsUsedProcessTest);

}  // namespace headless
