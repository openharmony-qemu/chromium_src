/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "nweb_context_menu_params_impl.h"

#include <unordered_map>

#include "nweb_context_menu_params.h"

using namespace OHOS::NWeb;

namespace {
using CmTf = NWebContextMenuParams::ContextMenuTypeFlags;
using CmMt = NWebContextMenuParams::ContextMenuMediaType;
using CmEf = NWebContextMenuParams::ContextMenuEditStateFlags;
using CmIt = NWebContextMenuParams::ContextMenuInputFieldType;
using CmSt = NWebContextMenuParams::ContextMenuSourceType;
using QmEf = NWebQuickMenuParams::QuickMenuEditStateFlags;
const std::unordered_map<int, int> kCmTypeFlagMap = {
    {CM_TYPEFLAG_NONE, CmTf::CM_TF_NONE},
    {CM_TYPEFLAG_PAGE, CmTf::CM_TF_PAGE},
    {CM_TYPEFLAG_FRAME, CmTf::CM_TF_FRAME},
    {CM_TYPEFLAG_LINK, CmTf::CM_TF_LINK},
    {CM_TYPEFLAG_MEDIA, CmTf::CM_TF_MEDIA},
    {CM_TYPEFLAG_SELECTION, CmTf::CM_TF_SELECTION},
    {CM_TYPEFLAG_EDITABLE, CmTf::CM_TF_EDITABLE},
};

const std::unordered_map<int, int> kCmMediaTypeMap = {
    {CM_MEDIATYPE_NONE, CmMt::CM_MT_NONE},
    {CM_MEDIATYPE_IMAGE, CmMt::CM_MT_IMAGE},
    {CM_MEDIATYPE_VIDEO, CmMt::CM_MT_VIDEO},
    {CM_MEDIATYPE_AUDIO, CmMt::CM_MT_AUDIO},
    {CM_MEDIATYPE_FILE, CmMt::CM_MT_NONE},
    {CM_MEDIATYPE_PLUGIN, CmMt::CM_MT_NONE},
};

const std::unordered_map<int, int> kCmEditStateFlagsMap = {
    {CM_EDITFLAG_NONE, CmEf::CM_ES_NONE},
    {CM_EDITFLAG_CAN_CUT, CmEf::CM_ES_CAN_CUT},
    {CM_EDITFLAG_CAN_COPY, CmEf::CM_ES_CAN_COPY},
    {CM_EDITFLAG_CAN_PASTE, CmEf::CM_ES_CAN_PASTE},
    {CM_EDITFLAG_CAN_SELECT_ALL, CmEf::CM_ES_CAN_SELECT_ALL},
};

const std::unordered_map<int, int> kQmEditStateFlagsMap = {
    {QM_EDITFLAG_NONE, QmEf::QM_EF_NONE},
    {QM_EDITFLAG_CAN_ELLIPSIS, QmEf::QM_EF_CAN_ELLIPSIS},
    {QM_EDITFLAG_CAN_CUT, QmEf::QM_EF_CAN_CUT},
    {QM_EDITFLAG_CAN_COPY, QmEf::QM_EF_CAN_COPY},
    {QM_EDITFLAG_CAN_PASTE, QmEf::QM_EF_CAN_PASTE},
    {QM_EDITFLAG_CAN_SELECT_ALL, QmEf::QM_EF_CAN_SELECT_ALL},
};

const std::unordered_map<int, int> KMenuEventFlagsMap = {
    {EF_NONE, EVENTFLAG_NONE},
    {EF_CAPS_LOCK_ON, EVENTFLAG_CAPS_LOCK_ON},
    {EF_SHIFT_DOWN, EVENTFLAG_SHIFT_DOWN},
    {EF_CONTROL_DOWN, EVENTFLAG_CONTROL_DOWN},
    {EF_ALT_DOWN, EVENTFLAG_ALT_DOWN},
    {EF_LEFT_MOUSE_BUTTON, EVENTFLAG_LEFT_MOUSE_BUTTON},
    {EF_MIDDLE_MOUSE_BUTTON, EVENTFLAG_MIDDLE_MOUSE_BUTTON},
    {EF_RIGHT_MOUSE_BUTTON, EVENTFLAG_RIGHT_MOUSE_BUTTON},
};

const std::unordered_map<int32_t, cef_menu_id_t> KMenuCommandIdMap = {
    {CI_IMAGE_COPY, MENU_ID_IMAGE_COPY},
    {CI_CUT, MENU_ID_CUT},
    {CI_COPY, MENU_ID_COPY},
    {CI_PASTE, MENU_ID_PASTE},
    {CI_DELETE, MENU_ID_DELETE},
    {CI_SELECT_ALL, MENU_ID_SELECT_ALL},
    {CI_REDO, MENU_ID_REDO},
    {CI_UNDO, MENU_ID_UNDO},
    {CI_PASTE_AND_MATCH_STYLE, MENU_ID_PASTE_MATCH_STYLE},
};

const std::unordered_map<int, int> kCmInputFieldTypeMap = {
    {CM_INPUTFIELDTYPE_NONE, CmIt::CM_IT_NONE},
    {CM_INPUTFIELDTYPE_PLAINTEXT, CmIt::CM_IT_PLAINTEXT},
    {CM_INPUTFIELDTYPE_PASSWORD, CmIt::CM_IT_PASSWORD},
    {CM_INPUTFIELDTYPE_NUMBER, CmIt::CM_IT_NUMBER},
    {CM_INPUTFIELDTYPE_TELEPHONE, CmIt::CM_IT_TELEPHONE},
    {CM_INPUTFIELDTYPE_OTHER, CmIt::CM_IT_OTHER},
};

const std::unordered_map<int, int> kCmSourceTypeMap = {
    {CM_SOURCETYPE_NONE, CmSt::CM_ST_NONE},
    {CM_SOURCETYPE_MOUSE, CmSt::CM_ST_MOUSE},
    {CM_SOURCETYPE_KEYBOARD, CmSt::CM_ST_NONE},
    {CM_SOURCETYPE_TOUCH, CmSt::CM_ST_NONE},
    {CM_SOURCETYPE_TOUCH_EDIT_MENU, CmSt::CM_ST_NONE},
    {CM_SOURCETYPE_LONG_PRESS, CmSt::CM_ST_LONG_PRESS},
    {CM_SOURCETYPE_LONG_TAP, CmSt::CM_ST_NONE},
    {CM_SOURCETYPE_TOUCH_HANDLE, CmSt::CM_ST_NONE},
    {CM_SOURCETYPE_STYLUS, CmSt::CM_ST_NONE},
    {CM_SOURCETYPE_ADJUST_SELECTION, CmSt::CM_ST_NONE},
    {CM_SOURCETYPE_SELECTION_RESET, CmSt::CM_ST_NONE},
};

CmIt ConvertContextMenuInputFieldType(
    CefContextMenuParamsExt::InputFieldType value) {
  std::unordered_map<int, int>::const_iterator iter =
      kCmInputFieldTypeMap.find(static_cast<int32_t>(value));
  if (iter != kCmInputFieldTypeMap.end()) {
    return static_cast<CmIt>(iter->second);
  }
  return CmIt::CM_IT_NONE;
}

CmSt ConvertContextMenuSourceType(CefContextMenuParamsExt::SourceType value) {
  std::unordered_map<int, int>::const_iterator iter =
      kCmSourceTypeMap.find(static_cast<int32_t>(value));
  if (iter != kCmSourceTypeMap.end()) {
    return static_cast<CmSt>(iter->second);
  }
  return CmSt::CM_ST_NONE;
}

cef_menu_id_t ConvertCommandId(int32_t id) {
  std::unordered_map<int32_t, cef_menu_id_t>::const_iterator iter =
      KMenuCommandIdMap.find(id);
  if (iter != KMenuCommandIdMap.end()) {
    return iter->second;
  }
  return MENU_ID_CUSTOM_FIRST;
}

int32_t ConvertMenuFlags(int32_t value,
                         const std::unordered_map<int, int>& flags_map) {
  int32_t result = 0;
  for (auto& kv : flags_map) {
    if ((kv.first & value) != 0) {
      result |= kv.second;
    }
  }
  return result;
}

CmMt ConvertContextMenuMediaType(CefContextMenuParamsExt::MediaType value) {
  std::unordered_map<int, int>::const_iterator iter =
      kCmMediaTypeMap.find(static_cast<int32_t>(value));
  if (iter != kCmMediaTypeMap.end()) {
    return static_cast<CmMt>(iter->second);
  }
  return CmMt::CM_MT_NONE;
}
}  // namespace

