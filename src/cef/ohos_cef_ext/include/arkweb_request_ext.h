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

#ifndef OHOS_CEF_EXT_INCLUDE_ARKWEB_REQUEST_EXT_H_
#define OHOS_CEF_EXT_INCLUDE_ARKWEB_REQUEST_EXT_H_
#pragma once

#include "arkweb/build/features/features.h"
#include "include/cef_request.h"

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
class ArkWebCefPostDataStream;

namespace network {
class ResourceRequestBody;
}
#endif

///
/// Extended of CefRequest
///
class ArkWebRequestExt : public virtual CefRequest,
                         public virtual CefBaseRefCounted {
 public:
  ///
  /// Returns whether the request was made for the main frame document.
  /// Will be false for subresources or iframes
  ///
  virtual bool IsMainFrame() = 0;

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
  ///
  /// Returns whether the request was redirect.
  ///
  virtual bool IsRedirect() = 0;

  ///
  /// Returns whether the request was triggered by user gesture.
  ///
  virtual bool HasUserGesture() = 0;

  ///
  /// Get the upload stream.
  ///
  virtual CefRefPtr<ArkWebCefPostDataStream> GetUploadStream() = 0;

  ///
  /// Set frame url.
  ///
  virtual void SetFrameUrl(const CefString& frame_url) = 0;

  ///
  /// Get frame url.
  ///
  virtual CefString GetFrameUrl() = 0;
#endif  // BUILDFLAG(ARKWEB_SCHEME_HANDLER)
};

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
///
/// Callback for init the stream.
///
class ArkWebCefPostDataStreamInitCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Callback for init the stream.
  ///
  virtual void OnInitComplete(int result) = 0;
};

///
/// Callback for read from stream.
///
class ArkWebCefPostDataStreamReadCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Callback for read from stream.
  ///
  virtual void OnReadComplete(char* buffer, int bytes_read) = 0;
};

///
/// Callback for async read from stream.
///
class ArkWebCefPostDataStreamAsyncReadCallback
    : public virtual CefBaseRefCounted {
 public:
  ///
  /// Callback for async read from stream.
  ///
  virtual void OnAsyncReadComplete(char* buffer, int bytes_read) = 0;
};

///
/// Class used to represent post data for a web request. The methods of this
/// class may be called on any thread.
///
class ArkWebCefPostDataStream : public virtual CefBaseRefCounted {
 public:
  ///
  /// Create a new CefPostDataStream object.
  ///
  static CefRefPtr<ArkWebCefPostDataStream> Create();

  ///
  /// Set ready callback.
  ///
  virtual void SetReadCallback(
      CefRefPtr<ArkWebCefPostDataStreamReadCallback> read_callback) = 0;

  ///
  /// Init the stream.
  ///
  virtual void Init(
      CefRefPtr<ArkWebCefPostDataStreamInitCallback> init_callback) = 0;

  ///
  /// Read the stream.
  ///
  virtual void Read(
      void* buffer,
      int buf_len,
      CefRefPtr<ArkWebCefPostDataStreamReadCallback> read_callback) = 0;

  ///
  /// AsyncRead the stream.
  ///
  virtual void AsyncRead(
      void* buffer,
      int buf_len,
      CefRefPtr<ArkWebCefPostDataStreamAsyncReadCallback> read_callback) = 0;

  ///
  /// Get the size of stream.
  ///
  virtual uint64_t GetSize() = 0;

  ///
  /// Get the position of stream.
  ///
  virtual uint64_t GetPosition() = 0;

  ///
  /// Get if the stream is trunked.
  ///
  virtual bool IsChunked() = 0;

  ///
  /// Get if the stream is trunked.
  ///
  virtual bool HasNullSource() = 0;

  ///
  /// Get if the stream is trunked.
  ///
  virtual bool IsEOF() = 0;

  ///
  /// Get if the stream is trunked.
  ///
  virtual bool IsInMemory() = 0;

  ///
  /// Reset();
  ///
  virtual void Reset() = 0;

  ///
  /// GetChunkedDataPipeGetter ARKWEB_NETWORK_BASE
  ///
  virtual void GetChunkedDataPipeGetter(network::ResourceRequestBody* body) = 0;
};
#endif  // BUILDFLAG(ARKWEB_SCHEME_HANDLER)

#endif  // OHOS_CEF_EXT_INCLUDE_ARKWEB_REQUEST_EXT_H_
