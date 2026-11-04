// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/renderer_host/render_widget_host_view_child_frame.h"

#include <stdint.h>

#include <memory>
#include <tuple>
#include <utility>

#include "base/memory/raw_ptr.h"
#include "base/run_loop.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/task_environment.h"
#include "build/build_config.h"
#include "components/input/child_frame_input_helper.h"
#include "components/viz/common/hit_test/hit_test_query.h"
#include "components/viz/common/surfaces/parent_local_surface_id_allocator.h"
#include "components/viz/host/host_frame_sink_manager.h"
#include "components/viz/test/begin_frame_args_test.h"
#include "components/viz/test/fake_external_begin_frame_source.h"
#include "components/viz/test/host_frame_sink_manager_test_api.h"
#include "content/browser/compositor/surface_utils.h"
#include "content/browser/gpu/compositor_util.h"
#include "content/browser/renderer_host/cross_process_frame_connector.h"
#include "content/browser/renderer_host/frame_token_message_queue.h"
#include "content/browser/renderer_host/frame_tree.h"
#include "content/browser/renderer_host/render_widget_host_delegate.h"
#include "content/browser/renderer_host/render_widget_host_factory.h"
#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/browser/site_instance_group.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/site_instance.h"
#include "content/public/browser/web_contents.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/fake_frame_widget.h"
#include "content/public/test/mock_render_process_host.h"
#include "content/public/test/test_browser_context.h"
#include "content/public/test/test_image_transport_factory.h"
#include "content/public/test/test_web_contents_factory.h"
#include "content/test/mock_render_widget_host_delegate.h"
#include "content/test/mock_widget.h"
#include "content/test/test_render_view_host.h"
#include "content/test/test_render_widget_host.h"
#include "content/test/test_web_contents.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/common/frame/frame_visual_properties.h"
#include "third_party/blink/public/common/input/synthetic_web_input_event_builders.h"
#include "ui/base/ui_base_features.h"
#include "ui/compositor/compositor.h"

namespace content {
namespace {

const viz::LocalSurfaceId kArbitraryLocalSurfaceId(
    1,
    base::UnguessableToken::CreateForTesting(2, 3));

}  // namespace

class MockChildFrameInputHelper : public input::ChildFrameInputHelper {
 public:
  explicit MockChildFrameInputHelper(input::RenderWidgetHostViewInput* view,
                                     Delegate* delegate)
      : ChildFrameInputHelper(view, delegate) {}
  ~MockChildFrameInputHelper() override = default;

  bool BubbleScrollEvent(const blink::WebGestureEvent& event) override {
    last_bubbled_event_type_ = event.GetType();
    return can_bubble_;
  }

  blink::WebInputEvent::Type GetAndResetLastBubbledEventType() {
    blink::WebInputEvent::Type last = last_bubbled_event_type_;
    last_bubbled_event_type_ = blink::WebInputEvent::Type::kUndefined;
    return last;
  }

  void SetCanBubble(bool can_bubble) { can_bubble_ = can_bubble; }

 private:
  blink::WebInputEvent::Type last_bubbled_event_type_ =
      blink::WebInputEvent::Type::kUndefined;
  bool can_bubble_ = true;
};

class MockFrameConnector : public CrossProcessFrameConnector {
 public:
  explicit MockFrameConnector() : CrossProcessFrameConnector(nullptr) {}
  ~MockFrameConnector() override = default;

  void FirstSurfaceActivation(const viz::SurfaceInfo& surface_info) override {
    last_surface_info_ = surface_info;
  }

  void SetViewportIntersection(
      const gfx::Rect& viewport_intersection,
      const gfx::Rect& main_frame_intersection,
      const gfx::Rect& compositor_visible_rect,
      blink::mojom::FrameOcclusionState occlusion_state) {
    intersection_state_.viewport_intersection = viewport_intersection;
    intersection_state_.main_frame_intersection = main_frame_intersection;
    intersection_state_.compositor_visible_rect = compositor_visible_rect;
    intersection_state_.occlusion_state = occlusion_state;
  }

