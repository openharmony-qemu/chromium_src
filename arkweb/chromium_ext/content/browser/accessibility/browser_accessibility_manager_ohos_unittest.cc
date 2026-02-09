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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ui/accessibility/ax_tree.h" 
#include "ui/accessibility/ax_node.h" 
#include "ui/accessibility/ax_tree_update.h" 
#include "ui/accessibility/ax_node_data.h" 
#include "ui/accessibility/ax_tree_observer.h" 
#include "ui/accessibility/platform/ax_platform_node_id.h" 
#include "ui/accessibility/platform/provide_ax_platform_for_tests.h"
#include "ui/accessibility/platform/test_ax_node_id_delegate.h"
#include "ui/accessibility/platform/test_ax_platform_tree_manager_delegate.h"
#include "ohos_sdk/openharmony/native/llvm/bin/../include/libcxx-ohos/include/c++/v1/__ranges/lazy_split_view.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/single_thread_task_runner.h"
#define private public
#include "content/browser/accessibility/browser_accessibility_manager_ohos.h"
#include "content/browser/accessibility/browser_accessibility_ohos.h"
#undef private

using namespace testing;
using namespace ui;
using namespace content;

constexpr int64_t kInvalidAccessibilityId = -1;
constexpr int64_t kArkWebId = 0;
constexpr int32_t kMaxContentChangedEventsToFire = 5;

class AccessibilityEventDispatcherTest  : public testing::Test {
 protected:
  void SetUp() override {
    ui::AXNodeData root;
  root.id = 8700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {8701};

  ui::AXNodeData b1;
  b1.id = 8701;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  mock_delegate_ = std::make_unique<ui::TestAXPlatformTreeManagerDelegate>();
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
          update, mock_node_id_delegate_, mock_delegate_.get());
  }

    BrowserAccessibilityManagerOHOS* manager_ = nullptr;
  ui::TestAXNodeIdDelegate mock_node_id_delegate_;
  std::unique_ptr<TestAXPlatformTreeManagerDelegate> mock_delegate_;
};

TEST_F(AccessibilityEventDispatcherTest, EnqueueEvent001) {
  BrowserAccessibilityManagerOHOS* manager = nullptr;
  std::unordered_map<int32_t, int32_t> delays = {{0, 1}, {2, 3}};
  std::unordered_set<int32_t> events = {1};
  AccessibilityEventDispatcher dispatcher(delays, events, manager);

  dispatcher.EnqueueEvent(0, 1, "");
  EXPECT_EQ(dispatcher.manager_, nullptr);
}

TEST_F(AccessibilityEventDispatcherTest, EnqueueEvent002) {
  std::unordered_map<int32_t, int32_t> delays = {{0, 1}, {2, 3}};
  std::unordered_set<int32_t> events = {1};
  AccessibilityEventDispatcher dispatcher(delays, events, manager_);
  dispatcher.manager_ = manager_;

  ASSERT_NO_FATAL_FAILURE(dispatcher.EnqueueEvent(0, 1, ""));
}

TEST_F(AccessibilityEventDispatcherTest, EnqueueEvent003) {
  std::unordered_map<int32_t, int32_t> delays = {{0, 0}, {1, 1}};
  std::unordered_set<int32_t> events = {1};
  AccessibilityEventDispatcher dispatcher(delays, events, manager_);
  dispatcher.manager_ = manager_;
  dispatcher.eventLastFiredTimes_.insert({0, 10});
  dispatcher.eventLastFiredTimes_.insert({1, 11});

  ASSERT_NO_FATAL_FAILURE(dispatcher.EnqueueEvent(0, 1, ""));
}

TEST_F(AccessibilityEventDispatcherTest, EnqueueEvent004) {
  std::unordered_map<int32_t, int32_t> delays = {{2, 200}};
  std::unordered_set<int32_t> events = {2};
  AccessibilityEventDispatcher dispatcher(delays, events, manager_);
  dispatcher.manager_ = manager_;

  int64_t uuid = 2;
  auto millis = std::chrono::time_point_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now());
  auto now = millis.time_since_epoch().count();
  dispatcher.eventLastFiredTimes_[uuid] = now - 300;

  ASSERT_NO_FATAL_FAILURE(dispatcher.EnqueueEvent(0, 2, ""));
  EXPECT_EQ(dispatcher.pendingEvents_.size(), 0);
  EXPECT_TRUE(dispatcher.eventLastFiredTimes_.find(uuid) != dispatcher.eventLastFiredTimes_.end());
}

