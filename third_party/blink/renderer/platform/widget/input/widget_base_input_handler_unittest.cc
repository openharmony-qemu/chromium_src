// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// NOTE(ArkWeb144 backport): this file is a reduced port of the upstream
// widget_base_input_handler_unittest.cc. Only the regression test for the
// GetTextInputType lifetime fix (crrev.com/c/8299776, Bug 553129513) is carried
// over. The upstream touch/gesture/mouse destruction cases belong to other
// upstream issues and are out of scope for this backport round, so they are
// intentionally omitted here; their production guards are already present in
// this baseline (widget_base_input_handler.cc weak_ptr_factory_ checks).

#include "third_party/blink/renderer/platform/widget/input/widget_base_input_handler.h"

#include <memory>

#include "base/functional/callback_helpers.h"
#include "base/test/task_environment.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/common/input/web_coalesced_input_event.h"
#include "third_party/blink/public/common/input/web_touch_event.h"
#include "third_party/blink/public/mojom/widget/platform_widget.mojom-blink.h"
#include "third_party/blink/public/platform/scheduler/test/renderer_scheduler_test_support.h"
#include "third_party/blink/renderer/platform/scheduler/public/dummy_schedulers.h"
#include "third_party/blink/renderer/platform/scheduler/public/page_scheduler.h"
#include "third_party/blink/renderer/platform/scheduler/test/fake_widget_scheduler.h"
#include "third_party/blink/renderer/platform/widget/compositing/test/stub_widget_base_client.h"
#include "third_party/blink/renderer/platform/widget/widget_base.h"
#include "ui/display/screen_infos.h"

namespace blink {

class MockWidgetBaseClient : public StubWidgetBaseClient {
 public:
  MOCK_METHOD(WebInputEventResult,
              HandleInputEvent,
              (const WebCoalescedInputEvent&),
              (override));
  MOCK_METHOD(WebInputEventResult, DispatchBufferedTouchEvents, (), (override));
  MOCK_METHOD(void,
              WillHandleGestureEvent,
              (const WebGestureEvent&, bool*),
              (override));
  MOCK_METHOD(void, WillHandleMouseEvent, (const WebMouseEvent&), (override));
  MOCK_METHOD(WebTextInputType, GetTextInputType, (), (override));
  const display::ScreenInfos& GetOriginalScreenInfos() override {
    return screen_infos_;
  }
  display::ScreenInfos screen_infos_{display::ScreenInfo()};
};

class WidgetBaseInputHandlerTest : public testing::Test {
 public:
  WidgetBaseInputHandlerTest()
      : widget_scheduler_(
            base::MakeRefCounted<scheduler::FakeWidgetScheduler>()) {}

  void SetUp() override {
    mojo::AssociatedRemote<mojom::blink::WidgetHost> widget_host_remote;
    mojo::PendingAssociatedReceiver<mojom::blink::WidgetHost>
        widget_host_receiver =
            widget_host_remote.BindNewEndpointAndPassDedicatedReceiver();

    mojo::AssociatedRemote<mojom::blink::Widget> widget_remote;
    mojo::PendingAssociatedReceiver<mojom::blink::Widget> widget_receiver =
        widget_remote.BindNewEndpointAndPassDedicatedReceiver();

    widget_base_ = std::make_unique<WidgetBase>(
        &client_, widget_host_remote.Unbind(), std::move(widget_receiver),
        scheduler::GetSingleThreadTaskRunnerForTesting(),
        /*hidden=*/false, /*never_composited=*/false,
        /*is_embedded=*/false,
        /*is_for_scalable_page=*/false);

    page_scheduler_ = scheduler::CreateDummyPageScheduler();
    display::ScreenInfo screen_info;
    display::ScreenInfos screen_infos(screen_info);
    widget_base_->InitializeCompositing(*page_scheduler_, screen_infos, nullptr,
                                        nullptr, nullptr);
  }

 protected:
  base::test::TaskEnvironment task_environment_;
  MockWidgetBaseClient client_;
  scoped_refptr<scheduler::FakeWidgetScheduler> widget_scheduler_;
  std::unique_ptr<PageScheduler> page_scheduler_;
  std::unique_ptr<WidgetBase> widget_base_;
};

TEST_F(WidgetBaseInputHandlerTest, UpdateTextInputStateDestroysWidget) {
  EXPECT_CALL(client_, GetTextInputType()).WillOnce([&]() {
    widget_base_->Shutdown(false);
    widget_base_.reset();
    return WebTextInputType::kWebTextInputTypeText;
  });

  widget_base_->UpdateTextInputState();
}

}  // namespace blink