  RenderWidgetHostViewBase* GetParentRenderWidgetHostView() override {
    return nullptr;
  }

  RenderWidgetHostViewBase* GetRootRenderWidgetHostView() override {
    return root_view_;
  }

  void SetRootRenderWidgetHostView(RenderWidgetHostViewBase* root_view) {
    root_view_ = root_view;
  }

  viz::SurfaceInfo last_surface_info_;

 private:
  raw_ptr<RenderWidgetHostViewBase> root_view_;
};

// A mock root RenderWidgetHostView for keyboard tooltip tests. It records
// tooltip text/bounds forwarded via UpdateTooltipFromKeyboard and tracks
// ClearKeyboardTriggeredTooltip calls.
class MockRenderWidgetHostView : public TestRenderWidgetHostView {
 public:
  explicit MockRenderWidgetHostView(RenderWidgetHost* rwh)
      : TestRenderWidgetHostView(rwh) {}
  ~MockRenderWidgetHostView() override = default;

  void UpdateTooltipFromKeyboard(const std::u16string& tooltip_text,
                                 const gfx::Rect& bounds) override {
    last_tooltip_text_ = tooltip_text;
    last_tooltip_bounds_ = bounds;
  }

  void ClearKeyboardTriggeredTooltip() override {
    last_tooltip_text_.clear();
    last_tooltip_bounds_ = gfx::Rect();
    tooltip_cleared_ = true;
  }

  const viz::FrameSinkId& GetFrameSinkId() const override {
    return frame_sink_id_;
  }
  viz::FrameSinkId GetRootFrameSinkId() override { return frame_sink_id_; }

  const std::u16string& last_tooltip_text() const { return last_tooltip_text_; }
  const gfx::Rect& last_tooltip_bounds() const { return last_tooltip_bounds_; }
  bool tooltip_cleared() const { return tooltip_cleared_; }
  void reset_tooltip_cleared() { tooltip_cleared_ = false; }

