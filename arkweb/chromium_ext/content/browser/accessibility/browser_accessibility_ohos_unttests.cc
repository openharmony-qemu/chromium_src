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

#include <memory>

#include "base/test/scoped_feature_list.h"
#include "build/build_config.h"
#include "content/public/test/browser_task_environment.h"
#include "content/test/test_content_client.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/accessibility/platform/browser_accessibility_manager.h"
#include "ui/accessibility/platform/provide_ax_platform_for_tests.h"
#include "ui/accessibility/platform/test_ax_node_id_delegate.h"
#include "ui/accessibility/platform/test_ax_platform_tree_manager_delegate.h"
#include "ui/strings/grit/auto_image_annotation_strings.h"
#include "ui/strings/grit/ax_strings.h"

#define private public
#include "content/browser/accessibility/browser_accessibility_manager_ohos.h"
#include "content/browser/accessibility/browser_accessibility_ohos.h"
#undef private

namespace content {

class MockContentClient : public content::TestContentClient {
 public:
  std::u16string GetLocalizedString(int message_id) override {
    switch (message_id) {
      case IDS_AX_COMBOBOX_EXPANDED_DIALOG:
        return u"Dialog";
      case IDS_AX_COMBOBOX_EXPANDED_AUTOCOMPLETE_DEFAULT:
        return u"Autocomplete options";
      case IDS_AX_COMBOBOX_EXPANDED_AUTOCOMPLETE_X_OPTIONS_AVAILABLE:
        return u"{x} options available";
      case IDS_AX_COMBOBOX_EXPANDED:
        return u"Expanded";
      case IDS_AX_ROLE_HEADING:
        return u"Heading";
      case IDS_AX_STATE_LINK_VISITED:
        return u"Visited";
      case IDS_AX_ROLE_POP_UP_BUTTON_MENU:
        return u"Popup button menu";
      case IDS_AX_ROLE_POP_UP_BUTTON_DIALOG:
        return u"Popup button dialog";
      case IDS_AX_ROLE_POP_UP_BUTTON:
        return u"Popup button";
      case IDS_AX_ROLL_CANVAS_OHOS:
        return u"Canvas";
      case IDS_AX_ROLE_COMBO_BOX:
        return u"Combo box";
      case IDS_AX_ROLE_DESCRIPTION_LIST:
        return u"Description list";
      case IDS_AX_ROLE_GRAPHIC:
        return u"Graphic";
      case IDS_AX_ROLE_BANNER:
        return u"Banner";
      case IDS_AX_ROLE_TABLE:
        return u"Table";
      case IDS_AX_ROLE_MARQUEE:
        return u"Marquee";
      case IDS_AX_ROLE_CHECK_BOX:
        return u"Checkbox";
      case IDS_AX_ROLE_RADIO:
        return u"Radio";
      case IDS_AX_ROLE_TEXT_FIELD:
        return u"Text field";
      case IDS_AX_MEDIA_VIDEO_ELEMENT:
        return u"Video";
      case IDS_AX_UNLABELED_IMAGE_ROLE_DESCRIPTION:
        return u"Unlabeled image";
      case IDS_AX_IMAGE_ELIGIBLE_FOR_ANNOTATION:
        return u"This image isn't labeled. Double tap on the more options "
               u"button at the top of the browser to get image descriptions.";
      case IDS_AX_IMAGE_ANNOTATION_PENDING:
        return u"Getting description...";
      case IDS_AX_IMAGE_ANNOTATION_ADULT:
        return u"Appears to contain adult content. No description available.";
      case IDS_AX_IMAGE_ANNOTATION_NO_DESCRIPTION:
        return u"No description available.";
      default:
        return std::u16string();
    }
  }
};

class BrowserAccessibilityOHOSTest : public ::testing::Test {
 public:
  void SetUp() override {
    test_browser_accessibility_delegate_ = std::make_unique<ui::TestAXPlatformTreeManagerDelegate>();
    SetContentClient(&client_);
    static ui::ProvideAXPlatformForTests provide_ax_platform_for_tests;
  }
  void TearDown() override {}

  std::unique_ptr<ui::TestAXPlatformTreeManagerDelegate> test_browser_accessibility_delegate_;
  ui::TestAXNodeIdDelegate node_id_delegate_;
  MockContentClient client_;
};

TEST_F(BrowserAccessibilityOHOSTest, DistanceCalculationFunctionality) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(50, 50, 100, 100);

  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT)));
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::UP)));
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::DOWN)));

  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT)));

  double major_dist =
      ui::BrowserAccessibilityOHOS::MajorAxisDistance(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_EQ(major_dist, 0.0);
  double minor_dist =
      ui::BrowserAccessibilityOHOS::MinorAxisDistance(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_GE(minor_dist, 0.0);

  double weighted_dist = ui::BrowserAccessibilityOHOS::GetWeightedDistanceFor(major_dist, minor_dist);
  EXPECT_GT(weighted_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, CandidateRectFunctionality) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(150, 50, 100, 100);
  gfx::Rect temp_best(200, 50, 100, 100);
  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::IsCandidateRect(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::IsCandidateRect(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT)));
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::CheckBetterRect(node_rect, static_cast<int32_t>(OHOS::NWeb::LEFT),
                                                             item_rect, temp_best));
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int32_t>(OHOS::NWeb::LEFT),
                                                            item_rect, temp_best));
}

TEST_F(BrowserAccessibilityOHOSTest, DistanceCalculationEdgeCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(200, 200, 100, 100);
  double far_edge_dist =
      ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(node_rect, item_rect, static_cast<int32_t>(4));
  EXPECT_GT(far_edge_dist, 0.0);

  for (int32_t direction = 2; direction <= 5; ++direction) {
    double major_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistance(node_rect, item_rect, direction);
    double minor_dist = ui::BrowserAccessibilityOHOS::MinorAxisDistance(node_rect, item_rect, direction);
    EXPECT_GE(major_dist, 0.0);
    EXPECT_GE(minor_dist, 0.0);
  }
}

TEST_F(BrowserAccessibilityOHOSTest, RectBeamEdgeCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect overlapping_rect(50, 50, 100, 100);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, overlapping_rect, static_cast<int32_t>(4)));
  gfx::Rect separated_rect(200, 200, 100, 100);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, separated_rect, static_cast<int32_t>(4)));
  gfx::Rect touching_rect(100, 0, 100, 100);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, touching_rect, static_cast<int32_t>(4)));
}

TEST_F(BrowserAccessibilityOHOSTest, DirectionJudgmentCases) {
  gfx::Rect node_rect(100, 100, 100, 100);
  gfx::Rect left_rect(0, 100, 50, 100);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, left_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
  gfx::Rect right_rect(250, 100, 50, 100);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, right_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT)));
  gfx::Rect up_rect(100, 0, 100, 50);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, up_rect, static_cast<int32_t>(OHOS::NWeb::UP)));
  gfx::Rect down_rect(100, 250, 100, 50);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, down_rect, static_cast<int32_t>(OHOS::NWeb::DOWN)));
}

