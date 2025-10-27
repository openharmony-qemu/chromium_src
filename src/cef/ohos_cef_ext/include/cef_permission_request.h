// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_INCLUDE_CEF_PERMISSION_REQUEST_H_
#define CEF_INCLUDE_CEF_PERMISSION_REQUEST_H_
#pragma once

#include "include/cef_base.h"
#include "include/internal/cef_types.h"

#if BUILDFLAG(IS_OHOS)
///
/// Class used to report screen capture permission request for the specified
/// origin.
///
/*--cef(source=library)--*/
class CefScreenCaptureAccessRequest : public virtual CefBaseRefCounted {
 public:
  ///
  /// Get the origin that is trying to acess the resource.
  ///
  /*--cef()--*/
  virtual CefString Origin() = 0;
  ///
  /// Set screen capture mode. {@link #cef_screen_capture_mode_t}
  ///
  /*--cef()--*/
  virtual void SetCaptureMode(int32_t mode) = 0;
  ///
  /// Set screen capture source id.
  ///
  /*--cef()--*/
  virtual void SetCaptureSourceId(int32_t sourceId) = 0;
  ///
  /// Report whether the origin is allowed to acess the resource.
  ///
  /*--cef()--*/
  virtual void ReportRequestResult(bool allowed) = 0;
};
#endif  // BUILDFLAG(IS_OHOS)

///
/// Class used to report permission request for the specified origin.
///
/*--cef(source=library)--*/
class CefAccessRequest : public virtual CefBaseRefCounted {
 public:
  ///
  /// Get the origin that is trying to acess the resource.
  ///
  /*--cef()--*/
  virtual CefString Origin() = 0;
  ///
  /// Get the resource that the origin is trying to acess.
  ///
  /*--cef()--*/
  virtual int ResourceAcessId() = 0;
  ///
  /// Report whether the origin is allowed to acess the resource.
  ///
  /*--cef()--*/
  virtual void ReportRequestResult(bool allowed) = 0;
};

///
/// Implement this interface to handle permission requests.
///
/*--cef(source=client,no_debugct_check)--*/
class CefPermissionRequest : public virtual CefBaseRefCounted {
 public:
  ///
  /// Notify the host application that web content from the specified origin is
  /// attempting to use the Geolocation API, but no permission state is
  /// currently set for that origin. The host application should invoke the
  /// specified callback with the desired permission state.
  ///
  /*--cef()--*/
  virtual void OnGeolocationShow(const CefString& origin) = 0;
  ///
  /// Revert the operation in the OnGeolocationShow.
  ///
  /*--cef()--*/
  virtual void OnGeolocationHide() = 0;
  ///
  /// Notify the host application that web content from the specified origin is
  /// attempting to access the resources, but no permission state is currently
  /// set for that origin. The host application should invoke the specified
  /// callback with the desired permission state.
  ///
  /*--cef()--*/
  virtual void OnPermissionRequest(CefRefPtr<CefAccessRequest> request) = 0;
  ///
  /// Revert the operation in the OnPermissionRequest.
  ///
  /*--cef()--*/
  virtual void OnPermissionRequestCanceled(
      CefRefPtr<CefAccessRequest> request) = 0;

#if BUILDFLAG(IS_OHOS)
  ///
  /// Notify the host application that web content from the specified origin is
  /// attempting to access the screen capture resources, but no permission state
  /// is currently set for that origin. The host application should invoke the
  /// specified callback with the desired permission state.
  ///
  /*--cef()--*/
  virtual void OnScreenCaptureRequest(
      CefRefPtr<CefScreenCaptureAccessRequest> request) = 0;
#endif  // BUILDFLAG(IS_OHOS)
};

///
/// Class used to handle the permission requests from |BrowserContext|.
///
/*--cef(source=library)--*/
class CefBrowserPermissionRequestDelegate : public virtual CefBaseRefCounted {
 public:
  ///
  /// Handle the Geolocation permission requests.
  ///
  /*--cef()--*/
  virtual void AskGeolocationPermission(const CefString& origin,
                                        cef_permission_callback_t callback) = 0;
  ///
  /// Cancel the Geolocation permission requests.
  ///
  /*--cef()--*/
  virtual void AbortAskGeolocationPermission(const CefString& origin) = 0;

#if BUILDFLAG(ARKWEB_SENSOR)
  ///
  /// Handle the Sensors permission requests.
  ///
  /*--cef()--*/
  virtual void AskSensorsPermission(const CefString& origin,
                                    cef_permission_callback_t callback) = 0;
  ///
  /// Cancel the Sensors permission requests.
  ///
  /*--cef()--*/
  virtual void AbortAskSensorsPermission(const CefString& origin) = 0;
#endif  // BUILDFLAG(ARKWEB_SENSOR)