 private:
  viz::FrameSinkId frame_sink_id_{999, 999};
  std::u16string last_tooltip_text_;
  gfx::Rect last_tooltip_bounds_;
  bool tooltip_cleared_ = false;
};

class RenderWidgetHostViewChildFrameTest
    : public RenderViewHostImplTestHarness {
 public:
  RenderWidgetHostViewChildFrameTest() {}

  void SetUp() override {
    RenderViewHostImplTestHarness::SetUp();

    process_host_ = std::make_unique<MockRenderProcessHost>(browser_context());
    site_instance_group_ =
        base::WrapRefCounted(SiteInstanceGroup::CreateForTesting(
            browser_context(), process_host_.get()));
    int32_t routing_id = process_host_->GetNextRoutingID();

    // Create a RenderWidgetHostImpl which will be associated with an
    // RenderWidgetHostViewChildFrame, to simulate what would be done for an
    // OOPIF.
    widget_host_ = RenderWidgetHostFactory::Create(
        /*frame_tree=*/&contents()->GetPrimaryFrameTree(), &delegate_,
        RenderWidgetHostImpl::DefaultFrameSinkId(*site_instance_group_,
                                                 routing_id),
        site_instance_group_->GetSafeRef(), routing_id,
        /*hidden=*/false, /*renderer_initiated_creation=*/false);

    widget_host_->BindWidgetInterfaces(
        mojo::AssociatedRemote<blink::mojom::WidgetHost>()
            .BindNewEndpointAndPassDedicatedReceiver(),
        widget_.GetNewRemote());
    widget_host_->BindFrameWidgetInterfaces(
        mojo::AssociatedRemote<blink::mojom::FrameWidgetHost>()
            .BindNewEndpointAndPassDedicatedReceiver(),
        TestRenderWidgetHost::CreateStubFrameWidgetRemote());

    display::ScreenInfo screen_info;
    screen_info.rect = gfx::Rect(1, 2, 3, 4);
    display::ScreenInfos screen_infos(screen_info);
    view_ = RenderWidgetHostViewChildFrame::Create(widget_host_.get(),
                                                   screen_infos);
    // Set MockChildFrameInputHelper as `input_helper_` member variable.
    view_->SetInputHelperForTesting(
        std::make_unique<MockChildFrameInputHelper>(view_, nullptr));

    // Test we get the expected ScreenInfo before the FrameDelegate is set.
    EXPECT_EQ(screen_info, view_->GetScreenInfo());
    EXPECT_EQ(screen_infos, view_->GetScreenInfos());

    test_frame_connector_ = std::make_unique<MockFrameConnector>();
    test_frame_connector_->SetView(view_, false);
    view_->SetFrameConnector(test_frame_connector_.get());
  }

  void TearDown() override {
    if (view_) {
      RenderWidgetHostViewChildFrame* local_view = view_;
      view_ = nullptr;
      local_view->Destroy();
    }
    widget_host_.reset();
    site_instance_group_.reset();
    process_host_->Cleanup();
    test_frame_connector_.reset();

    process_host_.reset();

    RenderViewHostImplTestHarness::TearDown();
  }

  viz::SurfaceId GetSurfaceId() const {
    return view_->last_activated_surface_info_.id();
  }

  viz::LocalSurfaceId GetLocalSurfaceId() const {
    return GetSurfaceId().local_surface_id();
  }

  MockChildFrameInputHelper* GetMockInputHelper() {
    return static_cast<MockChildFrameInputHelper*>(view_->input_helper_.get());
  }

 protected:
  std::unique_ptr<MockRenderProcessHost> process_host_;
  scoped_refptr<SiteInstanceGroup> site_instance_group_;
  MockRenderWidgetHostDelegate delegate_;
  MockWidget widget_;

  // Tests should set these to NULL if they've already triggered their
  // destruction.
  std::unique_ptr<RenderWidgetHostImpl> widget_host_;
  raw_ptr<RenderWidgetHostViewChildFrame> view_ = nullptr;
  std::unique_ptr<MockFrameConnector> test_frame_connector_;
};

TEST_F(RenderWidgetHostViewChildFrameTest, VisibilityTest) {
  // Calling show and hide also needs to be propagated to child frame by the
  // |frame_connector_| which itself requires a |frame_proxy_in_parent_renderer|
  // (set to nullptr for MockFrameConnector). To avoid crashing the test
  // |frame_connector_| is to set to nullptr.
  view_->SetFrameConnector(nullptr);

  view_->Show();
  ASSERT_TRUE(view_->IsShowing());

  view_->Hide();
  ASSERT_FALSE(view_->IsShowing());

  // Restore the MockFrameConnector to avoid a crash during destruction.
  view_->SetFrameConnector(test_frame_connector_.get());
}

// Tests that the viewport intersection rect is dispatched to the RenderWidget
// whenever screen rects are updated.
TEST_F(RenderWidgetHostViewChildFrameTest, ViewportIntersectionUpdated) {
  gfx::Rect intersection_rect(5, 5, 100, 80);
  gfx::Rect main_frame_intersection(5, 10, 200, 200);
  blink::mojom::FrameOcclusionState occlusion_state =
      blink::mojom::FrameOcclusionState::kPossiblyOccluded;

  test_frame_connector_->SetViewportIntersection(
      intersection_rect, main_frame_intersection, intersection_rect,
      occlusion_state);

  MockRenderProcessHost* process =
      static_cast<MockRenderProcessHost*>(widget_host_->GetProcess());
  process->Init();

  mojo::AssociatedRemote<blink::mojom::FrameWidgetHost> blink_frame_widget_host;
  auto blink_frame_widget_host_receiver =
      blink_frame_widget_host.BindNewEndpointAndPassDedicatedReceiver();
  mojo::AssociatedRemote<blink::mojom::FrameWidget> blink_frame_widget;
  auto blink_frame_widget_receiver =
      blink_frame_widget.BindNewEndpointAndPassDedicatedReceiver();
  widget_host_->BindFrameWidgetInterfaces(
      std::move(blink_frame_widget_host_receiver), blink_frame_widget.Unbind());
  FakeFrameWidget fake_frame_widget(std::move(blink_frame_widget_receiver));

  widget_host_->RendererWidgetCreated(/*for_frame_widget=*/true);
  base::RunLoop().RunUntilIdle();
  widget_.ClearScreenRects();
  base::RunLoop().RunUntilIdle();

  auto& intersection_state = fake_frame_widget.GetIntersectionState();
  EXPECT_EQ(gfx::Rect(intersection_rect),
            intersection_state->viewport_intersection);
  EXPECT_EQ(gfx::Rect(main_frame_intersection),
            intersection_state->main_frame_intersection);
  EXPECT_EQ(gfx::Rect(intersection_rect),
            intersection_state->compositor_visible_rect);
  EXPECT_EQ(static_cast<blink::mojom::FrameOcclusionState>(occlusion_state),
            intersection_state->occlusion_state);
}

// Tests that moving the child around does not affect the physical backing size.
TEST_F(RenderWidgetHostViewChildFrameTest, CompositorViewportPixelSize) {
  display::ScreenInfo screen_info;
  screen_info.device_scale_factor = 2.0f;

  blink::FrameVisualProperties visual_properties;
  visual_properties.screen_infos = display::ScreenInfos(screen_info);
  test_frame_connector_->SynchronizeVisualProperties(visual_properties, false);

  gfx::Size local_frame_size(1276, 410);
  test_frame_connector_->SetLocalFrameSize(local_frame_size);
  EXPECT_EQ(local_frame_size, view_->GetCompositorViewportPixelSize());

  gfx::Rect rect_in_parent_view(local_frame_size);
  rect_in_parent_view.set_origin(gfx::Point(230, 263));
  test_frame_connector_->SetRectInParentView(rect_in_parent_view);
  EXPECT_EQ(local_frame_size, view_->GetCompositorViewportPixelSize());
  EXPECT_EQ(gfx::Point(115, 131), view_->GetViewBounds().origin());
}

// Tests that SynchronizeVisualProperties is called only once and all the
// parameters change atomically.
TEST_F(RenderWidgetHostViewChildFrameTest,
       SynchronizeVisualPropertiesOncePerChange) {
  MockRenderProcessHost* process =
      static_cast<MockRenderProcessHost*>(widget_host_->GetProcess());
  process->Init();

  widget_host_->RendererWidgetCreated(/*for_frame_widget=*/true);

  constexpr gfx::Rect compositor_viewport_pixel_rect(100, 100);
  constexpr gfx::Rect rect_in_local_root(compositor_viewport_pixel_rect);
  viz::ParentLocalSurfaceIdAllocator allocator;
  allocator.GenerateId();
  viz::LocalSurfaceId local_surface_id = allocator.GetCurrentLocalSurfaceId();

  blink::FrameVisualProperties visual_properties;
  visual_properties.screen_infos = display::ScreenInfos(display::ScreenInfo());
  visual_properties.rect_in_local_root = rect_in_local_root;
  visual_properties.compositor_viewport = compositor_viewport_pixel_rect;
  visual_properties.local_frame_size = compositor_viewport_pixel_rect.size();
  visual_properties.capture_sequence_number = 123u;
  visual_properties.local_surface_id = local_surface_id;
  visual_properties.root_widget_viewport_segments.emplace_back(1, 2, 3, 4);

  base::RunLoop().RunUntilIdle();
  widget_.ClearVisualProperties();
  test_frame_connector_->SynchronizeVisualProperties(visual_properties);

  // Update to the renderer.
  base::RunLoop().RunUntilIdle();
  ASSERT_EQ(1u, widget_.ReceivedVisualProperties().size());
  {
    blink::VisualProperties sent_visual_properties =
        widget_.ReceivedVisualProperties().at(0);

    EXPECT_EQ(compositor_viewport_pixel_rect,
              sent_visual_properties.compositor_viewport_pixel_rect);
    EXPECT_EQ(rect_in_local_root.size(),
              sent_visual_properties.new_size_device_px);
    EXPECT_EQ(local_surface_id, sent_visual_properties.local_surface_id);
    EXPECT_EQ(123u, sent_visual_properties.capture_sequence_number);
    EXPECT_EQ(1u, sent_visual_properties.root_widget_viewport_segments.size());
    EXPECT_EQ(gfx::Rect(1, 2, 3, 4),
              sent_visual_properties.root_widget_viewport_segments[0]);
  }
}

// Test that when we have a gesture scroll sequence that is not consumed by the
// child, the events are bubbled so that the parent may consume them.
TEST_F(RenderWidgetHostViewChildFrameTest, UncomsumedGestureScrollBubbled) {
  blink::WebGestureEvent scroll_begin =
      blink::SyntheticWebGestureEventBuilder::BuildScrollBegin(
          0.f, 10.f, blink::WebGestureDevice::kTouchscreen);
  blink::WebGestureEvent scroll_update =
      blink::SyntheticWebGestureEventBuilder::BuildScrollUpdate(
          0.f, 10.f, 0, blink::WebGestureDevice::kTouchscreen);
  blink::WebGestureEvent scroll_end =
      blink::SyntheticWebGestureEventBuilder::Build(
          blink::WebInputEvent::Type::kGestureScrollEnd,
          blink::WebGestureDevice::kTouchscreen);

  view_->GestureEventAck(
      scroll_begin, blink::mojom::InputEventResultSource::kCompositorThread,
      blink::mojom::InputEventResultState::kNoConsumerExists);
  EXPECT_EQ(blink::WebInputEvent::Type::kGestureScrollBegin,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());
  view_->GestureEventAck(
      scroll_update, blink::mojom::InputEventResultSource::kCompositorThread,
      blink::mojom::InputEventResultState::kNoConsumerExists);
  EXPECT_EQ(blink::WebInputEvent::Type::kGestureScrollUpdate,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());
  view_->GestureEventAck(scroll_end,
                         blink::mojom::InputEventResultSource::kBrowser,
                         blink::mojom::InputEventResultState::kIgnored);
  EXPECT_EQ(blink::WebInputEvent::Type::kGestureScrollEnd,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());
}

// Test that when we have a gesture scroll sequence that is consumed by the
// child, the events are not bubbled to the parent.
TEST_F(RenderWidgetHostViewChildFrameTest, ConsumedGestureScrollNotBubbled) {
  blink::WebGestureEvent scroll_begin =
      blink::SyntheticWebGestureEventBuilder::BuildScrollBegin(
          0.f, 10.f, blink::WebGestureDevice::kTouchscreen);
  blink::WebGestureEvent scroll_update =
      blink::SyntheticWebGestureEventBuilder::BuildScrollUpdate(
          0.f, 10.f, 0, blink::WebGestureDevice::kTouchscreen);
  blink::WebGestureEvent scroll_end =
      blink::SyntheticWebGestureEventBuilder::Build(
          blink::WebInputEvent::Type::kGestureScrollEnd,
          blink::WebGestureDevice::kTouchscreen);

  view_->GestureEventAck(
      scroll_begin, blink::mojom::InputEventResultSource::kCompositorThread,
      blink::mojom::InputEventResultState::kConsumed);
  EXPECT_EQ(blink::WebInputEvent::Type::kUndefined,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());
  view_->GestureEventAck(
      scroll_update, blink::mojom::InputEventResultSource::kCompositorThread,
      blink::mojom::InputEventResultState::kConsumed);
  EXPECT_EQ(blink::WebInputEvent::Type::kUndefined,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());

  // Scrolling in a child my reach its extent and no longer be consumed, however
  // scrolling is latched to the child so we do not bubble the update.
  view_->GestureEventAck(
      scroll_update, blink::mojom::InputEventResultSource::kCompositorThread,
      blink::mojom::InputEventResultState::kNoConsumerExists);
  EXPECT_EQ(blink::WebInputEvent::Type::kUndefined,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());

  view_->GestureEventAck(scroll_end,
                         blink::mojom::InputEventResultSource::kBrowser,
                         blink::mojom::InputEventResultState::kIgnored);
  EXPECT_EQ(blink::WebInputEvent::Type::kUndefined,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());
}

// Test that the child does not continue to attempt to bubble scroll events if
// bubbling has failed for the current scroll gesture.
TEST_F(RenderWidgetHostViewChildFrameTest,
       DoNotBubbleRemainingEventsOfRejectedScrollGesture) {
  blink::WebGestureEvent scroll_begin =
      blink::SyntheticWebGestureEventBuilder::BuildScrollBegin(
          0.f, 10.f, blink::WebGestureDevice::kTouchscreen);
  blink::WebGestureEvent scroll_update =
      blink::SyntheticWebGestureEventBuilder::BuildScrollUpdate(
          0.f, 10.f, 0, blink::WebGestureDevice::kTouchscreen);
  blink::WebGestureEvent scroll_end =
      blink::SyntheticWebGestureEventBuilder::Build(
          blink::WebInputEvent::Type::kGestureScrollEnd,
          blink::WebGestureDevice::kTouchscreen);

  GetMockInputHelper()->SetCanBubble(false);

  view_->GestureEventAck(
      scroll_begin, blink::mojom::InputEventResultSource::kCompositorThread,
      blink::mojom::InputEventResultState::kNoConsumerExists);
  EXPECT_EQ(blink::WebInputEvent::Type::kGestureScrollBegin,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());

  // The GSB was rejected, so the child view must not attempt to bubble the
  // remaining events of the scroll sequence.
  view_->GestureEventAck(
      scroll_update, blink::mojom::InputEventResultSource::kCompositorThread,
      blink::mojom::InputEventResultState::kNoConsumerExists);
  EXPECT_EQ(blink::WebInputEvent::Type::kUndefined,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());
  view_->GestureEventAck(scroll_end,
                         blink::mojom::InputEventResultSource::kBrowser,
                         blink::mojom::InputEventResultState::kIgnored);
  EXPECT_EQ(blink::WebInputEvent::Type::kUndefined,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());

  GetMockInputHelper()->SetCanBubble(true);

  // When we have a new scroll gesture, the view may try bubbling again.
  view_->GestureEventAck(
      scroll_begin, blink::mojom::InputEventResultSource::kCompositorThread,
      blink::mojom::InputEventResultState::kNoConsumerExists);
  EXPECT_EQ(blink::WebInputEvent::Type::kGestureScrollBegin,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());
  view_->GestureEventAck(
      scroll_update, blink::mojom::InputEventResultSource::kCompositorThread,
      blink::mojom::InputEventResultState::kNoConsumerExists);
  EXPECT_EQ(blink::WebInputEvent::Type::kGestureScrollUpdate,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());
  view_->GestureEventAck(scroll_end,
                         blink::mojom::InputEventResultSource::kBrowser,
                         blink::mojom::InputEventResultState::kIgnored);
  EXPECT_EQ(blink::WebInputEvent::Type::kGestureScrollEnd,
            GetMockInputHelper()->GetAndResetLastBubbledEventType());
}

TEST_F(RenderWidgetHostViewChildFrameTest,
       TooltipBoundsClampedToViewportIntersection) {
  auto root_view =
      std::make_unique<testing::NiceMock<MockRenderWidgetHostView>>(
          widget_host_.get());
  RenderWidgetHostViewChildFrame* child_view =
      RenderWidgetHostViewChildFrame::Create(widget_host_.get(),
                                             display::ScreenInfos());
  std::unique_ptr<MockFrameConnector> connector =
      std::make_unique<MockFrameConnector>();

  connector->SetRootRenderWidgetHostView(root_view.get());
  connector->SetView(child_view, false);

  // Set child frame visible viewport intersection to [0, 0, 200, 200].
  connector->SetViewportIntersection(
      gfx::Rect(0, 0, 200, 200), gfx::Rect(0, 0, 200, 200),
      gfx::Rect(0, 0, 200, 200),
      blink::mojom::FrameOcclusionState::kGuaranteedNotOccluded);
  connector->SetRectInParentView(gfx::Rect(0, 0, 200, 200));
  connector->SetLocalFrameSize(gfx::Size(200, 200));
  child_view->SetSize(gfx::Size(200, 200));

  // 0. If hit-test data / transform to root view is unavailable, fail closed
  // and clear the tooltip rather than using untransformed child coordinates.
  root_view->reset_tooltip_cleared();
  child_view->UpdateTooltipFromKeyboard(u"sample_no_transform",
                                        gfx::Rect(10, 20, 30, 40));
  EXPECT_TRUE(root_view->last_tooltip_text().empty());
  EXPECT_TRUE(root_view->tooltip_cleared());

  // Populate hit-test data so GetTransformToViewCoordSpace() succeeds.
  std::vector<viz::AggregatedHitTestRegion> regions = {
      {root_view->GetFrameSinkId(), viz::HitTestRegionFlags::kHitTestMine,
       gfx::Rect(0, 0, 400, 400), gfx::Transform(),
       /*child_count=*/1},
      {child_view->GetFrameSinkId(), viz::HitTestRegionFlags::kHitTestMine,
       gfx::Rect(0, 0, 400, 400), gfx::Transform(),
       /*child_count=*/0},
  };
  viz::DisplayHitTestQueryMap hit_test_map;
  auto query = std::make_unique<viz::HitTestQuery>(std::nullopt);
  query->OnAggregatedHitTestRegionListUpdated(regions);
  hit_test_map[root_view->GetFrameSinkId()] = std::move(query);
  viz::HostFrameSinkManagerTestApi(GetHostFrameSinkManager())
      .SetDisplayHitTestQuery(std::move(hit_test_map));

  // 1. Elements with valid bounds within the viewport intersection should have
  // their tooltips and bounds forwarded to the root view.
  child_view->UpdateTooltipFromKeyboard(u"sample_tooltip",
                                        gfx::Rect(10, 20, 30, 40));
  EXPECT_EQ(u"sample_tooltip", root_view->last_tooltip_text());
  EXPECT_EQ(gfx::Rect(10, 20, 30, 40), root_view->last_tooltip_bounds());

  // 2. Elements with bounds extending partially outside the viewport
  // intersection should have their bounds clamped to the intersection.
  child_view->UpdateTooltipFromKeyboard(u"sample_tooltip_2",
                                        gfx::Rect(150, 150, 100, 100));
  EXPECT_EQ(u"sample_tooltip_2", root_view->last_tooltip_text());
  EXPECT_EQ(gfx::Rect(150, 150, 50, 50), root_view->last_tooltip_bounds());

  // 3. Elements with bounds completely outside the viewport intersection
  // should not show a tooltip.
  root_view->reset_tooltip_cleared();
  child_view->UpdateTooltipFromKeyboard(u"sample_tooltip_3",
                                        gfx::Rect(-350, -530, 400, 300));
  EXPECT_TRUE(root_view->last_tooltip_text().empty());
  EXPECT_TRUE(root_view->last_tooltip_bounds().IsEmpty());
  EXPECT_TRUE(root_view->tooltip_cleared());

  // 4. If the viewport intersection is empty (e.g. frame is scrolled out of
  // view), no tooltip should be shown.
  connector->SetViewportIntersection(
      gfx::Rect(), gfx::Rect(), gfx::Rect(),
      blink::mojom::FrameOcclusionState::kGuaranteedNotOccluded);
  root_view->reset_tooltip_cleared();
  child_view->UpdateTooltipFromKeyboard(u"sample_tooltip_4",
                                        gfx::Rect(10, 20, 30, 40));
  EXPECT_TRUE(root_view->last_tooltip_text().empty());
  EXPECT_TRUE(root_view->last_tooltip_bounds().IsEmpty());

  // 5. Passing empty tooltip text should clear any active tooltip.
  connector->SetViewportIntersection(
      gfx::Rect(0, 0, 200, 200), gfx::Rect(0, 0, 200, 200),
      gfx::Rect(0, 0, 200, 200),
      blink::mojom::FrameOcclusionState::kGuaranteedNotOccluded);
  root_view->reset_tooltip_cleared();
  child_view->UpdateTooltipFromKeyboard(std::u16string(),
                                        gfx::Rect(10, 20, 30, 40));
  EXPECT_TRUE(root_view->last_tooltip_text().empty());
  EXPECT_TRUE(root_view->tooltip_cleared());

  // 6. With DSF = 2.0, viewport_intersection is in physical pixels ([0, 0, 400,
  // 400] for a 200x200 DIP frame), while bounds is in DIPs. Ensure clamping
  // converts viewport_intersection to DIPs ([0, 0, 200, 200]).
  display::ScreenInfo screen_info;
  screen_info.device_scale_factor = 2.0f;
  blink::FrameVisualProperties visual_properties;
  visual_properties.screen_infos = display::ScreenInfos(screen_info);
  visual_properties.local_frame_size = gfx::Size(400, 400);
  connector->SynchronizeVisualProperties(visual_properties, false);
  connector->SetViewportIntersection(
      gfx::Rect(0, 0, 400, 400), gfx::Rect(0, 0, 400, 400),
      gfx::Rect(0, 0, 400, 400),
      blink::mojom::FrameOcclusionState::kGuaranteedNotOccluded);
  child_view->UpdateTooltipFromKeyboard(u"sample_tooltip_dsf2",
                                        gfx::Rect(150, 150, 100, 100));
  EXPECT_EQ(u"sample_tooltip_dsf2", root_view->last_tooltip_text());
  EXPECT_EQ(gfx::Rect(150, 150, 50, 50), root_view->last_tooltip_bounds());

  // 7. With DSF = 0.5, viewport_intersection is [0, 0, 100, 100] in physical
  // pixels for a 200x200 DIP frame. Legitimate tooltips in [100..200] DIPs
  // must not be clipped or dropped.
  screen_info.device_scale_factor = 0.5f;
  visual_properties.screen_infos = display::ScreenInfos(screen_info);
  visual_properties.local_frame_size = gfx::Size(100, 100);
  connector->SynchronizeVisualProperties(visual_properties, false);
  connector->SetViewportIntersection(
      gfx::Rect(0, 0, 100, 100), gfx::Rect(0, 0, 100, 100),
      gfx::Rect(0, 0, 100, 100),
      blink::mojom::FrameOcclusionState::kGuaranteedNotOccluded);
  child_view->UpdateTooltipFromKeyboard(u"sample_tooltip_dsf_half",
                                        gfx::Rect(120, 120, 30, 40));
  EXPECT_EQ(u"sample_tooltip_dsf_half", root_view->last_tooltip_text());
  EXPECT_EQ(gfx::Rect(120, 120, 30, 40), root_view->last_tooltip_bounds());

  viz::HostFrameSinkManagerTestApi(GetHostFrameSinkManager())
      .SetDisplayHitTestQuery(viz::DisplayHitTestQueryMap());
  child_view->Destroy();
  connector->SetRootRenderWidgetHostView(nullptr);
}

}  // namespace content
