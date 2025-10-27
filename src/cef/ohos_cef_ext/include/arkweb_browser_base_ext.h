// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ARKWEB_INCLUDE_BROWSER_BASE_EXT_H_
#define ARKWEB_INCLUDE_BROWSER_BASE_EXT_H_
#pragma once

#if BUILDFLAG(ARKWEB_PDF)
///
/// Callback interface for CefBrowserHost::CreateToPDF. The methods of this
/// class will be called on the browser process UI thread.
///
/*--cef(source=client)--*/
class CefPdfValueCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be executed when the PDF create has completed. |value|
  /// is the output pdf data stream.
  /// successfully or false otherwise.
  ///
  /*--cef()--*/
  virtual void OnReceiveValue(const char* value, const long size) = 0;
};
#endif

///
/// callback for RegisterScreenCaptureDelegateListener().
///
/*--cef(source=client)--*/
class CefScreenCaptureCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon completion.
  ///
  /*--cef()--*/
  virtual void OnStateChange(int32_t nweb_id,
                             const CefString& session_id,
                             int32_t code) = 0;
};

class ArkWebBrowserBase {
public:
  ///
  /// Determine if BeforeUnload or Unload events need to be triggered.
  ///
  /*--cef()--*/
  virtual bool NeedToFireBeforeUnloadOrUnloadEvents() = 0;

  ///
  /// Trigger the BeforeUnload event with an option to auto-cancel.
  ///
  /*--cef()--*/
  virtual void DispatchBeforeUnload() = 0;
};

class ArkwebBrowserHostBase {
public:

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  virtual void NotifyScreenInfoChangedV2() {}
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  ///
  /// SetPopupWindow.
  ///
  /*--cef()--*/
  virtual void SetPopupWindow(cef_native_window_t window) = 0;
#endif

#if BUILDFLAG(ARKWEB_PDF)
  ///
  /// Create pdf data stream.
  ///
  /*--cef()--*/
  virtual void CreateToPDF(const CefPdfPrintSettings& settings,
                           CefRefPtr<CefPdfValueCallback> callback) = 0;
#endif
  ///
  /// set video assistant enable.
  ///
  /*--cef()--*/
  virtual void EnableVideoAssistant(bool enable) = 0;

  ///
  /// execute video assistant function.
  ///
  /*--cef()--*/
  virtual void ExecuteVideoAssistantFunction(const CefString& cmdId) = 0;

#if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
  ///
  /// Get whether it is the iframe.
  ///
  /*--cef()--*/
  virtual bool IsIframe() = 0;

  ///
  /// fresh focused frame for context menu.
  ///
  /*--cef()--*/
  virtual void ReloadFocusedFrame() = 0;
#endif

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  ///
  ///  Close current screen capture.
  ///
  /*--cef()--*/
  virtual void StopScreenCapture(int32_t nweb_id,
                                 const CefString& session_id) = 0;

  ///
  ///  set screen capture picker show.
  ///
  /*--cef()--*/
  virtual void SetScreenCapturePickerShow() = 0;
 
  ///
  ///  disable screen capture session reuse.
  ///
  /*--cef()--*/
  virtual void DisableSessionReuse() = 0;

  ///
  ///  Register screen capture listener.
  ///
  /*--cef()--*/
  virtual void RegisterScreenCaptureDelegateListener(
      CefRefPtr<CefScreenCaptureCallback> listener) = 0;
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

  /// set custom web media player enable.
  ///
  /*--cef()--*/
  virtual void CustomWebMediaPlayer(bool enable) = 0;

  ///
  /// set media resumes playback when the page is restored from BFCache.
  ///
  /*--cef()--*/
  virtual void SetMediaResumeFromBFCachePage(bool resume) = 0;
};
#endif //ARKWEB_INCLUDE_BROWSER_BASE_EXT_H_