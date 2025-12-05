// Copyright (c) 2012 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the CEF translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef ARKWEB_INCLUDE_CEF_DIALOG_HANDLER_EXT_H_
#define ARKWEB_INCLUDE_CEF_DIALOG_HANDLER_EXT_H_
#pragma once

#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_dialog_handler.h"

#if BUILDFLAG(IS_ARKWEB)
///
/// Callback interface for asynchronous continuation of <select> selection.
///

class CefSelectPopupCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Continue the <select> selection. |indices| should be the 0-based array
  /// index of the value selected from the <select> array passed to
  /// CefDialogHandler::ShowSelectPopup.
  ///

  virtual void Continue(const std::vector<int>& indices) = 0;

  ///
  /// Cancel <select> selection.
  ///

  virtual void Cancel() = 0;
};

///
/// Callback interface for asynchronous continuation of datetime chooser.
///

class CefDateTimeChooserCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Notify the date time chooser result.
  ///

  virtual void Continue(bool success, double dialog_value) = 0;
};
#endif  // BUILDFLAG(IS_ARKWEB)

class CefDialogHandlerExt : public virtual CefDialogHandler {
 public:
  ///
  /// Called to run a file chooser dialog. |mode| represents the type of dialog
  /// to display. |title| to the title to be used for the dialog and may be
  /// empty to show the default title ("Open" or "Save" depending on the mode).
  /// |default_file_path| is the path with optional directory and/or file name
  /// component that should be initially selected in the dialog.
  /// |accept_filters| are used to restrict the selectable file types and may be
  /// any combination of valid lower-cased MIME types (e.g. "text/*" or
  /// "image/*") and individual file extensions (e.g. ".txt" or ".png").
  /// |accept_extensions| provides the semicolon-delimited expansion of MIME
  /// types to file extensions (if known, or empty string otherwise).
  /// |accept_descriptions| provides the descriptions for MIME types (if known,
  /// or empty string otherwise). For example, the "image/*" mime type might
  /// have extensions ".png;.jpg;.bmp;..." and description "Image Files".
  /// |accept_filters|, |accept_extensions| and |accept_descriptions| will all
  /// be the same size. To display a custom dialog return true and execute
  /// |callback| either inline or at a later time. To display the default dialog
  /// return false. If this method returns false it may be called an additional
  /// time for the same dialog (both before and after MIME type expansion).
  ///

#if BUILDFLAG(IS_ARKWEB)
  ///
  /// Show <select> popup menu.
  ///

  virtual void OnSelectPopupMenu(
      CefRefPtr<CefBrowser> browser,
      const CefRect& bounds,
      double item_font_size,
      int selected_item,
      const std::vector<CefSelectPopupItem>& menu_items,
      bool right_aligned,
      bool allow_multiple_selection,
      CefRefPtr<CefSelectPopupCallback> callback) {}

  ///
  /// Show <input type="date"> or <input type="datatime">
  ///
  virtual void OnDateTimeChooserPopup(
      CefRefPtr<CefBrowser> browser,
      const CefDateTimeChooser& date_time_chooser,
      const std::vector<CefDateTimeSuggestion>& suggestion,
      CefRefPtr<CefDateTimeChooserCallback> callback) {}

  ///
  /// Close date chooser popup.
  ///
  virtual void OnDateTimeChooserClose() {}

  ///
  /// hide password Autofill popup menu.
  ///

  virtual void OnHideAutofillPopup() {}

  ///
  /// Show adblock num.
  ///

  virtual void OnAdsBlocked(
      CefRefPtr<CefBrowser> browser,
      const CefString& main_frame_url,
      const std::map<CefString, CefString>& subresource_blocked,
      bool is_site_first_report) {}

  ///
  /// Show password Autofill popup menu.
  ///

  virtual void OnShowAutofillPopup(
      CefRefPtr<CefBrowser> browser,
      const CefRect& bounds,
      bool right_aligned,
      const std::vector<CefAutofillPopupItem>& menu_items,
      bool is_password_popup_type) {}

  ///
  /// Called when notify ui to show password dialog to query user to save
  /// password.
  ///

  virtual void ShowPasswordDialog(bool is_update, const CefString& url) {}

  ///
  /// trigger adblock switch from ui.
  ///
  /*--cef()--*/
  virtual bool TrigAdBlockEnabledForSiteFromUi(CefRefPtr<CefBrowser> browser,
                                               const CefString& main_frame_url,
                                               int main_frame_tree_node_id) {
    return false;
  }

  ///
  /// Handles the picture in picture event.
  ///
  /*--cef()--*/
  virtual void OnPipEvent(CefRefPtr<CefBrowser> browser,
                          int event) {}

  CefRefPtr<CefDialogHandlerExt> AsArkDialogHandler() override { return this; }
#endif  // BUILDFLAG(IS_ARKWEB)
};
#endif