TEST_F(AccessibilityEventDispatcherTest, EnqueueEvent005) {
  std::unordered_map<int32_t, int32_t> delays = {{4, 200}};
  std::unordered_set<int32_t> events;
  AccessibilityEventDispatcher dispatcher(delays, events, manager_);
  dispatcher.manager_ = manager_;

  int64_t uuid = 4;
  auto millis = std::chrono::time_point_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now());
  auto now = millis.time_since_epoch().count();
  dispatcher.eventLastFiredTimes_[uuid] = now - 100;

  size_t events_before = dispatcher.pendingEvents_.size();
  ASSERT_NO_FATAL_FAILURE(dispatcher.EnqueueEvent(0, 4, ""));
  EXPECT_EQ(dispatcher.pendingEvents_.size(), events_before);
}

TEST_F(AccessibilityEventDispatcherTest, RunTask001) {
  BrowserAccessibilityManagerOHOS* manager = nullptr;
  std::unordered_map<int32_t, int32_t> delays = {{0, 1}, {2, 3}};
  std::unordered_set<int32_t> events = {1};
  AccessibilityEventDispatcher dispatcher(delays, events, manager);

  dispatcher.pendingEvents_.insert({0, base::DelayedTaskHandle()});
  dispatcher.RunTask(0, 1, 0, "");
  EXPECT_EQ(dispatcher.pendingEvents_.size(), 0);
}

TEST_F(AccessibilityEventDispatcherTest, RunTask002) {
  std::unordered_map<int32_t, int32_t> delays = {{0, 0}, {1, 1}};
  std::unordered_set<int32_t> events = {1};
  AccessibilityEventDispatcher dispatcher(delays, events, manager_);
  dispatcher.manager_ = manager_;

  dispatcher.RunTask(0, 1, 0, "");
  EXPECT_EQ(dispatcher.pendingEvents_.size(), 0);
}

TEST_F(AccessibilityEventDispatcherTest, Uuid001) {
  std::unordered_map<int32_t, int32_t> delays = {{0, 0}, {1, 1}};
  std::unordered_set<int32_t> events = {1};
  AccessibilityEventDispatcher dispatcher(delays, events, manager_);

  dispatcher.viewIndependentEvents_.insert(1);
  EXPECT_EQ(dispatcher.Uuid(1, 2), 4294967298);
}

TEST_F(AccessibilityEventDispatcherTest, Uuid002) {
  std::unordered_map<int32_t, int32_t> delays = {{0, 0}, {1, 1}};
  std::unordered_set<int32_t> events = {1, 2};
  AccessibilityEventDispatcher dispatcher(delays, events, manager_);
}

TEST_F(AccessibilityEventDispatcherTest, Uuid003) {
  std::unordered_map<int32_t, int32_t> delays = {{0, 0}, {1, 1}};
  std::unordered_set<int32_t> events = {1, -2};
  AccessibilityEventDispatcher dispatcher(delays, events, manager_);

  dispatcher.viewIndependentEvents_.insert(1);
  EXPECT_EQ(dispatcher.Uuid(-1, 2), 0);
}

TEST_F(AccessibilityEventDispatcherTest, Uuid004) {
  std::unordered_map<int32_t, int32_t> delays = {{0, 0}, {1, 1}};
  std::unordered_set<int32_t> events = {1, 2};

  AccessibilityEventDispatcher dispatcher(delays, events, manager_);

  dispatcher.viewIndependentEvents_.insert(2);
  EXPECT_EQ(dispatcher.Uuid(1, -2), 0);
}

class BrowserAccessibilityManagerOhosTest : public ::testing::Test {
 public:
  void SetUp() override {
    test_browser_accessibility_delegate_ = std::make_unique<ui::TestAXPlatformTreeManagerDelegate>();
    static ui::ProvideAXPlatformForTests provide_ax_platform_for_tests;
  }
  void TearDown() override {
    test_browser_accessibility_delegate_.reset();
  }