namespace OHOS::NWeb {
NWebContextMenuParamsImpl::NWebContextMenuParamsImpl(
    CefRefPtr<CefContextMenuParams> params,
    float virutal_device_ratio,
    int32_t view_port_height)
    : params_(params),
      virutal_device_ratio_(virutal_device_ratio),
      view_port_height_(view_port_height) {}

int32_t NWebContextMenuParamsImpl::GetXCoord() {
  if (params_ != nullptr) {
    return params_->GetXCoord() * virutal_device_ratio_;
  }
  return -1;
}

int32_t NWebContextMenuParamsImpl::GetYCoord() {
  if (params_ != nullptr) {
    return params_->GetYCoord() * virutal_device_ratio_;
    ;
  }
  return -1;
}

int32_t NWebContextMenuParamsImpl::GetContextMenuTypeFlags() {
  if (params_ != nullptr) {
    return ConvertMenuFlags(params_->GetTypeFlags(), kCmTypeFlagMap);
  }
  return 0;
}

std::string NWebContextMenuParamsImpl::GetLinkUrl() {
  if (params_ != nullptr) {
    return params_->GetLinkUrl().ToString();
  }
  return std::string();
}

std::string NWebContextMenuParamsImpl::GetUnfilteredLinkUrl() {
  if (params_ != nullptr) {
    return params_->GetUnfilteredLinkUrl().ToString();
  }
  return std::string();
}

std::string NWebContextMenuParamsImpl::GetSourceUrl() {
  if (params_ != nullptr) {
    return params_->GetSourceUrl().ToString();
  }
  return std::string();
}

bool NWebContextMenuParamsImpl::HasImageContents() {
  if (params_ != nullptr) {
    return params_->HasImageContents();
  }
  return false;
}

std::string NWebContextMenuParamsImpl::GetTitleText() {
  if (params_ != nullptr) {
    return params_->GetTitleText().ToString();
  }
  return std::string();
}

std::string NWebContextMenuParamsImpl::GetPageUrl() {
  if (params_ != nullptr) {
    return params_->GetPageUrl().ToString();
  }
  return std::string();
}

CmMt NWebContextMenuParamsImpl::GetMediaType() {
  if (params_ != nullptr) {
    return ConvertContextMenuMediaType(params_->GetMediaType());
  }
  return CmMt::CM_MT_NONE;
}

bool NWebContextMenuParamsImpl::IsEditable() {
  if (params_ != nullptr) {
    return params_->IsEditable();
  }

  return false;
}

int32_t NWebContextMenuParamsImpl::GetEditStateFlags() {
  if (params_ != nullptr) {
    return ConvertMenuFlags(params_->GetEditStateFlags(), kCmEditStateFlagsMap);
  }
  return 0;
}

NWebQuickMenuParamsImpl::NWebQuickMenuParamsImpl(int32_t x,
                                                 int32_t y,
                                                 int32_t width,
                                                 int32_t height,
                                                 int32_t flags,
                                                 int32_t select_x,
                                                 int32_t select_y,
                                                 int32_t select_width,
                                                 int32_t select_height)
    : x_(x),
      y_(y),
      width_(width),
      height_(height),
      edit_flags_(ConvertMenuFlags(flags, kQmEditStateFlagsMap)),
      select_x_(select_x),
      select_y_(select_y),
      select_width_(select_width),
      select_height_(select_height) {}

int32_t NWebQuickMenuParamsImpl::GetXCoord() {
  return x_;
}

int32_t NWebQuickMenuParamsImpl::GetYCoord() {
  return y_;
}

int32_t NWebQuickMenuParamsImpl::GetWidth() {
  return width_;
}

int32_t NWebQuickMenuParamsImpl::GetHeight() {
  return height_;
}

int32_t NWebQuickMenuParamsImpl::GetEditStateFlags() {
  return edit_flags_;
}

int32_t NWebQuickMenuParamsImpl::GetSelectX() {
  return select_x_;
}

int32_t NWebQuickMenuParamsImpl::GetSelectY() {
  return select_y_;
}

int32_t NWebQuickMenuParamsImpl::GetSelectWidth() {
  return select_width_;
}

int32_t NWebQuickMenuParamsImpl::GetSelectXHeight() {
  return select_height_;
}

void NWebQuickMenuParamsImpl::SetIsMouseTrigger(bool is_mouse_trigger) {
  is_mouse_trigger_ = is_mouse_trigger;
}

void NWebQuickMenuParamsImpl::SetIsLongPressActived(
    bool is_long_press_actived) {
  is_long_press_actived_ = is_long_press_actived;
}

bool NWebQuickMenuParamsImpl::GetIsLongPressActived() {
  return is_long_press_actived_;
}

std::shared_ptr<NWebTouchHandleState>
NWebQuickMenuParamsImpl::GetTouchHandleState(
    NWebTouchHandleState::TouchHandleType type) {
  switch (type) {
    case NWebTouchHandleState::TouchHandleType::INSERT_HANDLE:
      return insert_touch_handle_state_;
    case NWebTouchHandleState::TouchHandleType::SELECTION_BEGIN_HANDLE:
      return begin_touch_handle_state_;
    case NWebTouchHandleState::TouchHandleType::SELECTION_END_HANDLE:
      return end_touch_handle_state_;
    default:
      return nullptr;
  }
}

void NWebQuickMenuParamsImpl::SetTouchHandleState(
    std::shared_ptr<NWebTouchHandleState> state,
    NWebTouchHandleState::TouchHandleType type) {
  switch (type) {
    case NWebTouchHandleState::TouchHandleType::INSERT_HANDLE:
      insert_touch_handle_state_ = state;
      break;
    case NWebTouchHandleState::TouchHandleType::SELECTION_BEGIN_HANDLE:
      begin_touch_handle_state_ = state;
      break;
    case NWebTouchHandleState::TouchHandleType::SELECTION_END_HANDLE:
      end_touch_handle_state_ = state;
      break;
    default:
      return;
  }
}

NWebContextMenuCallbackImpl::NWebContextMenuCallbackImpl(
    CefRefPtr<CefRunContextMenuCallback> callback)
    : callback_(callback) {}

void NWebContextMenuCallbackImpl::Continue(int32_t commandId,
                                           MenuEventFlags flag) {
  if (callback_ != nullptr) {
    int32_t event_flag =
        ConvertMenuFlags(static_cast<int32_t>(flag), KMenuEventFlagsMap);
    callback_->Continue(ConvertCommandId(commandId),
                        static_cast<cef_event_flags_t>(event_flag));
  }
}

void NWebContextMenuCallbackImpl::Cancel() {
  if (callback_ != nullptr) {
    callback_->Cancel();
  }
}

NWebQuickMenuCallbackImpl::NWebQuickMenuCallbackImpl(
    CefRefPtr<CefRunQuickMenuCallback> callback)
    : callback_(callback) {}

void NWebQuickMenuCallbackImpl::Continue(int32_t commandId,
                                         MenuEventFlags flag) {
  if (callback_ != nullptr) {
    int32_t event_flag =
        ConvertMenuFlags(static_cast<int32_t>(flag), KMenuEventFlagsMap);
    callback_->Continue(commandId, static_cast<cef_event_flags_t>(event_flag));
  }
}

void NWebQuickMenuCallbackImpl::Cancel() {
  if (callback_ != nullptr) {
    callback_->Cancel();
  }
}

CmIt NWebContextMenuParamsImpl::GetInputFieldType() {
  if (params_ != nullptr) {
    return ConvertContextMenuInputFieldType(
        params_->AsCefCefContextMenuParamsExt()->GetInputFieldType());
  }
  return CmIt::CM_IT_NONE;
}

CmSt NWebContextMenuParamsImpl::GetSourceType() {
  if (params_ != nullptr) {
    return ConvertContextMenuSourceType(
        params_->AsCefCefContextMenuParamsExt()->GetSourceType());
  }
  return CmSt::CM_ST_NONE;
}

std::string NWebContextMenuParamsImpl::GetSelectionText() {
  if (params_ != nullptr) {
    return params_->GetSelectionText().ToString();
  }
  return std::string();
}

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void NWebContextMenuParamsImpl::GetImageRect(int& x, int& y, int& w, int& h) {
  if (params_ != nullptr) {
    params_->AsCefCefContextMenuParamsExt()->GetImageRect(x, y, w, h);
    y += view_port_height_ * virutal_device_ratio_;
  }
}

bool NWebContextMenuParamsImpl::IsAILink() {
  if (params_ != nullptr) {
    return params_->AsCefCefContextMenuParamsExt()->IsAILink();
  }
  return false;
}
#endif

}  // namespace OHOS::NWeb
