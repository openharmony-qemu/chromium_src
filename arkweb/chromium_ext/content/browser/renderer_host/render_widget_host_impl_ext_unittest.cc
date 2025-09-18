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

#define protected public
#include "arkweb/chromium_ext/content/browser/renderer_host/render_widget_host_impl_ext.h"
#undef protected
#include "arkweb/chromium_ext/base/ohos/ltpo/include/sliding_observer.h"
#include "arkweb/chromium_ext/content/browser/renderer_host/render_widget_host_view_base_interface.h"
#include "base/command_line.h"
#include "components/input/switches.h"
#include "content/browser/gpu/gpu_process_host.h"
#include "content/browser/renderer_host/mock_render_widget_host.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/browser/renderer_host/render_widget_host_delegate.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "content/browser/renderer_host/visible_time_request_trigger.h"
#include "content/browser/scheduler/browser_task_executor.h"
#include "content/public/common/content_switches.h"
#include "content/public/test/mock_render_process_host.h"
#include "content/public/test/test_browser_context.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_render_view_host.h"
#include "ui/events/base_event_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace content {
namespace {

using ::testing::_;
using ::testing::Return;

class MockRenderWidgetHostDelegate : public RenderWidgetHostDelegate {
 public:
  MOCK_METHOD(RenderViewHostDelegateView*, GetDelegateView, (), (override));
  void ExecuteEditCommand(const std::string& command,
                          const std::optional<std::u16string>& value) override {
  }

  VisibleTimeRequestTrigger& GetVisibleTimeRequestTrigger() override {
    return visible_time_request_trigger_;
  }

  void Undo() override {}
  void Redo() override {}
  void Cut() override {}
  void Copy() override {}
  void Paste() override {}
  void PasteAndMatchStyle() override {}
  void SelectAll() override {}

 private:
  VisibleTimeRequestTrigger visible_time_request_trigger_;
};

class MockRenderViewHostDelegateView : public RenderViewHostDelegateView {
 public:
  MockRenderViewHostDelegateView() = default;
  MockRenderViewHostDelegateView(const MockRenderViewHostDelegateView&) =
      delete;
  MockRenderViewHostDelegateView& operator=(
      const MockRenderViewHostDelegateView&) = delete;
  ~MockRenderViewHostDelegateView() override = default;
  MOCK_METHOD(void, OnOverlayStateChanged, (const gfx::Rect&), (override));
  MOCK_METHOD(gfx::Rect, GetVisibleRectToWeb, (), (override));
};

class MockRenderWidgetHostView : public TestRenderWidgetHostView {
 public:
  explicit MockRenderWidgetHostView(RenderWidgetHostImpl* rwh)
      : TestRenderWidgetHostView(rwh) {}
  MOCK_METHOD(std::vector<int8_t>, GetWordSelection, (const std::string&, int8_t), (override));
  MOCK_METHOD(void, DynamicFrameLossEvent, (const std::string&, bool), (override));
  MOCK_METHOD(void, DidNativeEmbedMouseEvent, (const blink::mojom::NativeEmbedMouseEventPtr&), (override));
  MOCK_METHOD(void, DidNativeEmbedEvent, (const blink::mojom::NativeEmbedTouchEventPtr&), (override));
};

class RenderWidgetHostProcess : public MockRenderProcessHost {
 public:
  explicit RenderWidgetHostProcess(BrowserContext* browser_context)
      : MockRenderProcessHost(browser_context) {
  }

  RenderWidgetHostProcess(const RenderWidgetHostProcess&) = delete;
  RenderWidgetHostProcess& operator=(const RenderWidgetHostProcess&) = delete;

  ~RenderWidgetHostProcess() override {}