TEST_F(BrowserAccessibilityOHOSTest, CandidateRectCases) {
  gfx::Rect node_rect(100, 100, 100, 100);
  gfx::Rect valid_candidate(0, 100, 50, 100);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::IsCandidateRect(node_rect, valid_candidate, static_cast<int32_t>(4)));
  gfx::Rect invalid_candidate(200, 100, 50, 100);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::IsCandidateRect(node_rect, invalid_candidate, static_cast<int32_t>(4)));
  const int32_t directions[] = {static_cast<int32_t>(OHOS::NWeb::UP), static_cast<int32_t>(OHOS::NWeb::DOWN),
                                static_cast<int32_t>(OHOS::NWeb::LEFT), static_cast<int32_t>(OHOS::NWeb::RIGHT)};
  for (int32_t direction : directions) {
    gfx::Rect test_rect;
    switch (direction) {
      case OHOS::NWeb::UP:
        test_rect = gfx::Rect(100, 0, 100, 50);
        break;
      case OHOS::NWeb::DOWN:
        test_rect = gfx::Rect(100, 200, 100, 50);
        break;
      case OHOS::NWeb::LEFT:
        test_rect = gfx::Rect(0, 100, 50, 100);
        break;
      case OHOS::NWeb::RIGHT:
        test_rect = gfx::Rect(200, 100, 50, 100);
        break;
      default:
        break;
    }
    bool is_candidate = ui::BrowserAccessibilityOHOS::IsCandidateRect(node_rect, test_rect, direction);
    EXPECT_TRUE(is_candidate);
  }
}

TEST_F(BrowserAccessibilityOHOSTest, CheckBetterRectCases) {
  gfx::Rect node_rect(100, 100, 100, 100);
  gfx::Rect item_rect(0, 100, 50, 100);
  gfx::Rect temp_best(200, 100, 50, 100);
  bool item_better = ui::BrowserAccessibilityOHOS::CheckBetterRect(node_rect, static_cast<int32_t>(OHOS::NWeb::LEFT),
                                                                   item_rect, temp_best);
  EXPECT_TRUE(item_better);
  bool temp_better = ui::BrowserAccessibilityOHOS::CheckBetterRect(node_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT),
                                                                   temp_best, item_rect);
  EXPECT_TRUE(temp_better);
}

