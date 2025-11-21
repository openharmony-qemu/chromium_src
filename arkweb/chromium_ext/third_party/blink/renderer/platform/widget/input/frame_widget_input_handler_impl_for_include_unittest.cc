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

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/memory/weak_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/task_environment.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "mojo/public/cpp/bindings/pending_associated_receiver.h"
#include "mojo/public/cpp/bindings/lib/interface_ptr_state.h"
#include "third_party/blink/renderer/platform/scheduler/public/thread_scheduler.h"
#include "third_party/blink/public/platform/scheduler/test/renderer_scheduler_test_support.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/mojom/input/input_handler.mojom-blink.h"
#include "third_party/blink/renderer/platform/widget/widget_base.h"
#include "third_party/blink/renderer/platform/widget/widget_base_client.h"
#include "third_party/blink/renderer/platform/widget/input/frame_widget_input_handler_impl.h"

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/widget/input/widget_input_handler_manager_utils.h"

namespace blink {

// Stub WidgetBaseClient for testing
class StubWidgetBaseClient : public WidgetBaseClient {
 public:
  void OnCommitRequested() override {}
  void BeginMainFrame(base::TimeTicks) override {}
  void UpdateLifecycle(WebLifecycleUpdate, DocumentUpdateReason) override {}
  std::unique_ptr<cc::LayerTreeFrameSink> AllocateNewLayerTreeFrameSink() override {
    return nullptr;
  }
  KURL GetURLForDebugTrace() override { return {}; }
  WebInputEventResult DispatchBufferedTouchEvents() override {
    return WebInputEventResult::kNotHandled;
  }
  WebInputEventResult HandleInputEvent(const WebCoalescedInputEvent&) override {
    return WebInputEventResult::kNotHandled;
  }
  bool SupportsBufferedTouchEvents() override { return false; }
  void WillHandleGestureEvent(const WebGestureEvent&, bool* suppress) override {}
  void WillHandleMouseEvent(const WebMouseEvent&) override {}
  void ObserveGestureEventAndResult(const WebGestureEvent&,
                                    const gfx::Vector2dF&,
                                    const cc::OverscrollBehavior&,
                                    bool) override {}
  void FocusChanged(mojom::blink::FocusState) override {}
  void UpdateVisualProperties(const VisualProperties& visual_properties) override {}
  const display::ScreenInfos& GetOriginalScreenInfos() override {
    return screen_infos_;
  }
  gfx::Rect ViewportVisibleRect() override { return gfx::Rect(); }

 private:
  display::ScreenInfos screen_infos_;
};

class MockWidgetBase : public WidgetBase {
 public:
  MockWidgetBase() : WidgetBase(
      &client_,
      CreateWidgetHostRemote(),
      CreateWidgetReceiver(),
      scheduler::GetSingleThreadTaskRunnerForTesting(),
      false, false, false, true) {}
  
  MOCK_METHOD(void, SelectRangeV2, (const gfx::Point&, bool));
  MOCK_METHOD(void, ShowFreeCopyMenu, ());
#if BUILDFLAG(ARKWEB_AI)
  MOCK_METHOD(void, OnDataDetectorSelectText, ());
#endif  // BUILDFLAG(ARKWEB_AI)

 private:
  CrossVariantMojoAssociatedRemote<mojom::WidgetHostInterfaceBase> CreateWidgetHostRemote() {
    mojo::AssociatedRemote<mojom::blink::WidgetHost> widget_host_remote;
    std::ignore = widget_host_remote.BindNewEndpointAndPassDedicatedReceiver();
    return widget_host_remote.Unbind();
  }
  
  CrossVariantMojoAssociatedReceiver<mojom::WidgetInterfaceBase> CreateWidgetReceiver() {
    mojo::AssociatedRemote<mojom::blink::Widget> widget_remote;
    mojo::PendingAssociatedReceiver<mojom::blink::Widget> widget_receiver =
        widget_remote.BindNewEndpointAndPassDedicatedReceiver();
    return std::move(widget_receiver);
  }

  StubWidgetBaseClient client_;
};

class MockFrameWidgetInputHandler : public mojom::blink::FrameWidgetInputHandler {
 public:
  MockFrameWidgetInputHandler() = default;
  
  base::WeakPtr<MockFrameWidgetInputHandler> GetWeakPtr() {
    return weak_ptr_factory_.GetWeakPtr();
  }
  
