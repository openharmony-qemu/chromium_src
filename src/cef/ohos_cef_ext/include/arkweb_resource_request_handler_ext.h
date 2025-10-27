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

#ifndef OHOS_CEF_EXT_INCLUDE_ARKWEB_RESOURCE_REQUEST_HANDLER_EXT_H_
#define OHOS_CEF_EXT_INCLUDE_ARKWEB_RESOURCE_REQUEST_HANDLER_EXT_H_
#pragma once

#include "arkweb_request_ext.h"
#include "include/cef_resource_request_handler.h"
#include "include/cef_scheme.h"

///
/// CefInterceptCallback
///
class CefInterceptCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Continue.
  ///
  virtual void ContinueLoad(CefRefPtr<CefResourceHandler> resource_handler) {}
};

///
/// Extended from CefRequest
///
class ArkWebResourceRequestHandlerExt
    : public virtual CefResourceRequestHandler,
      public virtual CefBaseRefCounted {
 public:
  virtual CefRefPtr<ArkWebResourceRequestHandlerExt>
  AsArkWebResourceRequestHandlerExt() override {
    return this;
  }

  ///
  /// GetResourceHandlerByIO.
  ///
  virtual void GetResourceHandlerByIO(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      CefRefPtr<CefInterceptCallback> callback,
      CefRefPtr<CefSchemeHandlerFactory> scheme_factory,
      const CefString& scheme) {}
};

#endif  // OHOS_CEF_EXT_INCLUDE_ARKWEB_RESOURCE_REQUEST_HANDLER_EXT_H_