  std::unique_ptr<ui::TestAXPlatformTreeManagerDelegate> test_browser_accessibility_delegate_;
  ui::TestAXNodeIdDelegate node_id_delegate_;
};
TEST_F(BrowserAccessibilityManagerOhosTest, FindElementType_CustomPredicate) {
  ui::AXNodeData root;
  root.id = 4000;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {4001, 4002};

  ui::AXNodeData b1;
  b1.id = 4001;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);
  
  ui::AXNodeData b2;
  b2.id = 4002;
  b2.role = ax::mojom::Role::kButton;
  b2.relative_bounds.bounds = gfx::RectF(100, 0, 50, 50);
  
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1, b2);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = 
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
          update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  
  int64_t result = manager->FindElementType(4001, "button", true, true, false);
  
  EXPECT_EQ(result, 4002);
}

TEST_F(BrowserAccessibilityManagerOhosTest, FindElementType001) {  
  ui::AXNodeData root;
  root.id = 4100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {4101};

  ui::AXNodeData b1;
  b1.id = 4101;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);
  
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  EXPECT_EQ(manager->FindElementType(1, "123", true, true, true), 0);
}

TEST_F(BrowserAccessibilityManagerOhosTest, HandleNavigate_ValidNewRootResetsFocus) {  
  ui::AXNodeData root;
  root.id = 4300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {4301};

  ui::AXNodeData b1;
  b1.id = 4301;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);
  
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  const int64_t initial_focus = 100;
  manager->accessibilityFocusId_ = initial_focus;
  ASSERT_EQ(manager->accessibilityFocusId_, initial_focus);
  manager->HandleNavigate(123);
  EXPECT_EQ(manager->accessibilityFocusId_, kInvalidAccessibilityId);
  manager->accessibilityFocusId_ = 0;
}

TEST_F(BrowserAccessibilityManagerOhosTest, HandleNavigate_NewRootResetsFocus) {
      
  ui::AXNodeData root;
  root.id = 4400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {4401};

  ui::AXNodeData b1;
  b1.id = 4401;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);
  
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  const int64_t initial_focus = 100;
  manager->accessibilityFocusId_ = initial_focus;
  ASSERT_EQ(manager->accessibilityFocusId_, initial_focus);
  manager->HandleNavigate(kInvalidAccessibilityId);
  EXPECT_EQ(manager->accessibilityFocusId_, initial_focus);
  manager->accessibilityFocusId_ = 0;
}

TEST_F(BrowserAccessibilityManagerOhosTest, HandleFocusChanged) {
  ui::AXNodeData root;
  root.id = 4500;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {4501};

  ui::AXNodeData b1;
  b1.id = 4501;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);
  
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  int64_t accessibilityFocusId= manager->GetRootAccessibilityId();
  ASSERT_NO_FATAL_FAILURE(manager->HandleFocusChanged(accessibilityFocusId));
  ASSERT_NO_FATAL_FAILURE(manager->HandleFocusChanged(4501));
  manager->accessibilityFocusId_ = 4501;
  ASSERT_NO_FATAL_FAILURE(manager->HandleFocusChanged(4501));
}

TEST_F(BrowserAccessibilityManagerOhosTest, DispatchEvent) {
  const int32_t kEventType = 1;
  const std::string kArgument = "test_argument";
  
  ui::AXNodeData root;
  root.id = 4600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {4601};

  ui::AXNodeData b1;
  b1.id = 4601;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);
  
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  EXPECT_FALSE((manager->DispatchEvent(4601,kEventType,kArgument)));
  EXPECT_FALSE((manager->DispatchEvent(kArkWebId,kEventType,kArgument)));
  EXPECT_FALSE((manager->DispatchEvent(999,kEventType,kArgument)));
}

TEST_F(BrowserAccessibilityManagerOhosTest, DispatchEvent_002) {
  ui::AXNodeData root;
  root.id = 6700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {6701};

  ui::AXNodeData b1;
  b1.id = 6701;
  b1.role = ax::mojom::Role::kButton;
  
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  bool result = manager->DispatchEvent(6701, 1, "test");
  EXPECT_FALSE(result);
}

