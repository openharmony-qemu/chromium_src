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

#include "mojo/public/cpp/bindings/pending_remote.h"

#define private public
#include "third_party/blink/renderer/core/frame/local_frame_client_impl.h"
#undef private

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/local_frame_client_impl_utils.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/core/exported/web_view_impl.h"
#include "arkweb/chromium_ext/third_party/blink/public/web/web_local_frame_client_ext.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/public/platform/web_policy_container.h"
#include "third_party/blink/public/common/tokens/tokens.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/testing/testing_platform_support.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"

namespace blink {

class LocalFrameClientImplUtilsTest : public PageTestBase {
 protected:
  void SetUp() override {
    PageTestBase::SetUp(gfx::Size());
    platform_ = std::make_unique<ScopedTestingPlatformSupport<TestingPlatformSupport>>();
    
  }

  WebLocalFrameImpl* CreateTestChildFrame(frame_test_helpers::TestWebFrameClient* client,
    WebLocalFrameImpl* main_frame_, frame_test_helpers::WebViewHelper &helper_) {
    if (!main_frame_ || !main_frame_->GetFrame() || !client) {
      return nullptr;
    }
    
    return frame_test_helpers::CreateLocalChild(
        *main_frame_,
        mojom::blink::TreeScopeType::kDocument,
        client,
        WebPolicyContainerBindParams(),
        [](WebLocalFrame* frame, const DocumentToken&,
           CrossVariantMojoRemote<mojom::BrowserInterfaceBrokerInterfaceBase>) {
        });
  }
  std::unique_ptr<ScopedTestingPlatformSupport<TestingPlatformSupport>> platform_;

};

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)

TEST_F(LocalFrameClientImplUtilsTest, NotifyLcpForBlankless_AllSixConditions) {
  frame_test_helpers::WebViewHelper helper_;
  WebViewImpl* web_view_ = helper_.Initialize();
  WebLocalFrameImpl* main_frame_ = To<WebLocalFrameImpl>(web_view_->MainFrame());
  Persistent<LocalFrameClientImpl> frame_client_impl_ = MakeGarbageCollected<LocalFrameClientImpl>(main_frame_);
  Member<LocalFrameClientImplUtils> utils_ = MakeGarbageCollected<LocalFrameClientImplUtils>(frame_client_impl_);


  frame_client_impl_->web_frame_ = nullptr;
  utils_->NotifyLcpForBlankless();

  frame_client_impl_->web_frame_ = main_frame_;
  auto* child_frame = CreateTestChildFrame(nullptr, main_frame_, helper_);
  frame_client_impl_->web_frame_ = child_frame;
  utils_->NotifyLcpForBlankless();

  auto* child_frame_with_default_client = CreateTestChildFrame(nullptr, main_frame_, helper_);
  frame_client_impl_->web_frame_ = child_frame_with_default_client;
  utils_->NotifyLcpForBlankless();

  auto client_ext = std::make_unique<frame_test_helpers::TestWebFrameClient>();
  auto* child_frame_with_ext = CreateTestChildFrame(client_ext.get(), main_frame_, helper_);
  frame_client_impl_->web_frame_ = child_frame_with_ext;
  utils_->NotifyLcpForBlankless();
}

#endif  // BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)

#if BUILDFLAG(ARKWEB_JSPROXY)
TEST_F(LocalFrameClientImplUtilsTest, RunScriptsAtHeadElementAvailable_NullFrameAndNullClient) {
  frame_test_helpers::WebViewHelper helper_;
  WebViewImpl* web_view_ = helper_.Initialize();
  WebLocalFrameImpl* main_frame_ = To<WebLocalFrameImpl>(web_view_->MainFrame());
  Persistent<LocalFrameClientImpl> frame_client_impl_ = MakeGarbageCollected<LocalFrameClientImpl>(main_frame_);
  Member<LocalFrameClientImplUtils> utils_ = MakeGarbageCollected<LocalFrameClientImplUtils>(frame_client_impl_);


  frame_client_impl_->web_frame_ = nullptr;
  utils_->RunScriptsAtHeadElementAvailable();

  auto* child_frame = CreateTestChildFrame(nullptr, main_frame_, helper_);
  frame_client_impl_->web_frame_ = child_frame;
  utils_->RunScriptsAtHeadElementAvailable();
}

#endif  // BUILDFLAG(ARKWEB_JSPROXY)

}  // namespace blink
