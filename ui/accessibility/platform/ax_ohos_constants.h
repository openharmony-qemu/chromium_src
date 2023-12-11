// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef UI_ACCESSIBILITY_PLATFORM_AX_OHOS_CONSTANTS_H_
#define UI_ACCESSIBILITY_PLATFORM_AX_OHOS_CONSTANTS_H_

#include "ui/accessibility/ax_export.h"

namespace ui {

AX_EXPORT extern const char ATTRS_COMMON_WIDTH[];
AX_EXPORT extern const char ATTRS_COMMON_HEIGHT[];
AX_EXPORT extern const char ATTRS_COMMON_PADDING[];
AX_EXPORT extern const char ATTRS_COMMON_MARGIN[];
AX_EXPORT extern const char ATTRS_COMMON_LAYOUT_PRIORITY[];
AX_EXPORT extern const char ATTRS_COMMON_LAYOUT_WEIGHT[];
// position
AX_EXPORT extern const char ATTRS_COMMON_ALIGN[];
AX_EXPORT extern const char ATTRS_COMMON_DIRECTION[];
AX_EXPORT extern const char ATTRS_COMMON_POSITION[];
AX_EXPORT extern const char ATTRS_COMMON_OFFSET[];
AX_EXPORT extern const char ATTRS_COMMON_USE_ALIGN[];
// layout
AX_EXPORT extern const char ATTRS_COMMON_RELATE_PARENT[];
AX_EXPORT extern const char ATTRS_COMMON_ASPECT_RATIO[];
AX_EXPORT extern const char ATTRS_COMMON_DISPLAY_PRIORITY[];
// border
AX_EXPORT extern const char ATTRS_COMMON_BORDER[];
// background
AX_EXPORT extern const char ATTRS_COMMON_BACKGROUND_COLOR[];
// opacity
AX_EXPORT extern const char ATTRS_COMMON_OPACITY[];
// visibility
AX_EXPORT extern const char ATTRS_COMMON_VISIBILITY[];
// enable
AX_EXPORT extern const char ATTRS_COMMON_ENABLE[];
// zindex
AX_EXPORT extern const char ATTRS_COMMON_ZINDEX[];

// root view
AX_EXPORT extern const char ROOT_ETS_TAG[];
// stage view
AX_EXPORT extern const char STAGE_ETS_TAG[];
// page view
AX_EXPORT extern const char PAGE_ETS_TAG[];
// container modal view (title bar)
AX_EXPORT extern const char CONTAINER_MODAL_ETS_TAG[];

AX_EXPORT extern const char ATOMIC_SERVICE_ETS_TAG[];
AX_EXPORT extern const char APP_BAR_ETS_TAG[];
// js custom view
AX_EXPORT extern const char JS_VIEW_ETS_TAG[];
AX_EXPORT extern const char JS_VIEW_COMPONENT_TAG[];
// js lazy foreach node
AX_EXPORT extern const char JS_LAZY_FOR_EACH_ETS_TAG[];
// js lazy foreach node
AX_EXPORT extern const char JS_FOR_EACH_ETS_TAG[];
// js lazy foreach node
AX_EXPORT extern const char JS_SYNTAX_ITEM_ETS_TAG[];
// js if lese node
AX_EXPORT extern const char JS_IF_ELSE_ETS_TAG[];
// js toast node
AX_EXPORT extern const char TOAST_ETS_TAG[];

// column
AX_EXPORT extern const char COLUMN_COMPONENT_TAG[];
AX_EXPORT extern const char COLUMN_ETS_TAG[];
AX_EXPORT extern const char COLUMN_ATTRS_ALIGN_ITEMS[];

// panel bar
AX_EXPORT extern const char DRAG_BAR_COMPONENT_TAG[];
AX_EXPORT extern const char DRAG_BAR_ETS_TAG[];

// text
AX_EXPORT extern const char TEXT_COMPONENT_TAG[];
AX_EXPORT extern const char TEXT_ETS_TAG[];
AX_EXPORT extern const char TEXT_ATTRS_OVER_FLOW[];
AX_EXPORT extern const char TEXT_ATTRS_MAX_LINES[];

// rich editor
AX_EXPORT extern const char RICH_EDITOR_ETS_TAG[];
AX_EXPORT extern const char RICH_EDITOR_DRAG_ETS_TAG[];

// stack
AX_EXPORT extern const char STACK_COMPONENT_TAG[];
AX_EXPORT extern const char STACK_ETS_TAG[];

// swiper
AX_EXPORT extern const char SWIPER_COMPONENT_TAG[];
AX_EXPORT extern const char SWIPER_ETS_TAG[];
AX_EXPORT extern const char SWIPER_INDICATOR_ETS_TAG[];
AX_EXPORT extern const char SWIPER_LEFT_ARROW_ETS_TAG[];
AX_EXPORT extern const char SWIPER_RIGHT_ARROW_ETS_TAG[];

// tabs
AX_EXPORT extern const char TABS_COMPONENT_TAG[];
AX_EXPORT extern const char TABS_ETS_TAG[];

// tab content item
AX_EXPORT extern const char TAB_CONTENT_ITEM_COMPONENT_TAG[];
AX_EXPORT extern const char TAB_CONTENT_ITEM_ETS_TAG[];

// tab bar
AX_EXPORT extern const char TAB_BAR_COMPONENT_TAG[];
AX_EXPORT extern const char TAB_BAR_ETS_TAG[];

// navigation view
AX_EXPORT extern const char NAVIGATION_VIEW_COMPONENT_TAG[];
AX_EXPORT extern const char NAVIGATION_VIEW_ETS_TAG[];
AX_EXPORT extern const char BAR_ITEM_ETS_TAG[];
AX_EXPORT extern const char TITLE_BAR_ETS_TAG[];
AX_EXPORT extern const char NAVIGATION_CONTENT_ETS_TAG[];
AX_EXPORT extern const char TOOL_BAR_ETS_TAG[];
AX_EXPORT extern const char NAVIGATION_MENU_ETS_TAG[];
AX_EXPORT extern const char BACK_BUTTON_ETS_TAG[];
AX_EXPORT extern const char BACK_BUTTON_IMAGE_ETS_TAG[];
AX_EXPORT extern const char NAVBAR_CONTENT_ETS_TAG[];
AX_EXPORT extern const char NAVBAR_ETS_TAG[];

// navRouter view
AX_EXPORT extern const char NAVROUTER_VIEW_ETS_TAG[];

// navDestination view
AX_EXPORT extern const char NAVDESTINATION_VIEW_ETS_TAG[];
AX_EXPORT extern const char NAVDESTINATION_TITLE_BAR_ETS_TAG[];
AX_EXPORT extern const char NAVDESTINATION_CONTENT_ETS_TAG[];

// navigation title view
AX_EXPORT extern const char NAVIGATION_TITLE_COMPONENT_TAG[];

// navigation menus view
AX_EXPORT extern const char NAVIGATION_MENUS_COMPONENT_TAG[];

// row split
AX_EXPORT extern const char ROW_SPLIT_COMPONENT_TAG[];
AX_EXPORT extern const char ROW_SPLIT_ETS_TAG[];

// column split
AX_EXPORT extern const char COLUMN_SPLIT_COMPONENT_TAG[];
AX_EXPORT extern const char COLUMN_SPLIT_ETS_TAG[];

// counter
AX_EXPORT extern const char COUNTER_COMPONENT_TAG[];
AX_EXPORT extern const char COUNTER_ETS_TAG[];

// flex
AX_EXPORT extern const char FLEX_COMPONENT_TAG[];
AX_EXPORT extern const char FLEX_ETS_TAG[];
AX_EXPORT extern const char WRAP_COMPONENT_TAG[];
AX_EXPORT extern const char WRAP_ETS_TAG[];

// grid
AX_EXPORT extern const char GRID_COMPONENT_TAG[];
AX_EXPORT extern const char GRID_ETS_TAG[];

// grid-item
AX_EXPORT extern const char GRID_ITEM_COMPONENT_TAG[];
AX_EXPORT extern const char GRID_ITEM_ETS_TAG[];

// waterFlow
AX_EXPORT extern const char WATERFLOW_COMPONENT_TAG[];
AX_EXPORT extern const char WATERFLOW_ETS_TAG[];

// flow-item
AX_EXPORT extern const char FLOW_ITEM_COMPONENT_TAG[];
AX_EXPORT extern const char FLOW_ITEM_ETS_TAG[];

// list
AX_EXPORT extern const char LIST_COMPONENT_TAG[];
AX_EXPORT extern const char LIST_ETS_TAG[];

// list-item
AX_EXPORT extern const char LIST_ITEM_COMPONENT_TAG[];
AX_EXPORT extern const char LIST_ITEM_ETS_TAG[];

// list-item
AX_EXPORT extern const char LIST_ITEM_GROUP_COMPONENT_TAG[];
AX_EXPORT extern const char LIST_ITEM_GROUP_ETS_TAG[];

// navigator
AX_EXPORT extern const char NAVIGATOR_COMPONENT_TAG[];
AX_EXPORT extern const char NAVIGATOR_ETS_TAG[];

// panel
AX_EXPORT extern const char PANEL_COMPONENT_TAG[];
AX_EXPORT extern const char PANEL_ETS_TAG[];
AX_EXPORT extern const char PANEL_CLOSE_ICON_ETS_TAG[];

// pattern-lock
AX_EXPORT extern const char PATTERN_LOCK_COMPONENT_TAG[];
AX_EXPORT extern const char PATTERN_LOCK_ETS_TAG[];

// row
AX_EXPORT extern const char ROW_COMPONENT_TAG[];
AX_EXPORT extern const char ROW_ETS_TAG[];

// rect
AX_EXPORT extern const char RECT_ETS_TAG[];

// line
AX_EXPORT extern const char LINE_ETS_TAG[];

// circle
AX_EXPORT extern const char CIRCLE_ETS_TAG[];

// ellipse
AX_EXPORT extern const char ELLIPSE_ETS_TAG[];

// path
AX_EXPORT extern const char PATH_ETS_TAG[];

// polygon
AX_EXPORT extern const char POLYGON_ETS_TAG[];

// polyline
AX_EXPORT extern const char POLYLINE_ETS_TAG[];

// shape
AX_EXPORT extern const char SHAPE_COMPONENT_TAG[];
AX_EXPORT extern const char SHAPE_ETS_TAG[];

// shape container
AX_EXPORT extern const char SHAPE_CONTAINER_COMPONENT_TAG[];
AX_EXPORT extern const char SHAPE_CONTAINER_ETS_TAG[];

// imageAnimator
AX_EXPORT extern const char IMAGE_ANIMATOR_COMPONENT_TAG[];
AX_EXPORT extern const char IMAGE_ANIMATOR_ETS_TAG[];

// image
AX_EXPORT extern const char IMAGE_COMPONENT_TAG[];
AX_EXPORT extern const char IMAGE_ETS_TAG[];

// qrcode
AX_EXPORT extern const char QRCODE_COMPONENT_TAG[];
AX_EXPORT extern const char QRCODE_ETS_TAG[];

// span
AX_EXPORT extern const char SPAN_COMPONENT_TAG[];
AX_EXPORT extern const char SPAN_ETS_TAG[];

// text
AX_EXPORT extern const char TEXT_COMPONENT_TAG[];
AX_EXPORT extern const char TEXT_ETS_TAG[];

// blank
AX_EXPORT extern const char BOX_COMPONENT_TAG[];
AX_EXPORT extern const char BLANK_ETS_TAG[];

// button
AX_EXPORT extern const char BUTTON_COMPONENT_TAG[];
AX_EXPORT extern const char BUTTON_ETS_TAG[];

// recycle
AX_EXPORT extern const char RECYCLE_VIEW_ETS_TAG[];

// common
AX_EXPORT extern const char COMMON_VIEW_ETS_TAG[];
// option
AX_EXPORT extern const char OPTION_COMPONENT_TAG[];
AX_EXPORT extern const char OPTION_ETS_TAG[];

// divider
AX_EXPORT extern const char DIVIDER_COMPONENT_TAG[];
AX_EXPORT extern const char DIVIDER_ETS_TAG[];

// checkbox
AX_EXPORT extern const char CHECKBOX_COMPONENT_TAG[];
AX_EXPORT extern const char CHECKBOX_ETS_TAG[];
AX_EXPORT extern const char CHECK_BOX_COMPONENT_TAG[];
AX_EXPORT extern const char CHECK_BOX_ETS_TAG[];

// switch
AX_EXPORT extern const char SWITCH_COMPONENT_TAG[];
AX_EXPORT extern const char SWITCH_ETS_TAG[];

// toggle
AX_EXPORT extern const char TOGGLE_COMPONENT_TAG[];
AX_EXPORT extern const char TOGGLE_ETS_TAG[];

// scroll
AX_EXPORT extern const char SCROLL_COMPONENT_TAG[];
AX_EXPORT extern const char SCROLL_ETS_TAG[];

// calendar
AX_EXPORT extern const char CALENDAR_COMPONENT_TAG[];
AX_EXPORT extern const char CALENDAR_ETS_TAG[];

// calendarPicker
AX_EXPORT extern const char CALENDAR_PICKER_COMPONENT_TAG[];
AX_EXPORT extern const char CALENDAR_PICKER_ETS_TAG[];

// badge
AX_EXPORT extern const char BADGE_COMPONENT_TAG[];
AX_EXPORT extern const char BADGE_ETS_TAG[];

// search
AX_EXPORT extern const char SEARCH_COMPONENT_TAG[];
AX_EXPORT extern const char SEARCH_ETS_TAG[];

// formComponent
AX_EXPORT extern const char FORM_COMPONENT_TAG[];
AX_EXPORT extern const char FORM_ETS_TAG[];

// formLink
AX_EXPORT extern const char FORM_LINK_TAG[];
AX_EXPORT extern const char FORM_LINK_ETS_TAG[];

// PluginComponent
AX_EXPORT extern const char PLUGIN_COMPONENT_TAG[];
AX_EXPORT extern const char PLUGIN_ETS_TAG[];

// video
AX_EXPORT extern const char VIDEO_COMPONENT_TAG[];
AX_EXPORT extern const char VIDEO_ETS_TAG[];

// AlphabetIndexer
AX_EXPORT extern const char INDEXER_COMPONENT_TAG[];
AX_EXPORT extern const char INDEXER_ETS_TAG[];

// slider
AX_EXPORT extern const char SLIDER_COMPONENT_TAG[];
AX_EXPORT extern const char SLIDER_ETS_TAG[];

// rating
AX_EXPORT extern const char RATING_COMPONENT_TAG[];
AX_EXPORT extern const char RATING_ETS_TAG[];

// progress
AX_EXPORT extern const char PROGRESS_COMPONENT_TAG[];
AX_EXPORT extern const char PROGRESS_ETS_TAG[];

// datapanel
AX_EXPORT extern const char DATA_PANEL_COMPONENT_TAG[];
AX_EXPORT extern const char DATA_PANEL_ETS_TAG[];

// sheet
AX_EXPORT extern const char SHEET_COMPONENT_TAG[];
AX_EXPORT extern const char SHEET_ETS_TAG[];
AX_EXPORT extern const char SHEET_MASK_TAG[];
AX_EXPORT extern const char SHEET_PAGE_TAG[];

// full screen modal
AX_EXPORT extern const char MODAL_PAGE_TAG[];

// hyperlink
AX_EXPORT extern const char HYPERLINK_COMPONENT_TAG[];
AX_EXPORT extern const char HYPERLINK_ETS_TAG[];

// stepper
AX_EXPORT extern const char STEPPER_COMPONENT_TAG[];
AX_EXPORT extern const char STEPPER_ETS_TAG[];

// stepperItem
AX_EXPORT extern const char STEPPER_ITEM_COMPONENT_TAG[];
AX_EXPORT extern const char STEPPER_ITEM_ETS_TAG[];

// scrollBar
AX_EXPORT extern const char SCROLL_BAR_COMPONENT_TAG[];
AX_EXPORT extern const char SCROLL_BAR_ETS_TAG[];
// refresh
AX_EXPORT extern const char REFRESH_COMPONENT_TAG[];
AX_EXPORT extern const char REFRESH_ETS_TAG[];

// datePicker
AX_EXPORT extern const char DATE_PICKER_COMPONENT_TAG[];
AX_EXPORT extern const char DATE_PICKER_ETS_TAG[];
AX_EXPORT extern const char TIME_PICKER_COMPONENT_TAG[];
AX_EXPORT extern const char TIME_PICKER_ETS_TAG[];

// Radio
AX_EXPORT extern const char RADIO_COMPONENT_TAG[];
AX_EXPORT extern const char RADIO_ETS_TAG[];

// gauge
AX_EXPORT extern const char GAUGE_COMPONENT_TAG[];
AX_EXPORT extern const char GAUGE_ETS_TAG[];

// gridContainer
AX_EXPORT extern const char GRIDCONTAINER_COMPONENT_TAG[];
AX_EXPORT extern const char GRIDCONTAINER_ETS_TAG[];

// gridCol
AX_EXPORT extern const char GRID_COL_COMPONENT_TAG[];
AX_EXPORT extern const char GRID_COL_ETS_TAG[];

// gridRow
AX_EXPORT extern const char GRID_ROW_COMPONENT_TAG[];
AX_EXPORT extern const char GRID_ROW_ETS_TAG[];

// relativeContainer
AX_EXPORT extern const char RELATIVE_CONTAINER_COMPONENT_TAG[];
AX_EXPORT extern const char RELATIVE_CONTAINER_ETS_TAG[];

// menu
AX_EXPORT extern const char MENU_COMPONENT_TAG[];
AX_EXPORT extern const char MENU_TAG[];
AX_EXPORT extern const char MENU_ETS_TAG[];
AX_EXPORT extern const char MENU_ITEM_ETS_TAG[];
AX_EXPORT extern const char MENU_ITEM_GROUP_ETS_TAG[];
AX_EXPORT extern const char MENU_WRAPPER_ETS_TAG[];
AX_EXPORT extern const char MENU_PREVIEW_ETS_TAG[];

// textarea
AX_EXPORT extern const char TEXTAREA_COMPONENT_TAG[];
AX_EXPORT extern const char TEXTAREA_ETS_TAG[];

// textdrag
AX_EXPORT extern const char TEXTDRAG_ETS_TAG[];

// textinput
AX_EXPORT extern const char TEXTINPUT_COMPONENT_TAG[];
AX_EXPORT extern const char TEXTINPUT_ETS_TAG[];

// texttimer
AX_EXPORT extern const char TEXTTIMER_COMPONENT_TAG[];
AX_EXPORT extern const char TEXTTIMER_ETS_TAG[];

// select
AX_EXPORT extern const char SELECT_COMPONENT_TAG[];
AX_EXPORT extern const char SELECT_ETS_TAG[];

// select_popup
AX_EXPORT extern const char SELECT_POPUP_COMPONENT_TAG[];
AX_EXPORT extern const char SELECT_POPUP_ETS_TAG[];

// marquee
AX_EXPORT extern const char MARQUEE_COMPONENT_TAG[];
AX_EXPORT extern const char MARQUEE_ETS_TAG[];

// textclock
AX_EXPORT extern const char TEXTCLOCK_COMPONENT_TAG[];
AX_EXPORT extern const char TEXTCLOCK_ETS_TAG[];

// textpicker
AX_EXPORT extern const char TEXT_PICKER_COMPONENT_TAG[];
AX_EXPORT extern const char TEXT_PICKER_ETS_TAG[];

// pickertextdialog
AX_EXPORT extern const char PICKER_TEXT_DIALOG_COMPONENT_TAG[];
AX_EXPORT extern const char PICKER_TEXT_DIALOG_ETS_TAG[];

// canvas
AX_EXPORT extern const char CANVAS_COMPONENT_TAG[];
AX_EXPORT extern const char CANVAS_ETS_TAG[];

// ActionSheet
AX_EXPORT extern const char ACTION_SHEET_DIALOG_COMPONENT_TAG[];
AX_EXPORT extern const char ACTION_SHEET_DIALOG_ETS_TAG[];

// AlertDialog
AX_EXPORT extern const char ALERT_DIALOG_COMPONENT_TAG[];
AX_EXPORT extern const char ALERT_DIALOG_ETS_TAG[];

// CustomDialog
AX_EXPORT extern const char CUSTOM_DIALOG_COMPONENT_TAG[];
AX_EXPORT extern const char CUSTOM_DIALOG_ETS_TAG[];

// datePickerdialog
AX_EXPORT extern const char DATE_PICKER_DIALOG_COMPONENT_TAG[];
AX_EXPORT extern const char DATE_PICKER_DIALOG_ETS_TAG[];

// Normal dialog
AX_EXPORT extern const char DIALOG_COMPONENT_TAG[];
AX_EXPORT extern const char DIALOG_ETS_TAG[];

// sideBar
AX_EXPORT extern const char SIDE_BAR_COMPONENT_TAG[];
AX_EXPORT extern const char SIDE_BAR_ETS_TAG[];

// loadingProgress
AX_EXPORT extern const char LOADING_PROGRESS_COMPONENT_TAG[];
AX_EXPORT extern const char LOADING_PROGRESS_ETS_TAG[];

// checkboxGroup
AX_EXPORT extern const char CHECKBOXGROUP_COMPONENT_TAG[];
AX_EXPORT extern const char CHECKBOXGROUP_ETS_TAG[];

// timePickerdialog
AX_EXPORT extern const char TIME_PICKER_DIALOG_COMPONENT_TAG[];
AX_EXPORT extern const char TIME_PICKER_DIALOG_ETS_TAG[];

// web
AX_EXPORT extern const char WEB_COMPONENT_TAG[];
AX_EXPORT extern const char WEB_ETS_TAG[];
AX_EXPORT extern const char WEB_CORE_TAG[];

// richText
AX_EXPORT extern const char RICH_TEXT_COMPONENT_TAG[];
AX_EXPORT extern const char RICH_TEXT_ETS_TAG[];

// xcomponent
AX_EXPORT extern const char XCOMPONENT_TAG[];
AX_EXPORT extern const char XCOMPONENT_ETS_TAG[];

// ability_component
AX_EXPORT extern const char ABILITY_COMPONENT_ETS_TAG[];
// Popup
AX_EXPORT extern const char POPUP_ETS_TAG[];

// remote window
AX_EXPORT extern const char REMOTE_WINDOW_ETS_TAG[];

// effect view
AX_EXPORT extern const char EFFECT_COMPONENT_ETS_TAG[];

// button
AX_EXPORT extern const char MODEL_COMPONENT_TAG[];
AX_EXPORT extern const char MODEL_ETS_TAG[];

// window scene
AX_EXPORT extern const char ROOT_SCENE_ETS_TAG[];
AX_EXPORT extern const char SCREEN_ETS_TAG[];
AX_EXPORT extern const char WINDOW_SCENE_ETS_TAG[];

// ui_extension_component
AX_EXPORT extern const char UI_EXTENSION_COMPONENT_TAG[];
AX_EXPORT extern const char UI_EXTENSION_SURFACE_TAG[];
AX_EXPORT extern const char UI_EXTENSION_COMPONENT_ETS_TAG[];

// LocationButton
AX_EXPORT extern const char LOCATION_BUTTON_COMPONENT_TAG[];
AX_EXPORT extern const char LOCATION_BUTTON_ETS_TAG[];

// PasteButton
AX_EXPORT extern const char PASTE_BUTTON_COMPONENT_TAG[];
AX_EXPORT extern const char PASTE_BUTTON_ETS_TAG[];

// SaveButton
AX_EXPORT extern const char SAVE_BUTTON_COMPONENT_TAG[];
AX_EXPORT extern const char SAVE_BUTTON_ETS_TAG[];

// Keyboard
AX_EXPORT extern const char KEYBOARD_ETS_TAG[];
// Particle
AX_EXPORT extern const char PARTICLE_COMPONENT_TAG[];
AX_EXPORT extern const char PARTICLE_ETS_TAG[];

// SelectOverlay
AX_EXPORT extern const char SELECT_OVERLAY_ETS_TAG[];
}  // namespace ui
#endif  // UI_ACCESSIBILITY_PLATFORM_AX_OHOS_CONSTANTS_H_