TEST_F(BrowserAccessibilityManagerOhosTest, MoveAccessibilityFocusToId) {

  ui::AXNodeData root;
  root.id = 4700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {4701, 4702};

  ui::AXNodeData b1;
  b1.id = 4701;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);
  
  ui::AXNodeData b2;
  b2.id = 4702;
  b2.role = ax::mojom::Role::kButton;
  b2.relative_bounds.bounds = gfx::RectF(100, 0, 50, 50);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1, b2);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  manager->HandleFocusChanged(4701);
  manager->HandleFocusChanged(4702);
  manager->accessibilityFocusId_= 4701;
  manager->eventDispatcher_ = nullptr;
  EXPECT_FALSE((manager->MoveAccessibilityFocusToId(manager->accessibilityFocusId_)));
  manager->accessibilityFocusId_= 0;
  // EXPECT_TRUE((manager->MoveAccessibilityFocusToId(4702)));
}

TEST_F(BrowserAccessibilityManagerOhosTest, FireBlinkEvent) {
  ui::AXNodeData root;
  root.id = 4800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {4801, 4802};

  ui::AXNodeData b1;
  b1.id = 4801;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);
  
  ui::AXNodeData b2;
  b2.id = 4802;
  b2.role = ax::mojom::Role::kButton;
  b2.relative_bounds.bounds = gfx::RectF(100, 0, 50, 50);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1, b2);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  ui::BrowserAccessibility* root_accessible =
      manager->GetManagerForRootFrame()->GetBrowserAccessibilityRoot();
      manager->eventDispatcher_ = nullptr;
  ASSERT_NO_FATAL_FAILURE((manager->FireBlinkEvent(ax::mojom::Event::kClicked,
                                                   root_accessible, 4801)));
  ASSERT_NO_FATAL_FAILURE((
      manager->FireBlinkEvent(ax::mojom::Event::kBlur, root_accessible, 4801)));
  ASSERT_NO_FATAL_FAILURE((manager->FireBlinkEvent(ax::mojom::Event::kHover,
                                                   root_accessible, 4801)));
  ASSERT_NO_FATAL_FAILURE((manager->FireBlinkEvent(
      ax::mojom::Event::kLayoutComplete, root_accessible, 4801)));
  ASSERT_NO_FATAL_FAILURE((
      manager->FireBlinkEvent(ax::mojom::Event::kNone, root_accessible, 4801)));
  ASSERT_NO_FATAL_FAILURE((
      manager->FireBlinkEvent(ax::mojom::Event::kNone, nullptr, 4801)));
}

TEST_F(BrowserAccessibilityManagerOhosTest, FireGeneratedEvent) {
  ui::AXNodeData root;
  root.id = 4200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4201);
  root.child_ids.push_back(4202);
  root.child_ids.push_back(4203);

  ui::AXNodeData textfield;
  textfield.id = 4201;
  textfield.role = ax::mojom::Role::kTextField;
  textfield.SetName("Test Name");
  textfield.SetValue("Test Value");
  textfield.AddStringAttribute(ax::mojom::StringAttribute::kPlaceholder, "Enter text here");

  ui::AXNodeData input;
  input.id = 4202;
  input.role = ax::mojom::Role::kTextField;
  input.AddStringAttribute(ax::mojom::StringAttribute::kPlaceholder, "Enter text here");
  input.AddStringAttribute(ax::mojom::StringAttribute::kDescription, "Input description");

  ui::AXNodeData button;
  button.id = 4203;
  button.role = ax::mojom::Role::kButton;
  button.AddStringAttribute(ax::mojom::StringAttribute::kDescription, "Button description");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, textfield, input, button);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  manager->HandleFocusChanged(4201);
  manager->HandleFocusChanged(4202);
  auto* node = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4201));
  ASSERT_NE(node, nullptr);
  auto root_accessible = node->node();
  manager->eventDispatcher_ = nullptr;
  ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
      AXEventGenerator::Event::CHILDREN_CHANGED, nullptr)));
  ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
      AXEventGenerator::Event::CHILDREN_CHANGED, root_accessible)));
  ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
      AXEventGenerator::Event::NONE, root_accessible)));
  ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
      AXEventGenerator::Event::VALUE_IN_TEXT_FIELD_CHANGED, root_accessible)));
  ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
      AXEventGenerator::Event::NAME_CHANGED, root_accessible)));
  ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
      AXEventGenerator::Event::SCROLL_HORIZONTAL_POSITION_CHANGED, root_accessible)));
  ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
      AXEventGenerator::Event::SCROLL_VERTICAL_POSITION_CHANGED, root_accessible)));
  ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
      AXEventGenerator::Event::DOCUMENT_SELECTION_CHANGED, root_accessible)));
  // ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
  //     AXEventGenerator::Event::LIVE_REGION_CHANGED, root_accessible)));
  // ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
  //     AXEventGenerator::Event::SUBTREE_CREATED, root_accessible)));
  // ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
  //     AXEventGenerator::Event::ALERT, root_accessible)));
  // ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
  //     AXEventGenerator::Event::LIVE_REGION_NODE_CHANGED, root_accessible)));
  // ASSERT_NO_FATAL_FAILURE((manager->FireGeneratedEvent(
  //     AXEventGenerator::Event::EXPANDED, root_accessible)));
}