  bool IsInitializedAndNotDead() override { return true; }
};

class RenderWidgetHostImplExtTest : public testing::Test  {
 protected:
  void SetUp() override {
    base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
    command_line->AppendSwitch(input::switches::kValidateInputEventStream);
    browser_context_ = std::make_unique<TestBrowserContext>();
    process_ =
        std::make_unique<RenderWidgetHostProcess>(browser_context_.get());
    site_instance_group_ =
        base::WrapRefCounted(SiteInstanceGroup::CreateForTesting(
            browser_context_.get(), process_.get()));
    sink_ = &process_->sink();

    host_ = std::make_unique<RenderWidgetHostImplExt>(
        nullptr, // frame_tree
        false, // self_owned
        RenderWidgetHostImpl::DefaultFrameSinkId(
            *site_instance_group_->GetSafeRef(), process_->GetNextRoutingID()), // frame_sink_id
        &delegate_, // delegate
        site_instance_group_->GetSafeRef(), // site_instance_group
        process_->GetNextRoutingID(), // routing_id
        false, // hidden
        false, // renderer_initiated_creation
        std::make_unique<FrameTokenMessageQueue>() // frame_token_message_queue
    );

    mock_view_ = std::make_unique<MockRenderWidgetHostView>(host_.get());

    host_->SetView(mock_view_.get());

    mock_delegate_view_ = std::make_unique<MockRenderViewHostDelegateView>();
    
    EXPECT_CALL(delegate_, GetDelegateView())
        .WillRepeatedly(Return(mock_delegate_view_.get()));
  }

  void TearDown() override {
    sink_ = nullptr;
    host_->SetView(nullptr);
    host_.reset();
    process_->Cleanup();
    site_instance_group_.reset();
    process_.reset();
    browser_context_.reset();
  }