  MOCK_METHOD(void, SelectRangeV2, (const gfx::Point&, bool), (override));
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  MOCK_METHOD(void, ShowFreeCopyMenu, (), (override));
#endif
#if BUILDFLAG(ARKWEB_AI)
  MOCK_METHOD(void, OnDataDetectorSelectText, (), (override));
#endif  // BUILDFLAG(ARKWEB_AI)
  void AddImeTextSpansToExistingText(uint32_t, uint32_t, const Vector<ui::ImeTextSpan>&) override {}
  void ClearImeTextSpansByType(uint32_t, uint32_t, ui::ImeTextSpan::Type) override {}
  void SetCompositionFromExistingText(int32_t, int32_t, const Vector<ui::ImeTextSpan>&) override {}
  void ExtendSelectionAndDelete(int32_t, int32_t) override {}
  void ExtendSelectionAndReplace(uint32_t, uint32_t, const String&) override {}
  void DeleteSurroundingText(int32_t, int32_t) override {}
  void DeleteSurroundingTextInCodePoints(int32_t, int32_t) override {}
  void SetEditableSelectionOffsets(int32_t, int32_t) override {}
  void HandleStylusWritingGestureAction(
    mojom::blink::StylusWritingGestureDataPtr, HandleStylusWritingGestureActionCallback) override {}
  void ExecuteEditCommand(const String&, const String&) override {}
  void Undo() override {}
  void Redo() override {}
  void Cut() override {}
  void Copy() override {}
  void CopyToFindPboard() override {}
  void CenterSelection() override {}
  void Paste() override {}
  void PasteAndMatchStyle() override {}
  void Replace(const String&) override {}
  void ReplaceMisspelling(const String&) override {}
  void Delete() override {}
  void SelectAll() override {}
  void CollapseSelection() override {}
  void SelectRange(const gfx::Point&, const gfx::Point&) override {}
  void SelectAroundCaret(mojom::blink::SelectionGranularity, bool, bool, SelectAroundCaretCallback) override {}
  void AdjustSelectionByCharacterOffset(int32_t, int32_t, blink::mojom::SelectionMenuBehavior) override {}
  void MoveRangeSelectionExtent(const gfx::Point&) override {}
  void ScrollFocusedEditableNodeIntoView() override {}
  void WaitForPageScaleAnimationForTesting(WaitForPageScaleAnimationForTestingCallback) override {}
  void MoveCaret(const gfx::Point&) override {}

 private:
  base::WeakPtrFactory<MockFrameWidgetInputHandler> weak_ptr_factory_{this};
};

class FrameWidgetInputHandlerImplTest : public testing::Test {
 protected:
  void SetUp() override {
    task_environment_ = std::make_unique<base::test::TaskEnvironment>();
  }

  void TearDown() override {
    task_environment_.reset();
  }

  std::unique_ptr<base::test::TaskEnvironment> task_environment_;
};

#if BUILDFLAG(ARKWEB_MENU)
TEST_F(FrameWidgetInputHandlerImplTest, SelectRangeV2_WidgetNull_ReturnsEarly) {
  base::WeakPtr<WidgetBase> null_widget;
  auto mock_handler = std::make_unique<MockFrameWidgetInputHandler>();
  
  FrameWidgetInputHandlerImpl handler(null_widget, 
                                     mock_handler->GetWeakPtr(),
                                     nullptr);
  
  handler.SelectRangeV2(gfx::Point(10, 20), true);
  
  task_environment_->RunUntilIdle();
}

TEST_F(FrameWidgetInputHandlerImplTest, SelectRangeV2_WidgetValid_CallsHandler) {
  auto mock_widget = std::make_unique<MockWidgetBase>();
  auto mock_handler = std::make_unique<MockFrameWidgetInputHandler>();
  
  ASSERT_NE(mock_widget->GetWeakPtr().get(), nullptr);
  
  FrameWidgetInputHandlerImpl handler(mock_widget->GetWeakPtr(),
                                     mock_handler->GetWeakPtr(),
                                     nullptr);
  
  
  handler.SelectRangeV2(gfx::Point(10, 20), true);
  
  task_environment_->RunUntilIdle();
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
TEST_F(FrameWidgetInputHandlerImplTest, ShowFreeCopyMenu_WidgetNull_ReturnsEarly) {
  base::WeakPtr<WidgetBase> null_widget;
  auto mock_handler = std::make_unique<MockFrameWidgetInputHandler>();
  
  FrameWidgetInputHandlerImpl handler(null_widget,
                                     mock_handler->GetWeakPtr(),
                                     nullptr);
  
  handler.ShowFreeCopyMenu();
  
  task_environment_->RunUntilIdle();
}

TEST_F(FrameWidgetInputHandlerImplTest, ShowFreeCopyMenu_WidgetValid_CallsHandler) {
  auto mock_widget = std::make_unique<MockWidgetBase>();
  auto mock_handler = std::make_unique<MockFrameWidgetInputHandler>();
  
  ASSERT_NE(mock_widget->GetWeakPtr().get(), nullptr);
  
  FrameWidgetInputHandlerImpl handler(mock_widget->GetWeakPtr(),
                                     mock_handler->GetWeakPtr(),
                                     nullptr);
  
  handler.ShowFreeCopyMenu();
  
  task_environment_->RunUntilIdle();
}
#endif

}  // namespace blink