TEST_F(BrowserAccessibilityManagerOhosTest, RetargetForEvents) {
  ui::AXNodeData root;
  root.id = 4900;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData b1;
  b1.id = 4901;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);

  root.child_ids = {4901};
  
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  auto test_browser_accessibility_delegate = std::make_unique<ui::TestAXPlatformTreeManagerDelegate>();
  ui::TestAXNodeIdDelegate node_id_delegate;
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = 
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
          update, node_id_delegate, test_browser_accessibility_delegate.get());
  auto* node = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4901));
  ASSERT_NE(node, nullptr);
  auto root_accessible = node->node();
  AXNode* result3 = manager->RetargetForEvents(
      root_accessible,  ui::AXPlatformTreeManager::RetargetEventType::RetargetEventTypeGenerated);
  EXPECT_EQ(result3->id(), 4901);
  AXNode* result1 = manager->RetargetForEvents(
      root_accessible,  ui::AXPlatformTreeManager::RetargetEventType::RetargetEventTypeBlinkGeneral);
  EXPECT_EQ(result1->id(), 4901);
}


TEST_F(BrowserAccessibilityManagerOhosTest, MoveAccessibilityFocus) {

  ui::AXNodeData root;
  root.id = 5100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {5101, 5102};

  ui::AXNodeData b1;
  b1.id = 5101;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);
  
  ui::AXNodeData b2;
  b2.id = 5102;
  b2.role = ax::mojom::Role::kButton;
  b2.relative_bounds.bounds = gfx::RectF(100, 0, 50, 50);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1, b2);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  manager->HandleFocusChanged(5101);
  ASSERT_NO_FATAL_FAILURE((manager->MoveAccessibilityFocus(5102,5102)));
}

TEST_F(BrowserAccessibilityManagerOhosTest, JumpToElementType_EmptySelector_UsesDefaultPredicate) {
  ui::AXNodeData root;
  root.id = 6100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {6101, 6102};

  ui::AXNodeData b1;
  b1.id = 6101;
  b1.role = ax::mojom::Role::kButton;

  ui::AXNodeData b2;
  b2.id = 6102;
  b2.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1, b2);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  EXPECT_TRUE(manager->JumpToElementType(6101, "", true, true));
}

TEST_F(BrowserAccessibilityManagerOhosTest, OnHoverEvent_Hit_UsesNode) {
  ui::AXNodeData root;
  root.id = 6200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {6201};

  ui::AXNodeData b1;
  b1.id = 6201;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 10, 10);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  ASSERT_NO_FATAL_FAILURE(manager->OnHoverEvent(gfx::PointF(5, 5)));
}

TEST_F(BrowserAccessibilityManagerOhosTest, SendAccessibilityEvent_ArkWebId_DirectDispatch) {
  ui::AXNodeData root;
  root.id = 6300;
  root.role = ax::mojom::Role::kRootWebArea;
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  ASSERT_NO_FATAL_FAILURE(manager->SendAccessibilityEvent(
      kArkWebId, OHOS::NWeb::AccessibilityEventType::CLICK, "x"));
}

TEST_F(BrowserAccessibilityManagerOhosTest, FireGeneratedEvent_SubtreeCreated_DialogOpens) {
  ui::AXNodeData root;
  root.id = 6400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {6401};

  ui::AXNodeData dialog;
  dialog.id = 6401;
  dialog.role = ax::mojom::Role::kDialog;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, dialog);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  ui::BrowserAccessibility* node = manager->GetFromID(6401);
  ASSERT_NE(node, nullptr);
  manager->eventDispatcher_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(manager->FireGeneratedEvent(
      AXEventGenerator::Event::SUBTREE_CREATED, node->node()));
}

