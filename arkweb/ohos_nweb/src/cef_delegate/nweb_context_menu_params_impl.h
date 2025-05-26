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

#ifndef NWEB_CONTEXT_MENU_PARAMS_IMPL_H
#define NWEB_CONTEXT_MENU_PARAMS_IMPL_H

#include "cef/include/cef_context_menu_handler.h"
#include "nweb_context_menu_params.h"

namespace OHOS::NWeb {
class NWebContextMenuParamsImpl : public NWebContextMenuParams {
 public:
  explicit NWebContextMenuParamsImpl(CefRefPtr<CefContextMenuParams> params,
                                     float virutal_device_ratio,
                                     int32_t view_port_height);
  int32_t GetXCoord() override;
  int32_t GetYCoord() override;
  int32_t GetContextMenuTypeFlags() override;
  std::string GetLinkUrl() override;
  std::string GetUnfilteredLinkUrl() override;
  std::string GetSourceUrl() override;
  bool HasImageContents() override;
  std::string GetTitleText() override;
  std::string GetPageUrl() override;
  ContextMenuMediaType GetMediaType() override;
  bool IsEditable() override;
  int32_t GetEditStateFlags() override;
  ContextMenuInputFieldType GetInputFieldType() override;
  ContextMenuSourceType GetSourceType() override;
  std::string GetSelectionText() override;
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void GetImageRect(int& x, int& y, int& w, int& h) override;
  bool IsAILink() override;
#endif

 private:
  CefRefPtr<CefContextMenuParams> params_;
  float virutal_device_ratio_ = 1.0;
  int32_t view_port_height_ = 0;
};

class NWebQuickMenuParamsImpl : public NWebQuickMenuParams {
 public:
  NWebQuickMenuParamsImpl(int32_t x,
                          int32_t y,
                          int32_t width,
                          int32_t height,
                          int32_t flags,
                          int32_t select_x,
                          int32_t select_y,
                          int32_t select_width,
                          int32_t select_height);
  int32_t GetXCoord() override;
  int32_t GetYCoord() override;
  int32_t GetWidth() override;
  int32_t GetHeight() override;
  int32_t GetEditStateFlags() override;
  int32_t GetSelectX() override;
  int32_t GetSelectY() override;
  int32_t GetSelectWidth() override;
  int32_t GetSelectXHeight() override;
  std::shared_ptr<NWebTouchHandleState> GetTouchHandleState(
      NWebTouchHandleState::TouchHandleType type) override;
  void SetTouchHandleState(std::shared_ptr<NWebTouchHandleState> state,
                           NWebTouchHandleState::TouchHandleType type);
  void SetIsMouseTrigger(bool is_mouse_trigger);
  bool GetIsLongPressActived() override;
  void SetIsLongPressActived(bool is_long_press_actived);

 private:
  int32_t x_;
  int32_t y_;
  int32_t width_;
  int32_t height_;
  int32_t edit_flags_;
  int32_t select_x_;
  int32_t select_y_;
  int32_t select_width_;
  int32_t select_height_;
  std::shared_ptr<NWebTouchHandleState> insert_touch_handle_state_;
  std::shared_ptr<NWebTouchHandleState> begin_touch_handle_state_;
  std::shared_ptr<NWebTouchHandleState> end_touch_handle_state_;
  bool is_mouse_trigger_ = false;
  bool is_long_press_actived_ = false;
};

class NWebContextMenuCallbackImpl : public NWebContextMenuCallback {
 public:
  explicit NWebContextMenuCallbackImpl(
      CefRefPtr<CefRunContextMenuCallback> callback);
  void Continue(int32_t commandId, MenuEventFlags flag) override;
  void Cancel() override;

 private:
  CefRefPtr<CefRunContextMenuCallback> callback_;
};

class NWebQuickMenuCallbackImpl : public NWebQuickMenuCallback {
 public:
  explicit NWebQuickMenuCallbackImpl(
      CefRefPtr<CefRunQuickMenuCallback> callback);
  void Continue(int32_t commandId, MenuEventFlags flag) override;
  void Cancel() override;

 private:
  CefRefPtr<CefRunQuickMenuCallback> callback_;
};
}  // namespace OHOS::NWeb

#endif
