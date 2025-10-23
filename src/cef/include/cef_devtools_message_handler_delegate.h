/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef CEF_INCLUDE_CEF_DEVTOOLS_MESSAGE_HANDLER_DELEGATE_H_
#define CEF_INCLUDE_CEF_DEVTOOLS_MESSAGE_HANDLER_DELEGATE_H_
#pragma once

#include "include/cef_base.h"
#include "include/cef_dialog_handler.h"

///
/// Callback interface for info bar.
///
/*--cef(source=library)--*/
class CefInfoBarCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Allow to continue or not.
  ///
  /*--cef()--*/
  virtual void Allow(bool allow) = 0;
};

///
/// Implement this interface to handle devtools message.
///
/*--cef(source=client)--*/
class CefDevToolsMessageHandlerDelegate : public virtual CefBaseRefCounted {
 public:
  typedef cef_file_dialog_mode_t FileDialogMode;

  ///
  /// Called when devtools request to open file chooser.
  ///
  /*--cef(optional_param=title,optional_param=default_file_path,
        optional_param=accept_filters)--*/
  virtual bool ShowFileChooser(
      FileDialogMode mode,
      const CefString& title,
      const CefString& default_file_path,
      const std::vector<CefString>& accept_filters,
      bool capture,
      CefRefPtr<CefFileDialogCallback> callback) {
    return false;
  }

  ///
  /// Called when devtools request to show info bar.
  ///
  /*--cef()--*/
  virtual void ShowInfoBar(
      const CefString& message,
      const CefString& path,
      CefRefPtr<CefInfoBarCallback> callback) {}

  ///
  /// Called when devtools request to display in front.
  ///
  /*--cef()--*/
  virtual bool BringToFront() { return false; }
  ///
  /// Called when devtools request to close browser window.
  ///
  /*--cef()--*/
  virtual bool CloseWindow() { return false; }
  ///
  /// Called when devtools request to display in front.
  ///
  /*--cef()--*/
  virtual bool ActiveDevToolsWindow() { return false; }
};

#endif  // CEF_INCLUDE_CEF_DEVTOOLS_MESSAGE_HANDLER_DELEGATE_H_
