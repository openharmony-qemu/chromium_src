// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unordered_set>

#include "testing/gtest/include/gtest/gtest.h"

#include "ui/accessibility/ax_enum_util.h"
#include "ui/accessibility/ax_enums.mojom.h"
#include "ui/accessibility/ax_role_properties.h"

namespace ui {

TEST(AXRolePropertiesTest, TestSupportsHierarchicalLevel) {
  // Test for iterating through all roles and validate if a role supports
  // hierarchical level.
  std::unordered_set<ax::mojom::Role>
      roles_expected_supports_hierarchical_level = {
          ax::mojom::Role::kComment, ax::mojom::Role::kListItem,
          ax::mojom::Role::kRow, ax::mojom::Role::kTreeItem};

  for (int role_idx = static_cast<int>(ax::mojom::Role::kMinValue);
       role_idx <= static_cast<int>(ax::mojom::Role::kMaxValue); role_idx++) {
    ax::mojom::Role role = static_cast<ax::mojom::Role>(role_idx);
    bool supports_hierarchical_level = SupportsHierarchicalLevel(role);

    SCOPED_TRACE(testing::Message() << "ax::mojom::Role=" << ToString(role)
                                    << ", Actual: supportsHierarchicalLevel="
                                    << supports_hierarchical_level
                                    << ", Expected: supportsHierarchicalLevel="
                                    << !supports_hierarchical_level);

    if (roles_expected_supports_hierarchical_level.find(role) !=
        roles_expected_supports_hierarchical_level.end())
      EXPECT_TRUE(supports_hierarchical_level);
    else
      EXPECT_FALSE(supports_hierarchical_level);
  }
}

TEST(AXRolePropertiesTest, TestSupportsToggle) {
  // Test for iterating through all roles and validate if a role supports
  // toggle.
  std::unordered_set<ax::mojom::Role> roles_expected_supports_toggle = {
      ax::mojom::Role::kCheckBox, ax::mojom::Role::kMenuItemCheckBox,
      ax::mojom::Role::kSwitch, ax::mojom::Role::kToggleButton};

  for (int role_idx = static_cast<int>(ax::mojom::Role::kMinValue);
       role_idx <= static_cast<int>(ax::mojom::Role::kMaxValue); role_idx++) {
    ax::mojom::Role role = static_cast<ax::mojom::Role>(role_idx);
    bool supports_toggle = SupportsToggle(role);

    SCOPED_TRACE(testing::Message()
                 << "ax::mojom::Role=" << ToString(role)
                 << ", Actual: supportsToggle=" << supports_toggle
                 << ", Expected: supportsToggle=" << !supports_toggle);

    if (roles_expected_supports_toggle.find(role) !=
        roles_expected_supports_toggle.end())
      EXPECT_TRUE(supports_toggle);
    else
      EXPECT_FALSE(supports_toggle);
  }
}

TEST(AXRolePropertiesTest, IsControlOnOHOS) {
  auto result = IsControlOnOHOS(ax::mojom::Role::kSplitter, true);
  EXPECT_EQ(result, true);

  result = IsControlOnOHOS(ax::mojom::Role::kSplitter, false);
  EXPECT_EQ(result, false);

  result = IsControlOnOHOS(ax::mojom::Role::kDate, false);
  EXPECT_EQ(result, true);

  result = IsControlOnOHOS(ax::mojom::Role::kDateTime, false);
  EXPECT_EQ(result, true);

  result = IsControlOnOHOS(ax::mojom::Role::kDocBackLink, false);
  EXPECT_EQ(result, true);

  result = IsControlOnOHOS(ax::mojom::Role::kDocBiblioRef, false);
  EXPECT_EQ(result, true);

  result = IsControlOnOHOS(ax::mojom::Role::kDocGlossRef, false);
  EXPECT_EQ(result, true);

  result = IsControlOnOHOS(ax::mojom::Role::kDocNoteRef, false);
  EXPECT_EQ(result, true);

  result = IsControlOnOHOS(ax::mojom::Role::kInputTime, false);
  EXPECT_EQ(result, true);

  result = IsControlOnOHOS(ax::mojom::Role::kLink, false);
  EXPECT_EQ(result, true);

  result = IsControlOnOHOS(ax::mojom::Role::kTreeItem, false);
  EXPECT_EQ(result, true);

  result = IsControlOnOHOS(ax::mojom::Role::kAlert, false);
  EXPECT_EQ(result, false);

  result = IsControlOnOHOS(ax::mojom::Role::kDialog, false);
  EXPECT_EQ(result, false);

  result = IsControlOnOHOS(ax::mojom::Role::kMenu, false);
  EXPECT_EQ(result, false);

  result = IsControlOnOHOS(ax::mojom::Role::kMenuBar, false);
  EXPECT_EQ(result, false);

  result = IsControlOnOHOS(ax::mojom::Role::kTree, false);
  EXPECT_EQ(result, false);

  result = IsControlOnOHOS(ax::mojom::Role::kUnknown, false);
  EXPECT_EQ(result, false);

  result = IsControlOnOHOS(ax::mojom::Role::kButton, false);
  EXPECT_EQ(result, IsControl(ax::mojom::Role::kButton));

  result = IsControlOnOHOS(ax::mojom::Role::kCheckBox, false);
  EXPECT_EQ(result, IsControl(ax::mojom::Role::kCheckBox));
}
}  // namespace ui
