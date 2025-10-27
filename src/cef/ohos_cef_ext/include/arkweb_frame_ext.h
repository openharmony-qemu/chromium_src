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

#ifndef CEF_INCLUDE_CEF_FRAME_EXT_H_
#define CEF_INCLUDE_CEF_FRAME_EXT_H_
#pragma once

#include "include/cef_base.h"
#include "include/cef_dom.h"
#include "include/cef_frame.h"
#include "include/cef_process_message.h"
#include "include/cef_request.h"
#include "include/cef_stream.h"
#include "include/cef_string_visitor.h"
class CefBrowser;
class CefURLRequest;
class CefURLRequestClient;
class CefV8Context;

#if BUILDFLAG(IS_OHOS)
///
/// Interface to implement to be notified of asynchronous completion via
/// CefFrameHostImpl::GetImages.
///
/*--cef(source=client)--*/
class CefGetImagesCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon completion. |num_deleted| will be the
  /// number of cookies that were deleted.
  ///
  /*--cef()--*/
  virtual void GetImages(bool response) = 0;
};
#endif  // BUILDFLAG(IS_OHOS)

class ArkwebFrameExt : public virtual CefBaseRefCounted {
 public:
  ///
  /// Loads the given URL with additional HTTP headers, specified as a map
  /// from name to value. Note that if this map contains any of the headers that
  /// are set by default by this WebView, such as those controlling caching,
  /// accept types or the User-Agent, their values may be overridden by this
  /// WebView's defaults.
  ///
  /*--cef(optional_param=url, optional_param=additionalHttpHeaders)--*/
  virtual void LoadHeaderUrl(const CefString& url,
                             const CefString& additionalHttpHeaders) = 0;

  ///
  /// web has image or not
  ///
  /*--cef()--*/
  virtual void GetImages(CefRefPtr<CefGetImagesCallback> callback) = 0;

  ///
  /// PostUrl
  ///
  /*--cef(optional_param=post_data)--*/
  virtual void PostURL(const CefString& url,
                       const std::vector<char>& post_data) = 0;

  ///
  /// LoadURLWithUserGesture
  ///
  /*--cef()--*/
  virtual void LoadURLWithUserGesture(const CefString& url,
                                      bool user_gesture) = 0;

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  virtual void SetJsOnlineProperty(bool network_up) = 0;
#endif

#if BUILDFLAG(ARKWEB_SYNC_RENDER)

  ///
  /// UpdateDrawRect
  ///
  /*--cef()--*/
  virtual void UpdateDrawRect() = 0;
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  virtual void SendBlanklessKeyToRenderFrame(uint32_t nweb_id,
                                             uint64_t blankless_key,
                                             uint64_t frame_sink_id,
                                             int64_t pref_hash) = 0;
#endif
};
#endif  // CEF_INCLUDE_CEF_FRAME_H_