TEST_F(BrowserAccessibilityManagerOhosTest, FireFocusEvent_DialogRole_NoOp) {
  ui::AXNodeData root;
  root.id = 5200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {5201};

  ui::AXNodeData dialog;
  dialog.id = 5201;
  dialog.role = ax::mojom::Role::kDialog;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, dialog);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  ui::BrowserAccessibility* node = manager->GetFromID(5201);
  ASSERT_NE(node, nullptr);
  ASSERT_NO_FATAL_FAILURE(manager->FireFocusEvent(node->node()));
}

TEST_F(BrowserAccessibilityManagerOhosTest, MoveAccessibilityFocusToId_ReturnsTrue) {
  ui::AXNodeData root;
  root.id = 5300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {5301, 5302};

  ui::AXNodeData b1;
  b1.id = 5301;
  b1.role = ax::mojom::Role::kButton;

  ui::AXNodeData b2;
  b2.id = 5302;
  b2.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1, b2);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  manager->HandleFocusChanged(5301);
  EXPECT_TRUE(manager->MoveAccessibilityFocusToId(5302));
}

TEST_F(BrowserAccessibilityManagerOhosTest, SendLocationChangeEvents_TooMany_UsesDelayedRoot) {
  ui::AXNodeData root;
  root.id = 5400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {5401};

  ui::AXNodeData b1;
  b1.id = 5401;
  b1.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  manager->eventDispatcher_ = nullptr;

  std::vector<ui::AXLocationChange> changes;
  for (int i = 0; i < kMaxContentChangedEventsToFire + 1; ++i) {
    changes.emplace_back();
  }
  ASSERT_NO_FATAL_FAILURE(manager->SendLocationChangeEvents(changes));
}

TEST_F(BrowserAccessibilityManagerOhosTest, RetargetForEvents_BlinkHover_PrefersInterestingChild) {
  ui::AXNodeData root;
  root.id = 5500;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {5501};

  ui::AXNodeData container;
  container.id = 5501;
  container.role = ax::mojom::Role::kGenericContainer;
  container.child_ids = {5502};

  ui::AXNodeData button;
  button.id = 5502;
  button.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, container, button);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  ui::BrowserAccessibility* container_ba = manager->GetFromID(5501);
  ASSERT_NE(container_ba, nullptr);
  AXNode* ret = manager->RetargetForEvents(
      container_ba->node(), ui::AXPlatformTreeManager::RetargetEventType::RetargetEventTypeBlinkHover);
  ASSERT_NE(ret, nullptr);
  EXPECT_EQ(ret->id(), 5502);
}

TEST_F(BrowserAccessibilityManagerOhosTest, OnHoverEvent_NoHit_UsesRoot) {
  ui::AXNodeData root;
  root.id = 5600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {5601};

  ui::AXNodeData b1;
  b1.id = 5601;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 10, 10);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  manager->eventDispatcher_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(manager->OnHoverEvent(gfx::PointF(10000, 10000)));
}

TEST_F(BrowserAccessibilityManagerOhosTest, FireGeneratedEvent_Alert_And_LiveRegionNodeChanged) {
  ui::AXNodeData root;
  root.id = 5700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {5701};

  ui::AXNodeData alert;
  alert.id = 5701;
  alert.role = ax::mojom::Role::kAlert;
  alert.SetName("Alert text");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, alert);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  manager->eventDispatcher_ = nullptr;
  ui::BrowserAccessibility* node = manager->GetFromID(5701);
  ASSERT_NE(node, nullptr);
  ASSERT_NO_FATAL_FAILURE(manager->FireGeneratedEvent(
      AXEventGenerator::Event::ALERT, node->node()));
  ASSERT_NO_FATAL_FAILURE(manager->FireGeneratedEvent(
      AXEventGenerator::Event::LIVE_REGION_NODE_CHANGED, node->node()));
}

