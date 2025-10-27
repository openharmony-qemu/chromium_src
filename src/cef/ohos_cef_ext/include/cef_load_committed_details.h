// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
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

#ifndef CEF_INCLUDE_CEF_LOAD_COMMITTED_DETAILS_H_
#define CEF_INCLUDE_CEF_LOAD_COMMITTED_DETAILS_H_
#pragma once

#include "include/cef_base.h"

///
/// Class used to represent the details of a committed navigation entry.
///
/*--cef(source=library)--*/
class CefLoadCommittedDetails : public virtual CefBaseRefCounted {
 public:
  typedef cef_navigation_entry_type_t NavigationType;

  ///
  /// Returns true when the main frame was navigated. False means the
  /// navigation was a sub-frame.
  ///
  /*--cef()--*/
  virtual bool IsMainFrame() = 0;

  ///
  /// Returns true if the navigation haapped without changing document.
  /// Example of same document navigations are:
  /// * reference fragment navigations
  /// * pushState/replaceState
  /// * same page history navigation
  ///
  /*--cef()--*/
  virtual bool IsSameDocument() = 0;

  ///
  /// Returns true if the committed entry has replaced the existing one.
  ///
  /*--cef()--*/
  virtual bool DidReplaceEntry() = 0;

  ///
  /// Returns the type of navigation that just occurred.
  ///
  /*--cef(default_retval=NAVIGATION_TYPE_UNKNOWN)--*/
  virtual NavigationType GetNavigationType() = 0;

  ///
  /// Returns the url of the current navigation.
  ///
  /*--cef()--*/
  virtual CefString GetCurrentURL() = 0;
};

#endif  // CEF_INCLUDE_CEF_LOAD_COMMITTED_DETAILS_H_