  ///
  /// Handle the Protected Media Identifier permission requests.
  ///
  /*--cef()--*/
  virtual void AskProtectedMediaIdentifierPermission(
      const CefString& origin,
      cef_permission_callback_t callback) = 0;
  ///
  /// Cancel the Protected Media Identifier permission requests.
  ///
  /*--cef()--*/
  virtual void AbortAskProtectedMediaIdentifierPermission(
      const CefString& origin) = 0;
  ///
  /// Handle the MIDI Sysex permission requests.
  ///
  /*--cef()--*/
  virtual void AskMIDISysexPermission(const CefString& origin,
                                      cef_permission_callback_t callback) = 0;
  ///
  /// Cancel the MIDI Sysex permission requests.
  ///
  /*--cef()--*/
  virtual void AbortAskMIDISysexPermission(const CefString& origin) = 0;

  ///
  /// Handle the Clipboard Read dWrite permission requests.
  ///
  /*--cef()--*/
  virtual void AskClipboardReadWritePermission(
      const CefString& origin,
      cef_permission_callback_t callback) = 0;
  ///
  /// Cancel the Clipboard Read Write permission requests.
  ///
  /*--cef()--*/
  virtual void AbortAskClipboardReadWritePermission(
      const CefString& origin) = 0;

  ///
  /// The callback for the Geolocation permission requests.
  ///
  /*--cef()--*/
  virtual void NotifyGeolocationPermission(bool value,
                                           const CefString& origin) = 0;

  ///
  /// Handle the Audio Capture permission requests.
  ///
  /*--cef()--*/
  virtual void AskAudioCapturePermission(
      const CefString& origin,
      cef_permission_callback_t callback) = 0;
  ///
  /// Cancel the Audio Capturepermission requests.
  ///
  /*--cef()--*/
  virtual void AbortAskAudioCapturePermission(const CefString& origin) = 0;

  ///
  /// Handle the Video Capture permission requests.
  ///
  /*--cef()--*/
  virtual void AskVideoCapturePermission(
      const CefString& origin,
      cef_permission_callback_t callback) = 0;
  ///
  /// Cancel the Video Capturepermission requests.
  ///
  /*--cef()--*/
  virtual void AbortAskVideoCapturePermission(const CefString& origin) = 0;

  ///
  /// Handle the Clipboard Sanitized permission requests.
  ///
  /*--cef()--*/
  virtual void AskClipboardSanitizedWritePermission(
      const CefString& origin,
      cef_permission_callback_t callback) = 0;
  ///
  /// Cancel the Clipboard Read Sanitezed permission requests.
  ///
  /*--cef()--*/
  virtual void AbortAskClipboardSanitizedWritePermission(
      const CefString& origin) = 0;
#if BUILDFLAG(ARKWEB_NOTIFICATION)
  ///
  /// Handle the Notification permission requests.
  ///
  /*--cef()--*/
  virtual void AskNotificationPermission(const CefString& origin,
                                         cef_permission_callback_t callback) = 0;
  ///
  /// Cancel the Notification permission requests.
  ///
  /*--cef()--*/
  virtual void AbortAskNotificationPermission(const CefString& origin) = 0;
#endif // ARKWEB_NOTIFICATION

};

///
/// Class used to set the geolocation permission state for the specified origin.
///
/*--cef(source=library)--*/
class CefGeolocationAcess : public virtual CefBaseRefCounted {
 public:
  ///
  /// Return true if the geolocation permission state is set for the specified
  /// origin.
  ///
  /*--cef()--*/
  virtual bool ContainOrigin(const CefString& origin) = 0;
  ///
  /// Return true if the geolocation permission state set for the specified
  /// origin is true.
  ///
  /*--cef()--*/
  virtual bool IsOriginAccessEnabled(const CefString& origin) = 0;
  ///
  /// Set the geolocation permission state to true  for the specified origin.
  ///
  /*--cef()--*/
  virtual void Enabled(const CefString& origin, bool incognito) = 0;
  ///
  /// Set the geolocation permission state to false  for the specified origin.
  ///
  /*--cef()--*/
  virtual void Disabled(const CefString& origin, bool incognito) = 0;
};

#endif  // CEF_INCLUDE_CEF_PERMISSION_REQUEST_H_