TEST_F(BrowserAccessibilityManagerOhosTest, FireGeneratedEvent_Expanded_Combobox_WithFocusedDescendant) {
  ui::AXNodeData root;
  root.id = 5800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {5801};

  ui::AXNodeData combobox;
  combobox.id = 5801;
  combobox.role = ax::mojom::Role::kComboBoxMenuButton;
  combobox.child_ids = {5802};

  ui::AXNodeData option;
  option.id = 5802;
  option.role = ax::mojom::Role::kListBoxOption;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combobox, option);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  manager->eventDispatcher_ = nullptr;

  manager->HandleFocusChanged(5802);
  ui::BrowserAccessibility* combo_ba = manager->GetFromID(5801);
  ASSERT_NE(combo_ba, nullptr);
  ASSERT_NO_FATAL_FAILURE(manager->FireGeneratedEvent(
      AXEventGenerator::Event::EXPANDED, combo_ba->node()));
}

TEST_F(BrowserAccessibilityManagerOhosTest, ScrollToMakeNodeVisible_Simple) {
  ui::AXNodeData root;
  root.id = 5900;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {5901};

  ui::AXNodeData b1;
  b1.id = 5901;
  b1.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  ASSERT_NO_FATAL_FAILURE(manager->ScrollToMakeNodeVisible(5901));
}

TEST_F(BrowserAccessibilityManagerOhosTest, PredicateForSearchKey_Unknown_UsesAllInteresting) {
  ui::AXNodeData root;
  root.id = 6001;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {6002, 6003};

  ui::AXNodeData b1;
  b1.id = 6002;
  b1.role = ax::mojom::Role::kButton;

  ui::AXNodeData b2;
  b2.id = 6003;
  b2.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1, b2);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  int64_t result = manager->FindElementType(6002, "unknown_selector", true, true, false);
  EXPECT_NE(result, 0);
}

TEST_F(BrowserAccessibilityManagerOhosTest, MoveAccessibilityFocusToIdAndRefocusIfNeeded_001) {
  ui::AXNodeData root;
  root.id = 7000;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {7001, 7002};

  ui::AXNodeData b1;
  b1.id = 7001;
  b1.role = ax::mojom::Role::kButton;

  ui::AXNodeData b2;
  b2.id = 7002;
  b2.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1, b2);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  manager->eventDispatcher_ = nullptr;

  int64_t root_id = manager->GetRootAccessibilityId();
  ASSERT_NE(root_id, kInvalidAccessibilityId);

  manager->accessibilityFocusId_ = 7001;
  ASSERT_EQ(manager->accessibilityFocusId_, 7001);
  ASSERT_NO_FATAL_FAILURE(
      manager->MoveAccessibilityFocusToIdAndRefocusIfNeeded(7001));
  EXPECT_EQ(manager->accessibilityFocusId_, root_id);
}

TEST_F(BrowserAccessibilityManagerOhosTest, MoveAccessibilityFocusToIdAndRefocusIfNeeded_002) {
  ui::AXNodeData root;
  root.id = 7100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {7101, 7102};

  ui::AXNodeData b1;
  b1.id = 7101;
  b1.role = ax::mojom::Role::kButton;

  ui::AXNodeData b2;
  b2.id = 7102;
  b2.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1, b2);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  manager->eventDispatcher_ = nullptr;
  manager->accessibilityFocusId_ = 7101;
  ASSERT_EQ(manager->accessibilityFocusId_, 7101);
  ASSERT_NO_FATAL_FAILURE(
      manager->MoveAccessibilityFocusToIdAndRefocusIfNeeded(7102));
  EXPECT_EQ(manager->accessibilityFocusId_, 7102);
}

TEST_F(BrowserAccessibilityManagerOhosTest, Copy) {
  ui::AXNodeData root;
  root.id = 7200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, nullptr);
  ASSERT_NO_FATAL_FAILURE(manager->Copy());
}

TEST_F(BrowserAccessibilityManagerOhosTest, Cut) {
  ui::AXNodeData root;
  root.id = 7300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, nullptr);
  ASSERT_NO_FATAL_FAILURE(manager->Cut());
}

TEST_F(BrowserAccessibilityManagerOhosTest, Paste) {
  ui::AXNodeData root;
  root.id = 7400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManagerOHOS> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, nullptr);
  ASSERT_NO_FATAL_FAILURE(manager->Paste());
}