TEST_F(BrowserAccessibilityOHOSTest, WeightedDistanceCalculation) {
  double major_dist = 10.0;
  double minor_dist = 5.0;
  double weighted_dist = ui::BrowserAccessibilityOHOS::GetWeightedDistanceFor(major_dist, minor_dist);
  EXPECT_GT(weighted_dist, 0.0);

  weighted_dist = ui::BrowserAccessibilityOHOS::GetWeightedDistanceFor(0.0, 0.0);
  EXPECT_EQ(weighted_dist, 0.0);

  weighted_dist = ui::BrowserAccessibilityOHOS::GetWeightedDistanceFor(-5.0, -3.0);
  EXPECT_GT(weighted_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, MinorAxisDistanceSpecialCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(200, 0, 100, 100);
  double horizontal_minor =
      ui::BrowserAccessibilityOHOS::MinorAxisDistance(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_EQ(horizontal_minor, 0.0);

  gfx::Rect item_rect2(0, 200, 100, 100);
  double vertical_minor =
      ui::BrowserAccessibilityOHOS::MinorAxisDistance(node_rect, item_rect2, static_cast<int32_t>(OHOS::NWeb::UP));
  EXPECT_EQ(vertical_minor, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, MajorAxisDistanceSpecialCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect overlapping_rect(50, 50, 100, 100);
  double overlap_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistance(node_rect, overlapping_rect,
                                                                        static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_EQ(overlap_dist, 0.0);

  gfx::Rect touching_rect(100, 0, 100, 100);
  double touching_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistance(node_rect, touching_rect,
                                                                         static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_EQ(touching_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, MajorAxisDistanceToFarEdgeSpecialCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(200, 0, 100, 100);
  double far_edge_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(
      node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_GT(far_edge_dist, 0.0);

  gfx::Rect overlapping_rect(50, 50, 100, 100);
  double overlap_far_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(
      node_rect, overlapping_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_GT(overlap_far_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, IsToDirectionOfEdgeCases) {
  gfx::Rect node_rect(100, 100, 100, 100);
  gfx::Rect left_rect(0, 100, 50, 100);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, left_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));

  gfx::Rect overlapping_rect(50, 100, 100, 100);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, overlapping_rect,
                                                             static_cast<int32_t>(OHOS::NWeb::LEFT)));

  gfx::Rect right_rect(200, 100, 50, 100);
  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, right_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
}

TEST_F(BrowserAccessibilityOHOSTest, CheckRectBeamEdgeCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect same_rect(0, 0, 100, 100);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, same_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));

  gfx::Rect separated_rect(200, 200, 100, 100);
  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, separated_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));

  gfx::Rect touching_rect(100, 0, 100, 100);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, touching_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
}

TEST_F(BrowserAccessibilityOHOSTest, InvalidDirectionHandling) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(200, 0, 100, 100);
  int32_t invalid_direction = 99;

  bool beam_result = ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, item_rect, invalid_direction);
  EXPECT_FALSE(beam_result);

  bool direction_result = ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, item_rect, invalid_direction);
  EXPECT_FALSE(direction_result);

  double major_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistance(node_rect, item_rect, invalid_direction);
  EXPECT_EQ(major_dist, 0.0);

  double minor_dist = ui::BrowserAccessibilityOHOS::MinorAxisDistance(node_rect, item_rect, invalid_direction);
  EXPECT_EQ(minor_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, EmptyRectHandling) {
  gfx::Rect empty_rect(0, 0, 0, 0);
  gfx::Rect normal_rect(100, 100, 100, 100);
  bool beam_result =
      ui::BrowserAccessibilityOHOS::CheckRectBeam(empty_rect, normal_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_FALSE(beam_result);

  bool direction_result =
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(empty_rect, normal_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_FALSE(direction_result);

  double major_dist =
      ui::BrowserAccessibilityOHOS::MajorAxisDistance(empty_rect, normal_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_EQ(major_dist, 0.0);

  double minor_dist =
      ui::BrowserAccessibilityOHOS::MinorAxisDistance(empty_rect, normal_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_GT(minor_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, NegativeCoordinateHandling) {
  gfx::Rect negative_rect(-100, -100, 100, 100);
  gfx::Rect positive_rect(100, 100, 100, 100);

  bool beam_result = ui::BrowserAccessibilityOHOS::CheckRectBeam(negative_rect, positive_rect,
                                                                 static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_FALSE(beam_result);

  bool direction_result = ui::BrowserAccessibilityOHOS::IsToDirectionOf(negative_rect, positive_rect,
                                                                        static_cast<int32_t>(OHOS::NWeb::RIGHT));
  EXPECT_TRUE(direction_result);

  double major_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistance(negative_rect, positive_rect,
                                                                      static_cast<int32_t>(OHOS::NWeb::RIGHT));
  EXPECT_GT(major_dist, 0.0);

  double minor_dist = ui::BrowserAccessibilityOHOS::MinorAxisDistance(negative_rect, positive_rect,
                                                                      static_cast<int32_t>(OHOS::NWeb::RIGHT));
  EXPECT_GT(minor_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, OutrightBetterCases) {
  gfx::Rect node_rect(100, 100, 100, 100);
  gfx::Rect rect1(0, 100, 50, 100);
  gfx::Rect rect2(200, 100, 50, 100);
  bool rect1_better =
      ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int32_t>(OHOS::NWeb::LEFT), rect1, rect2);
  EXPECT_FALSE(rect1_better);
  bool rect2_better =
      ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT), rect2, rect1);
  EXPECT_FALSE(rect2_better);
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_OutrightBetterTrue) {
  gfx::Rect node_rect(100, 100, 50, 50);

  gfx::Rect rect1(20, 110, 30, 30);

  gfx::Rect rect2(200, 200, 30, 30);

  bool better =
      ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int32_t>(OHOS::NWeb::LEFT), rect1, rect2);
  EXPECT_TRUE(better);
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_MajorAxisDistanceToFarEdge_AllDirs) {
  gfx::Rect a(100, 100, 50, 50);
  gfx::Rect b(10, 10, 20, 20);
  double dL = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(a, b, static_cast<int32_t>(OHOS::NWeb::LEFT));
  double dR = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(a, b, static_cast<int32_t>(OHOS::NWeb::RIGHT));
  double dU = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(a, b, static_cast<int32_t>(OHOS::NWeb::UP));
  double dD = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(a, b, static_cast<int32_t>(OHOS::NWeb::DOWN));
  EXPECT_GT(dL, 0.0);
  EXPECT_GT(dR, 0.0);
  EXPECT_GT(dU, 0.0);
  EXPECT_GT(dD, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_CheckRectBeam_InvalidDir) {
  gfx::Rect a(0, 0, 10, 10);
  gfx::Rect b(100, 100, 5, 5);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::CheckRectBeam(a, b, 99));
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_IsToDirectionOf_InvalidDir) {
  gfx::Rect a(0, 0, 10, 10);
  gfx::Rect b(100, 100, 5, 5);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::IsToDirectionOf(a, b, 99));
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_MajorMinorDistances_InvalidDir) {
  gfx::Rect a(0, 0, 10, 10);
  gfx::Rect b(100, 100, 5, 5);
  EXPECT_EQ(ui::BrowserAccessibilityOHOS::MajorAxisDistance(a, b, 99), 0.0);
  EXPECT_EQ(ui::BrowserAccessibilityOHOS::MinorAxisDistance(a, b, 99), 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_OutrightBetter_TieBreakers) {
  gfx::Rect node_rect(0, 0, 50, 50);
  int dir = static_cast<int>(OHOS::NWeb::RIGHT);

  gfx::Rect rect1(100, 0, 30, 30);
  gfx::Rect rect2(100, 40, 30, 30);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, dir, rect1, rect2));

  gfx::Rect rect3(100, 0, 30, 30);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, dir, rect1, rect3));
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_OutrightBetter_LeftRightVariants) {
  gfx::Rect node_rect(0, 0, 50, 50);

  gfx::Rect l1(-100, 10, 20, 20);
  gfx::Rect l2(10, 200, 20, 20);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int>(OHOS::NWeb::LEFT), l1, l2));

  gfx::Rect r1(100, 10, 20, 20);
  gfx::Rect r2(10, -200, 20, 20);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int>(OHOS::NWeb::RIGHT), r1, r2));
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_OutrightBetter_UpDownDistanceCompare) {
  gfx::Rect node_rect(0, 0, 50, 50);

  gfx::Rect u1(0, -60, 20, 20);
  gfx::Rect u2(0, -10, 20, 5);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int>(OHOS::NWeb::UP), u1, u2));

  gfx::Rect d1(0, 110, 20, 20);
  gfx::Rect d2(0, 60, 20, 5);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int>(OHOS::NWeb::DOWN), d1, d2));

  gfx::Rect d2_far(0, 60, 20, 200);
  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int>(OHOS::NWeb::DOWN), d1, d2_far));
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_MajorAxisDistanceToFarEdge_MinOne) {
  gfx::Rect node_rect(0, 0, 10, 10);
  gfx::Rect item_rect_right(10, 0, 1, 1);
  double dR = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(node_rect, item_rect_right,
                                                                       static_cast<int32_t>(OHOS::NWeb::RIGHT));
  EXPECT_GE(dR, 1.0);

  gfx::Rect item_rect_left(-1, 0, 1, 1);
  double dL = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(node_rect, item_rect_left,
                                                                       static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_GE(dL, 1.0);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetAccessibilityNodeByFocusMove_Directions) {
  ui::AXNodeData root;
  root.id = 8700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {8701, 8702, 8703, 8704};

  ui::AXNodeData b1;
  b1.id = 8701;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);

  ui::AXNodeData b2;
  b2.id = 8702;
  b2.role = ax::mojom::Role::kButton;
  b2.relative_bounds.bounds = gfx::RectF(100, 0, 50, 50);

  ui::AXNodeData b3;
  b3.id = 8703;
  b3.role = ax::mojom::Role::kButton;
  b3.relative_bounds.bounds = gfx::RectF(0, 100, 50, 50);

  ui::AXNodeData b4;
  b4.id = 8704;
  b4.role = ax::mojom::Role::kButton;
  b4.relative_bounds.bounds = gfx::RectF(100, 100, 50, 50);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1, b2, b3, b4);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n1 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8701));
  auto* n2 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8702));
  auto* n3 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8703));
  auto* n4 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8704));
  ASSERT_NE(n1, nullptr);
  ASSERT_NE(n2, nullptr);
  ASSERT_NE(n3, nullptr);
  ASSERT_NE(n4, nullptr);
  EXPECT_EQ(n1->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::RIGHT)), n2);
  EXPECT_EQ(n1->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::DOWN)), n3);
  EXPECT_EQ(n4->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::LEFT)), n3);
  EXPECT_EQ(n4->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::UP)), n2);
  EXPECT_EQ(n1->GetAccessibilityNodeByFocusMove(99), nullptr);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetAccessibilityNodeByFocusMove_NullManager) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  EXPECT_EQ(n->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::FORWARD)), nullptr);
  EXPECT_EQ(n->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::BACKWARD)), nullptr);
  EXPECT_EQ(n->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::LEFT)), nullptr);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_SliderScrollAndInputTagGate) {
  ui::AXNodeData root;
  root.id = 100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(101);

  ui::AXNodeData slider;
  slider.id = 101;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "input");
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 100.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 50.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kStepValueForRange, 5.0f);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* node = manager->GetFromID(101);
  ASSERT_NE(node, nullptr);
  auto* ohos_node = static_cast<ui::BrowserAccessibilityOHOS*>(node);

  EXPECT_TRUE(ohos_node->IsScrollSupported());
  EXPECT_TRUE(ohos_node->CanScrollForward());
  EXPECT_TRUE(ohos_node->CanScrollBackward());

  ui::AXNodeData root2;
  root2.id = 110;
  root2.role = ax::mojom::Role::kRootWebArea;
  root2.child_ids.push_back(111);

  ui::AXNodeData slider2;
  slider2.id = 111;
  slider2.role = ax::mojom::Role::kSlider;
  slider2.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "div");

  ui::AXTreeUpdate update2 = ui::MakeAXTreeUpdateForTesting(root2, slider2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager2(
      new ui::BrowserAccessibilityManagerOHOS(update2, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* node2 = static_cast<ui::BrowserAccessibilityOHOS*>(manager2->GetFromID(111));
  ASSERT_NE(node2, nullptr);
  EXPECT_FALSE(node2->IsScrollSupported());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ScrollableContainerCanScroll) {
  ui::AXNodeData root;
  root.id = 400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(401);

  ui::AXNodeData scrollable;
  scrollable.id = 401;
  scrollable.role = ax::mojom::Role::kGenericContainer;
  scrollable.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 10);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 20);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 100);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 100);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, scrollable);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* node = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(401));
  ASSERT_NE(node, nullptr);

  EXPECT_TRUE(node->IsScrollSupported());
  EXPECT_TRUE(node->CanScrollLeft());
  EXPECT_TRUE(node->CanScrollUp());
  EXPECT_TRUE(node->CanScrollRight());
  EXPECT_TRUE(node->CanScrollDown());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_ScrollDirectionsAndLimits) {
  ui::AXNodeData root;
  root.id = 7400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(7401);

  ui::AXNodeData sc;
  sc.id = 7401;
  sc.role = ax::mojom::Role::kGenericContainer;
  sc.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 0);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 0);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 100);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 100);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, sc);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7401));
  ASSERT_NE(n, nullptr);

  EXPECT_FALSE(n->CanScrollUp());
  EXPECT_FALSE(n->CanScrollLeft());
  EXPECT_TRUE(n->CanScrollDown());
  EXPECT_TRUE(n->CanScrollRight());

  EXPECT_TRUE(n->CanScrollDown());
  EXPECT_TRUE(n->CanScrollRight());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_AtBoundaries_CanScrollFlags) {
  ui::AXNodeData root;
  root.id = 8460;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(8461);

  ui::AXNodeData cont;
  cont.id = 8461;
  cont.role = ax::mojom::Role::kGenericContainer;
  cont.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 50);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 50);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 50);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 50);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, cont);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8461));
  ASSERT_NE(n, nullptr);

  EXPECT_TRUE(n->CanScrollUp());
  EXPECT_TRUE(n->CanScrollLeft());
  EXPECT_FALSE(n->CanScrollDown());
  EXPECT_FALSE(n->CanScrollRight());

  ui::AXNodeData root2;
  root2.id = 8470;
  root2.role = ax::mojom::Role::kRootWebArea;
  root2.child_ids.push_back(8471);

  ui::AXNodeData cont2;
  cont2.id = 8471;
  cont2.role = ax::mojom::Role::kGenericContainer;
  cont2.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 50);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 50);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 0);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 0);

  ui::AXTreeUpdate update2 = ui::MakeAXTreeUpdateForTesting(root2, cont2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager2(
      new ui::BrowserAccessibilityManagerOHOS(update2, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* m = static_cast<ui::BrowserAccessibilityOHOS*>(manager2->GetFromID(8471));
  ASSERT_NE(m, nullptr);

  EXPECT_FALSE(m->CanScrollUp());
  EXPECT_FALSE(m->CanScrollLeft());
  EXPECT_TRUE(m->CanScrollDown());
  EXPECT_TRUE(m->CanScrollRight());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_TableHeader_ImageUrl_NonScrollable) {
  ui::AXNodeData root;
  root.id = 2500;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2501);
  root.child_ids.push_back(2502);
  root.child_ids.push_back(2503);

  ui::AXNodeData header;
  header.id = 2501;
  header.role = ax::mojom::Role::kColumnHeader;

  ui::AXNodeData image;
  image.id = 2502;
  image.role = ax::mojom::Role::kImage;
  image.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://img.invalid/x.png");

  ui::AXNodeData container;
  container.id = 2503;
  container.role = ax::mojom::Role::kGenericContainer;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, header, image, container);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* h = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2501));
  auto* im = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2502));
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2503));
  ASSERT_NE(h, nullptr);
  ASSERT_NE(im, nullptr);
  ASSERT_NE(c, nullptr);
  EXPECT_TRUE(h->IsTableHeader());
  EXPECT_EQ(im->GetTargetUrl(), "https://img.invalid/x.png");
  EXPECT_FALSE(c->IsScrollSupported());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_SliderEdgeScrollability) {
  ui::AXNodeData root;
  root.id = 2900;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2901);

  ui::AXNodeData slider;
  slider.id = 2901;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "input");
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 10.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 10.0f);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* s = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2901));
  ASSERT_NE(s, nullptr);
  EXPECT_FALSE(s->CanScrollForward());

  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 0.0f);

  ui::AXTreeUpdate update2 = ui::MakeAXTreeUpdateForTesting(root, slider);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager2(
      new ui::BrowserAccessibilityManagerOHOS(update2, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  s = static_cast<ui::BrowserAccessibilityOHOS*>(manager2->GetFromID(2901));
  EXPECT_FALSE(s->CanScrollBackward());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ValueExposureAndTargetUrl) {
  ui::AXNodeData root;
  root.id = 200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(201);
  root.child_ids.push_back(202);

  ui::AXNodeData textfield;
  textfield.id = 201;
  textfield.role = ax::mojom::Role::kTextField;
  textfield.SetValue("Typed Value");

  ui::AXNodeData link;
  link.id = 202;
  link.role = ax::mojom::Role::kLink;
  link.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, textfield, link);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* text_node = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(201));
  ASSERT_NE(text_node, nullptr);
  EXPECT_TRUE(text_node->ShouldExposeValueAsName());

  auto* link_node = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(202));
  ASSERT_NE(link_node, nullptr);
  EXPECT_EQ(link_node->GetTargetUrl(), "https://example.com");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetTargetUrl_Branches) {
  ui::AXNodeData root;
  root.id = 5600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(5601);
  root.child_ids.push_back(5602);
  root.child_ids.push_back(5603);

  ui::AXNodeData image;
  image.id = 5601;
  image.role = ax::mojom::Role::kImage;
  image.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/image.jpg");

  ui::AXNodeData link;
  link.id = 5602;
  link.role = ax::mojom::Role::kLink;
  link.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/page");

  ui::AXNodeData video;
  video.id = 5603;
  video.role = ax::mojom::Role::kVideo;
  video.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/video.mp4");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, image, link, video);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* img = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5601));
  auto* lnk = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5602));
  auto* vid = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5603));

  ASSERT_NE(img, nullptr);
  ASSERT_NE(lnk, nullptr);
  ASSERT_NE(vid, nullptr);

  EXPECT_EQ(img->GetTargetUrl(), "https://example.com/image.jpg");
  EXPECT_EQ(lnk->GetTargetUrl(), "https://example.com/page");
  EXPECT_EQ(vid->GetTargetUrl(), "https://example.com/video.mp4");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetTargetUrl_NonMediaLink) {
  ui::AXNodeData root;
  root.id = 5700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(5701);

  ui::AXNodeData button;
  button.id = 5701;
  button.role = ax::mojom::Role::kButton;
  button.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/button");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* btn = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5701));
  ASSERT_NE(btn, nullptr);

  auto url = btn->GetTargetUrl();
  EXPECT_EQ(url, std::string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetTargetUrl_LinkAndNonLink) {
  // Link with URL
  {
    ui::AXNodeData root;
    root.id = 8600;
    root.role = ax::mojom::Role::kRootWebArea;
    root.child_ids.push_back(8601);

    ui::AXNodeData link;
    link.id = 8601;
    link.role = ax::mojom::Role::kLink;
    link.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/a");

    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, link);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager(
        new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8601));
    ASSERT_NE(n, nullptr);
    EXPECT_EQ(n->GetTargetUrl(), std::string("https://example.com/a"));
  }

  // Non-link returns empty
  {
    ui::AXNodeData root;
    root.id = 8610;
    root.role = ax::mojom::Role::kRootWebArea;
    root.child_ids.push_back(8611);

    ui::AXNodeData btn;
    btn.id = 8611;
    btn.role = ax::mojom::Role::kButton;
    btn.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/img");

    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, btn);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager(
        new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8611));
    ASSERT_NE(n, nullptr);
    EXPECT_EQ(n->GetTargetUrl(), std::string());
  }
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetTargetUrl_VideoAndImage) {
  ui::AXNodeData root;
  root.id = 9850;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9851);
  root.child_ids.push_back(9852);

  ui::AXNodeData img;
  img.id = 9851;
  img.role = ax::mojom::Role::kImage;
  img.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://a/img.png");

  ui::AXNodeData video;
  video.id = 9852;
  video.role = ax::mojom::Role::kVideo;
  video.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://a/v.mp4");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, img, video);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n_img = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9851));
  auto* n_vid = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9852));
  ASSERT_NE(n_img, nullptr);
  ASSERT_NE(n_vid, nullptr);
  EXPECT_EQ(n_img->GetTargetUrl(), std::string("https://a/img.png"));
  EXPECT_EQ(n_vid->GetTargetUrl(), std::string("https://a/v.mp4"));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ColorWellTextAndRangeSupport) {
  ui::AXNodeData root;
  root.id = 300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(301);
  root.child_ids.push_back(302);

  ui::AXNodeData colorwell;
  colorwell.id = 301;
  colorwell.role = ax::mojom::Role::kColorWell;
  colorwell.AddIntAttribute(ax::mojom::IntAttribute::kColorValue, 0xFF0000);

  ui::AXNodeData slider;
  slider.id = 302;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 1.0f);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, colorwell, slider);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* cw = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(301));
  ASSERT_NE(cw, nullptr);
  auto text16 = cw->GetSubstringTextContentUTF16(absl::nullopt);

  ASSERT_GE(text16.size(), static_cast<size_t>(7));
  EXPECT_EQ(text16[0], u'#');

  auto* sl = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(302));
  ASSERT_NE(sl, nullptr);
  EXPECT_TRUE(sl->GetData().IsRangeValueSupported());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_InputTypeAndLiveRegion) {
  ui::AXNodeData root;
  root.id = 600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(601);
  root.child_ids.push_back(602);

  ui::AXNodeData input;
  input.id = 601;
  input.role = ax::mojom::Role::kTextField;
  input.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "input");
  input.AddStringAttribute(ax::mojom::StringAttribute::kInputType, "email");

  ui::AXNodeData live;
  live.id = 602;
  live.role = ax::mojom::Role::kGenericContainer;
  live.AddStringAttribute(ax::mojom::StringAttribute::kLiveStatus, "assertive");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input, live);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* in = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(601));
  ASSERT_NE(in, nullptr);
  EXPECT_EQ(in->OHOSInputType(), static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_EMAIL));

  auto* lr = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(602));
  ASSERT_NE(lr, nullptr);
  EXPECT_EQ(lr->OHOSLiveRegionType(), 2);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_InputTypeAllAndLiveRegionAll) {
  ui::AXNodeData root;
  root.id = 2300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2301);
  root.child_ids.push_back(2302);

  ui::AXNodeData input;
  input.id = 2301;
  input.role = ax::mojom::Role::kTextField;
  input.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "input");
  input.AddStringAttribute(ax::mojom::StringAttribute::kInputType, "text");

  ui::AXNodeData live;
  live.id = 2302;
  live.role = ax::mojom::Role::kGenericContainer;
  live.AddStringAttribute(ax::mojom::StringAttribute::kLiveStatus, "polite");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input, live);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* in = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2301));
  auto* lr = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2302));
  ASSERT_NE(in, nullptr);
  ASSERT_NE(lr, nullptr);

  struct Case {
    const char* type;
    int32_t expect;
  } cases[] = {
      {"", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TEXT)},
      {"text", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TEXT)},
      {"search", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TEXT)},
      {"date", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_DATE)},
      {"datetime", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TIME)},
      {"datetime-local", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TIME)},
      {"email", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_EMAIL)},
      {"month", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_DATE)},
      {"number", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_NUMBER)},
      {"password", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_PASSWORD)},
      {"tel", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_PHONENUMBER)},
      {"time", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TIME)},
      {"url", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_DEFAULT)},
      {"week", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_DATE)},
  };

  for (const auto& c : cases) {
    ui::AXNodeData upd = input;
    upd.AddStringAttribute(ax::mojom::StringAttribute::kInputType, c.type);

    ui::AXTreeUpdate upd_case = ui::MakeAXTreeUpdateForTesting(root, upd, live);
    std::unique_ptr<ui::BrowserAccessibilityManager> m_case(new ui::BrowserAccessibilityManagerOHOS(
        upd_case, node_id_delegate_, test_browser_accessibility_delegate_.get()));
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(m_case->GetFromID(2301));
    ASSERT_NE(n, nullptr);
    EXPECT_EQ(n->OHOSInputType(), c.expect);
  }

  EXPECT_EQ(lr->OHOSLiveRegionType(), 1);
  ui::AXNodeData upd_live = live;
  upd_live.AddStringAttribute(ax::mojom::StringAttribute::kLiveStatus, "assertive");

  upd_live.AddStringAttribute(ax::mojom::StringAttribute::kLiveStatus, "assertive");
  ui::AXTreeUpdate upd1 = ui::MakeAXTreeUpdateForTesting(root, input, upd_live);
  std::unique_ptr<ui::BrowserAccessibilityManager> m1(
      new ui::BrowserAccessibilityManagerOHOS(upd1, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  lr = static_cast<ui::BrowserAccessibilityOHOS*>(m1->GetFromID(2302));
  ASSERT_NE(lr, nullptr);
  EXPECT_EQ(lr->OHOSLiveRegionType(), 2);

  upd_live.AddStringAttribute(ax::mojom::StringAttribute::kLiveStatus, "off");
  ui::AXTreeUpdate upd2 = ui::MakeAXTreeUpdateForTesting(root, input, upd_live);
  std::unique_ptr<ui::BrowserAccessibilityManager> m2(
      new ui::BrowserAccessibilityManagerOHOS(upd2, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  lr = static_cast<ui::BrowserAccessibilityOHOS*>(m2->GetFromID(2302));
  ASSERT_NE(lr, nullptr);
  EXPECT_EQ(lr->OHOSLiveRegionType(), 0);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_InputType_NonInputTag) {
  ui::AXNodeData root;
  root.id = 2999;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3000);

  ui::AXNodeData node;
  node.id = 3000;
  node.role = ax::mojom::Role::kTextField;
  node.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "div");
  node.AddStringAttribute(ax::mojom::StringAttribute::kInputType, "password");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, node);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3000));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->OHOSInputType(), static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_DEFAULT));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescriptionVariants) {
  ui::AXNodeData root;
  root.id = 700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(701);
  root.child_ids.push_back(702);
  root.child_ids.push_back(703);

  ui::AXNodeData heading;
  heading.id = 701;
  heading.role = ax::mojom::Role::kHeading;
  heading.SetName("H");

  ui::AXNodeData link;
  link.id = 702;
  link.role = ax::mojom::Role::kLink;
  link.AddState(ax::mojom::State::kVisited);

  ui::AXNodeData button;
  button.id = 703;
  button.role = ax::mojom::Role::kButton;
  button.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kMenu));

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, link, button);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* h = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(701));
  auto* l = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(702));
  auto* b = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(703));
  ASSERT_NE(h, nullptr);
  ASSERT_NE(l, nullptr);
  ASSERT_NE(b, nullptr);

  auto h_desc = h->GetRoleDescription();
  EXPECT_EQ(h_desc, u"Heading");
  auto l_desc = l->GetRoleDescription();

  EXPECT_EQ(l_desc, u" Visited");
  auto b_desc = b->GetRoleDescription();
  EXPECT_EQ(b_desc, u"Popup button menu");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_MoreRoles) {
  ui::AXNodeData root;
  root.id = 3200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3201);
  root.child_ids.push_back(3202);
  root.child_ids.push_back(3203);
  root.child_ids.push_back(3204);
  root.child_ids.push_back(3205);

  ui::AXNodeData canvas;
  canvas.id = 3201;
  canvas.role = ax::mojom::Role::kCanvas;

  ui::AXNodeData header;
  header.id = 3202;
  header.role = ax::mojom::Role::kHeader;

  ui::AXNodeData listgrid;
  listgrid.id = 3203;
  listgrid.role = ax::mojom::Role::kListGrid;

  ui::AXNodeData mir;
  mir.id = 3204;
  mir.role = ax::mojom::Role::kMenuItemRadio;

  ui::AXNodeData rb;
  rb.id = 3205;
  rb.role = ax::mojom::Role::kRadioButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, canvas, header, listgrid, mir, rb);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  int ids[] = {3201, 3202, 3203, 3204, 3205};
  for (int id : ids) {
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(id));
    ASSERT_NE(n, nullptr);
    auto desc = n->GetRoleDescription();
    EXPECT_FALSE(desc.empty());
  }
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_AriaRoleDescription) {
  ui::AXNodeData root;
  root.id = 3300;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData button;
  button.id = 3301;
  button.role = ax::mojom::Role::kButton;
  button.AddStringAttribute(ax::mojom::StringAttribute::kRoleDescription, "Custom Button");

  root.child_ids = {3301};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3301));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, u"Custom Button");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_HeadingNoVisited) {
  ui::AXNodeData root;
  root.id = 3400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3401);

  ui::AXNodeData heading;
  heading.id = 3401;
  heading.role = ax::mojom::Role::kHeading;
  heading.SetName("Test Heading");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3401));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, u"Heading");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_HeadingWithVisited) {
  ui::AXNodeData root;
  root.id = 3500;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3501);

  ui::AXNodeData heading;
  heading.id = 3501;
  heading.role = ax::mojom::Role::kHeading;
  heading.SetName("Test Heading");
  heading.AddState(ax::mojom::State::kVisited);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3501));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, u"Heading Visited");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_LinkNoVisited) {
  ui::AXNodeData root;
  root.id = 3600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3601);

  ui::AXNodeData link;
  link.id = 3601;
  link.role = ax::mojom::Role::kLink;
  link.SetName("Test Link");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, link);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3601));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, std::u16string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_ButtonNoPopup) {
  ui::AXNodeData root;
  root.id = 3700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3701);

  ui::AXNodeData button;
  button.id = 3701;
  button.role = ax::mojom::Role::kButton;
  button.SetName("Test Button");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3701));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, std::u16string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_ButtonPopupTypes) {
  ui::AXNodeData root;
  root.id = 3800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3801);
  root.child_ids.push_back(3802);
  root.child_ids.push_back(3803);
  root.child_ids.push_back(3804);

  ui::AXNodeData button1;
  button1.id = 3801;
  button1.role = ax::mojom::Role::kButton;
  button1.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kTrue));

  ui::AXNodeData button2;
  button2.id = 3802;
  button2.role = ax::mojom::Role::kButton;
  button2.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kDialog));

  ui::AXNodeData button3;
  button3.id = 3803;
  button3.role = ax::mojom::Role::kButton;
  button3.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kListbox));

  ui::AXNodeData button4;
  button4.id = 3804;
  button4.role = ax::mojom::Role::kButton;
  button4.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kFalse));

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button1, button2, button3, button4);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* b1 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3801));
  auto* b2 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3802));
  auto* b3 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3803));
  auto* b4 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3804));

  ASSERT_NE(b1, nullptr);
  ASSERT_NE(b2, nullptr);
  ASSERT_NE(b3, nullptr);
  ASSERT_NE(b4, nullptr);

  auto desc1 = b1->GetRoleDescription();
  auto desc2 = b2->GetRoleDescription();
  auto desc3 = b3->GetRoleDescription();
  auto desc4 = b4->GetRoleDescription();

  EXPECT_EQ(desc1, u"Popup button menu");
  EXPECT_EQ(desc2, u"Popup button dialog");
  EXPECT_EQ(desc3, u"Popup button");
  EXPECT_EQ(desc4, std::u16string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_VariousRoles) {
  ui::AXNodeData root;
  root.id = 3900;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3901);
  root.child_ids.push_back(3902);
  root.child_ids.push_back(3903);
  root.child_ids.push_back(3904);
  root.child_ids.push_back(3905);
  root.child_ids.push_back(3906);
  root.child_ids.push_back(3907);
  root.child_ids.push_back(3908);

  ui::AXNodeData alert;
  alert.id = 3901;
  alert.role = ax::mojom::Role::kAlert;

  ui::AXNodeData audio;
  audio.id = 3902;
  audio.role = ax::mojom::Role::kAudio;

  ui::AXNodeData code;
  code.id = 3903;
  code.role = ax::mojom::Role::kCode;

  ui::AXNodeData details;
  details.id = 3904;
  details.role = ax::mojom::Role::kDetails;

  ui::AXNodeData emphasis;
  emphasis.id = 3905;
  emphasis.role = ax::mojom::Role::kEmphasis;

  ui::AXNodeData form;
  form.id = 3906;
  form.role = ax::mojom::Role::kForm;

  ui::AXNodeData rowgroup;
  rowgroup.id = 3907;
  rowgroup.role = ax::mojom::Role::kRowGroup;

  ui::AXNodeData sectionfooter;
  sectionfooter.id = 3908;
  sectionfooter.role = ax::mojom::Role::kSectionFooter;

  ui::AXTreeUpdate update =
      ui::MakeAXTreeUpdateForTesting(root, alert, audio, code, details, emphasis, form, rowgroup, sectionfooter);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  int ids[] = {3901, 3902, 3903, 3904, 3905, 3906, 3907, 3908};
  for (int id : ids) {
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(id));
    ASSERT_NE(n, nullptr);
    auto desc = n->GetRoleDescription();

    EXPECT_EQ(desc, std::u16string());
  }
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_MoreRoleTypes) {
  ui::AXNodeData root;
  root.id = 4000;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4001);
  root.child_ids.push_back(4002);
  root.child_ids.push_back(4003);
  root.child_ids.push_back(4004);
  root.child_ids.push_back(4005);
  root.child_ids.push_back(4006);
  root.child_ids.push_back(4007);
  root.child_ids.push_back(4008);

  ui::AXNodeData sectionheader;
  sectionheader.id = 4001;
  sectionheader.role = ax::mojom::Role::kSectionHeader;

  ui::AXNodeData sectionwithoutname;
  sectionwithoutname.id = 4002;
  sectionwithoutname.role = ax::mojom::Role::kSectionWithoutName;

  ui::AXNodeData strong;
  strong.id = 4003;
  strong.role = ax::mojom::Role::kStrong;

  ui::AXNodeData subscript;
  subscript.id = 4004;
  subscript.role = ax::mojom::Role::kSubscript;

  ui::AXNodeData superscript;
  superscript.id = 4005;
  superscript.role = ax::mojom::Role::kSuperscript;

  ui::AXNodeData time;
  time.id = 4006;
  time.role = ax::mojom::Role::kTime;

  ui::AXNodeData combobox;
  combobox.id = 4007;
  combobox.role = ax::mojom::Role::kComboBoxSelect;

  ui::AXNodeData figure;
  figure.id = 4008;
  figure.role = ax::mojom::Role::kFigure;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, sectionheader, sectionwithoutname, strong, subscript,
                                                           superscript, time, combobox, figure);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  int no_desc_ids[] = {4001, 4002, 4003, 4004, 4005, 4006};
  for (int id : no_desc_ids) {
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(id));
    ASSERT_NE(n, nullptr);
    auto desc = n->GetRoleDescription();
    EXPECT_TRUE(desc.empty());
  }

  auto* cb = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4007));
  auto* fig = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4008));
  ASSERT_NE(cb, nullptr);
  ASSERT_NE(fig, nullptr);

  auto cb_desc = cb->GetRoleDescription();
  auto fig_desc = fig->GetRoleDescription();
  EXPECT_EQ(cb_desc, u"Combo box");
  EXPECT_EQ(fig_desc, u"Graphic");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_DefaultBranch) {
  ui::AXNodeData root;
  root.id = 4100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4101);

  ui::AXNodeData generic;
  generic.id = 4101;
  generic.role = ax::mojom::Role::kGenericContainer;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, generic);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4101));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, std::u16string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetRoleDescription_ButtonPopupDetailed) {
  ui::AXNodeData root;
  root.id = 7800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(7801);
  root.child_ids.push_back(7802);
  root.child_ids.push_back(7803);

  ui::AXNodeData menu_btn;
  menu_btn.id = 7801;
  menu_btn.role = ax::mojom::Role::kButton;
  menu_btn.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kMenu));

  ui::AXNodeData dlg_btn;
  dlg_btn.id = 7802;
  dlg_btn.role = ax::mojom::Role::kButton;
  dlg_btn.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kDialog));

  ui::AXNodeData list_btn;
  list_btn.id = 7803;
  list_btn.role = ax::mojom::Role::kButton;
  list_btn.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kListbox));

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, menu_btn, dlg_btn, list_btn);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* m = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7801));
  auto* d = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7802));
  auto* l = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7803));
  ASSERT_NE(m, nullptr);
  ASSERT_NE(d, nullptr);
  ASSERT_NE(l, nullptr);
  EXPECT_EQ(m->GetRoleDescription(), u"Popup button menu");
  EXPECT_EQ(d->GetRoleDescription(), u"Popup button dialog");
  EXPECT_EQ(l->GetRoleDescription(), u"Popup button");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_SubstringTextContent_Compose) {
  ui::AXNodeData root;
  root.id = 800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(801);

  ui::AXNodeData tf;
  tf.id = 801;
  tf.role = ax::mojom::Role::kTextField;
  tf.SetValue("only-value");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, tf);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));

  auto* node = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(801));
  ASSERT_NE(node, nullptr);
  auto text16 = node->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_EQ(text16, u"only-value");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ImageAnnotationStatus_PendingAdultNoDescSucceeded) {
  // Root -> image; cover multiple statuses by re-instantiating trees
  auto make_root = [](int id_root, int id_img) {
    ui::AXNodeData root;
    root.id = id_root;
    root.role = ax::mojom::Role::kRootWebArea;
    root.child_ids.push_back(id_img);
    return root;
  };

  // Pending
  {
    ui::AXNodeData root = make_root(8100, 8101);
    ui::AXNodeData img;
    img.id = 8101;
    img.role = ax::mojom::Role::kImage;
    img.AddIntAttribute(ax::mojom::IntAttribute::kImageAnnotationStatus,
                        static_cast<int>(ax::mojom::ImageAnnotationStatus::kAnnotationPending));
    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, img);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager(
        new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
    auto* i = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8101));
    ASSERT_NE(i, nullptr);
    auto text = i->GetSubstringTextContentUTF16(absl::nullopt);
    EXPECT_EQ(text, u"Getting description...");
  }

  // Adult
  {
    ui::AXNodeData root = make_root(8110, 8111);
    ui::AXNodeData img;
    img.id = 8111;
    img.role = ax::mojom::Role::kImage;
    img.AddIntAttribute(ax::mojom::IntAttribute::kImageAnnotationStatus,
                        static_cast<int>(ax::mojom::ImageAnnotationStatus::kAnnotationAdult));
    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, img);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager(
        new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
    auto* i = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8111));
    ASSERT_NE(i, nullptr);
    auto text = i->GetSubstringTextContentUTF16(absl::nullopt);
    EXPECT_EQ(text, u"Appears to contain adult content. No description available.");
  }

  // No description (Empty)
  {
    ui::AXNodeData root = make_root(8120, 8121);
    ui::AXNodeData img;
    img.id = 8121;
    img.role = ax::mojom::Role::kImage;
    img.AddIntAttribute(ax::mojom::IntAttribute::kImageAnnotationStatus,
                        static_cast<int>(ax::mojom::ImageAnnotationStatus::kAnnotationEmpty));
    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, img);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager(
        new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
    auto* i = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8121));
    ASSERT_NE(i, nullptr);
    auto text = i->GetSubstringTextContentUTF16(absl::nullopt);
    EXPECT_EQ(text, u"No description available.");
  }

  // Succeeded -> uses kImageAnnotation string
  {
    ui::AXNodeData root = make_root(8130, 8131);
    ui::AXNodeData img;
    img.id = 8131;
    img.role = ax::mojom::Role::kImage;
    img.AddIntAttribute(ax::mojom::IntAttribute::kImageAnnotationStatus,
                        static_cast<int>(ax::mojom::ImageAnnotationStatus::kAnnotationSucceeded));
    img.AddStringAttribute(ax::mojom::StringAttribute::kImageAnnotation, "ALT-TEXT");
    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, img);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager(
        new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
    auto* i = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8131));
    ASSERT_NE(i, nullptr);
    auto text = i->GetSubstringTextContentUTF16(absl::nullopt);
    EXPECT_EQ(text, u"ALT-TEXT");
  }
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_DocumentSplitterAndUrlFallback) {
  ui::AXNodeData root;
  root.id = 1800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.SetName("DocTitle");
  root.child_ids.push_back(1801);
  root.child_ids.push_back(1802);
  ui::AXNodeData splitter;
  splitter.id = 1801;
  splitter.role = ax::mojom::Role::kSplitter;
  splitter.SetName("Split");
  ui::AXNodeData link;
  link.id = 1802;
  link.role = ax::mojom::Role::kLink;
  link.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/path?q=1");
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, splitter, link);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* r = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1800));
  auto* s = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1801));
  auto* l = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1802));
  ASSERT_NE(r, nullptr);
  ASSERT_NE(s, nullptr);
  ASSERT_NE(l, nullptr);
  auto r_text = r->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_EQ(r_text, std::u16string());
  auto s_text = s->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_EQ(s_text, u"Split");
  auto l_text = l->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(l_text.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_SubstringTextContent_RangeCompose) {
  ui::AXNodeData root;
  root.id = 2199;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2200);
  ui::AXNodeData node;
  node.id = 2200;
  node.role = ax::mojom::Role::kSlider;
  node.SetName("Label");
  node.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 3.0f);
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, node);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2200));
  ASSERT_NE(n, nullptr);
  auto text = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(text.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContent_RootAndSplitterReturnOwnText) {
  ui::AXNodeData root;
  root.id = 7700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.SetName("RootTitle");
  root.child_ids.push_back(7701);
  ui::AXNodeData splitter;
  splitter.id = 7701;
  splitter.role = ax::mojom::Role::kSplitter;
  splitter.SetName("Divider");
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, splitter);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* r = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7700));
  auto* s = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7701));
  ASSERT_NE(r, nullptr);
  ASSERT_NE(s, nullptr);
  EXPECT_EQ(r->GetSubstringTextContentUTF16(absl::nullopt), std::u16string());
  EXPECT_EQ(s->GetSubstringTextContentUTF16(absl::nullopt), u"Divider");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_IframeRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  ui::AXNodeData iframe;
  iframe.id = 2;
  iframe.role = ax::mojom::Role::kIframe;
  iframe.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test iframe");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, iframe);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_TRUE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_RootWebAreaRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  root.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test root");
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_TRUE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_ColorWellRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  ui::AXNodeData color_well;
  color_well.id = 2;
  color_well.role = ax::mojom::Role::kColorWell;
  color_well.AddIntAttribute(ax::mojom::IntAttribute::kColorValue, 0xFF0000);
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, color_well);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
  EXPECT_TRUE(result.find(u"#") != std::u16string::npos);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_RangeValueSupported) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  ui::AXNodeData slider;
  slider.id = 2;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddStringAttribute(ax::mojom::StringAttribute::kName, "Volume");
  slider.AddStringAttribute(ax::mojom::StringAttribute::kValue, "50");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_RangeValueSupportedEmptyName) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  ui::AXNodeData slider;
  slider.id = 2;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddStringAttribute(ax::mojom::StringAttribute::kValue, "75");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_EmptyNameUseValue) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  ui::AXNodeData text_field;
  text_field.id = 2;
  text_field.role = ax::mojom::Role::kTextField;
  text_field.AddStringAttribute(ax::mojom::StringAttribute::kValue, "input value");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, text_field);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_PlatformDocumentRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  root.AddStringAttribute(ax::mojom::StringAttribute::kName, "Document title");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_TRUE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_SplitterRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData splitter;
  splitter.id = 2;
  splitter.role = ax::mojom::Role::kSplitter;
  splitter.AddStringAttribute(ax::mojom::StringAttribute::kName, "Resize handle");

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, splitter);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_ImageAnnotationSucceeded) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData image;
  image.id = 2;
  image.role = ax::mojom::Role::kImage;
  image.AddStringAttribute(ax::mojom::StringAttribute::kImageAnnotation, "A beautiful sunset");
  image.AddIntAttribute(ax::mojom::IntAttribute::kImageAnnotationStatus,
                        static_cast<int32_t>(ax::mojom::ImageAnnotationStatus::kAnnotationSucceeded));

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, image);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_AlertRoleWithChildren) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData alert;
  alert.id = 2;
  alert.role = ax::mojom::Role::kAlert;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;
  text.AddStringAttribute(ax::mojom::StringAttribute::kName, "Alert message");

  root.child_ids = {2};
  alert.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, alert, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_WithPredicate) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData div;
  div.id = 2;
  div.role = ax::mojom::Role::kGenericContainer;

  ui::AXNodeData text1;
  text1.id = 3;
  text1.role = ax::mojom::Role::kStaticText;
  text1.AddStringAttribute(ax::mojom::StringAttribute::kName, "First");

  ui::AXNodeData text2;
  text2.id = 4;
  text2.role = ax::mojom::Role::kStaticText;
  text2.AddStringAttribute(ax::mojom::StringAttribute::kName, "Second");

  root.child_ids = {2};
  div.child_ids = {3, 4};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, div, text1, text2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  auto predicate = ui::BrowserAccessibilityOHOS::NonEmptyPredicate();
  std::u16string result = n->GetSubstringTextContentUTF16(predicate);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_NonEmptyPredicate) {
  auto predicate = ui::BrowserAccessibilityOHOS::NonEmptyPredicate();
  EXPECT_FALSE(predicate.Run(u""));
  EXPECT_TRUE(predicate.Run(u"x"));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_LinkRoleEmptyName) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData link;
  link.id = 2;
  link.role = ax::mojom::Role::kLink;
  link.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, link);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_ImageRoleEmptyName) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData image;
  image.id = 2;
  image.role = ax::mojom::Role::kImage;
  image.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/image.jpg");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, image);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager(
      new ui::BrowserAccessibilityManagerOHOS(update, node_id_delegate_, test_browser_accessibility_delegate_.get()));
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}
}  // namespace content