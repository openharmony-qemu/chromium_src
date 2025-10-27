// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_INCLUDE_CEF_FORM_HANDLER_H_
#define CEF_INCLUDE_CEF_FORM_HANDLER_H_
#pragma once

#include "include/cef_base.h"
#include "include/cef_browser.h"

///
/// Implement this interface to handle events related to browser form status.
/// The methods of this class will be called on the browser process UI thread or
/// render process main thread (TID_RENDERER).
///
/*--cef(source=client)--*/
class CefFormHandler : public virtual CefBaseRefCounted {
 public:
  ///
  /// Called when the user edited.
  ///
  /*--cef()--*/
  virtual void OnFormEditingStateChanged(CefRefPtr<CefBrowser> browser,
                                         bool is_editing,
                                         uint64_t form_id) {}
};

#endif  // CEF_INCLUDE_CEF_FORM_HANDLER_H_