  BrowserTaskEnvironment task_environment_{
      base::test::TaskEnvironment::TimeSource::MOCK_TIME};
  std::unique_ptr<RenderWidgetHostImplExt> host_;
  MockRenderWidgetHostDelegate delegate_;
  std::unique_ptr<MockRenderWidgetHostView> mock_view_;
  std::unique_ptr<MockRenderViewHostDelegateView> mock_delegate_view_;
  std::unique_ptr<TestBrowserContext> browser_context_;
  std::unique_ptr<RenderWidgetHostProcess> process_;
  scoped_refptr<SiteInstanceGroup> site_instance_group_;
  raw_ptr<IPC::TestSink> sink_;
};

TEST_F(RenderWidgetHostImplExtTest, GetWordSelection_WithView) {
  std::string text = "hello world";
  int8_t offset = 5;
  std::vector<int8_t> expected_selection = {4, 6};

  EXPECT_CALL(*mock_view_, GetWordSelection(text, offset))
      .WillOnce(Return(expected_selection));

  bool callback_called = false;
  host_->GetWordSelection(text, offset,
      base::BindOnce([](bool* called, const std::vector<int8_t>& result) {
        *called = true;
        EXPECT_EQ(result.size(), 2);
      }, &callback_called));

  EXPECT_TRUE(callback_called);
}

TEST_F(RenderWidgetHostImplExtTest, GetWordSelection_NoView) {
  host_->SetView(nullptr);

  std::string text = "hello world";
  int8_t offset = 5;
  bool callback_called = false;

  host_->GetWordSelection(text, offset,
      base::BindOnce([](bool* called, const std::vector<int8_t>& result) {
        *called = true;
        EXPECT_TRUE(result.empty());
      }, &callback_called));

  EXPECT_FALSE(callback_called);
}

TEST_F(RenderWidgetHostImplExtTest, OnOverlayStateChanged_WithDelegateView) {
  gfx::Rect image_rect(0, 0, 100, 100);

  EXPECT_CALL(*mock_delegate_view_, OnOverlayStateChanged(image_rect))
      .Times(1);

  host_->OnOverlayStateChanged(image_rect);
}

TEST_F(RenderWidgetHostImplExtTest, GetVisibleRectToWeb_NoDelegateView) {
  EXPECT_CALL(delegate_, GetDelegateView())
      .WillOnce(Return(nullptr));

  bool callback_called = false;
  gfx::Rect result_rect;
  
  host_->GetVisibleRectToWeb(
      base::BindOnce([](bool* called, gfx::Rect* result, const gfx::Rect& rect) {
        *called = true;
        *result = rect;
      }, &callback_called, &result_rect));

  EXPECT_TRUE(callback_called);
  EXPECT_TRUE(result_rect.IsEmpty());
}

TEST_F(RenderWidgetHostImplExtTest, GetVisibleRectToWeb_NoView) {
  host_->SetView(nullptr);
  
  bool callback_called = false;
  gfx::Rect result_rect;
  
  host_->GetVisibleRectToWeb(
      base::BindOnce([](bool* called, gfx::Rect* result, const gfx::Rect& rect) {
        *called = true;
        *result = rect;
      }, &callback_called, &result_rect));
  
  EXPECT_TRUE(callback_called);
  EXPECT_TRUE(result_rect.IsEmpty());
}

TEST_F(RenderWidgetHostImplExtTest, GetVisibleRectToWeb_WithDelegateView) {
  gfx::Rect expected_rect(0, 0, 100, 100);

  EXPECT_CALL(*mock_delegate_view_, GetVisibleRectToWeb())
      .WillOnce(Return(expected_rect));

  bool callback_called = false;
  gfx::Rect result_rect;
  
  host_->GetVisibleRectToWeb(
      base::BindOnce([](bool* called, gfx::Rect* result, const gfx::Rect& rect) {
        *called = true;
        *result = rect;
      }, &callback_called, &result_rect));

  EXPECT_TRUE(callback_called);
  EXPECT_EQ(result_rect.width(), 100);
  EXPECT_EQ(result_rect.height(), 100);
}

TEST_F(RenderWidgetHostImplExtTest, DynamicFrameLossEvent_WithView) {
  std::string sceneId = "test_scene";
  bool isStart = true;
  EXPECT_CALL(*mock_view_, DynamicFrameLossEvent(sceneId, isStart))
      .Times(1);
  host_->DynamicFrameLossEvent(sceneId, isStart); 
}

TEST_F(RenderWidgetHostImplExtTest, DynamicFrameLossEvent_NoView) {
  std::string sceneId = "test_scene";
  bool isStart = true;
  host_->SetView(nullptr);

  EXPECT_CALL(*mock_view_, DynamicFrameLossEvent(sceneId, isStart))
      .Times(0);

  ASSERT_NO_FATAL_FAILURE(host_->DynamicFrameLossEvent(sceneId, isStart));
}

TEST_F(RenderWidgetHostImplExtTest, DidNativeEmbedEvent) {
  blink::mojom::NativeEmbedTouchEventPtr touchEvent =
      blink::mojom::NativeEmbedTouchEvent::New();

  EXPECT_CALL(*mock_view_, DidNativeEmbedEvent(testing::_))
      .Times(1);

  host_->DidNativeEmbedEvent(std::move(touchEvent));
}

TEST_F(RenderWidgetHostImplExtTest, DidNativeEmbedEvent_NoView) {
  blink::mojom::NativeEmbedTouchEventPtr touchEvent =
      blink::mojom::NativeEmbedTouchEvent::New();
  host_->SetView(nullptr);

  EXPECT_CALL(*mock_view_, DidNativeEmbedEvent(testing::_))
      .Times(0);

  ASSERT_NO_FATAL_FAILURE(host_->DidNativeEmbedEvent(std::move(touchEvent)));
}

TEST_F(RenderWidgetHostImplExtTest, DidNativeEmbedMouseEvent) {
  blink::mojom::NativeEmbedMouseEventPtr mouseEvent =
      blink::mojom::NativeEmbedMouseEvent::New();
  
  EXPECT_CALL(*mock_view_, DidNativeEmbedMouseEvent(testing::_))
      .Times(1);
  
  host_->DidNativeEmbedMouseEvent(std::move(mouseEvent));
}

TEST_F(RenderWidgetHostImplExtTest, DidNativeEmbedMouseEvent_NoView) {
  blink::mojom::NativeEmbedMouseEventPtr mouseEvent =
      blink::mojom::NativeEmbedMouseEvent::New();
  host_->SetView(nullptr);

  EXPECT_CALL(*mock_view_, DidNativeEmbedMouseEvent(testing::_))
      .Times(0);

  ASSERT_NO_FATAL_FAILURE(host_->DidNativeEmbedMouseEvent(std::move(mouseEvent)));
}

TEST_F(RenderWidgetHostImplExtTest, SendCurrentLanguage_WithView) {
  std::string language = "en-US";
  ASSERT_NO_FATAL_FAILURE(host_->SendCurrentLanguage(language));
}

TEST_F(RenderWidgetHostImplExtTest, SendCurrentLanguage_NoView) {
  std::string language = "en-US";
  host_->SetView(nullptr);
  ASSERT_NO_FATAL_FAILURE(host_->SendCurrentLanguage(language));
}

}  // namespace
}  // namespace content
