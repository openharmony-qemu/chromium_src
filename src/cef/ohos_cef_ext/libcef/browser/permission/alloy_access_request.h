// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PERMISSION_ALLOY_ACCESS_REQUEST_H_
#define CEF_LIBCEF_BROWSER_PERMISSION_ALLOY_ACCESS_REQUEST_H_

#include <memory>
#include <map>

#include "cef/libcef/browser/browser_host_base.h"
#include "cef/ohos_cef_ext/include/cef_permission_request.h"

#if BUILDFLAG(ARKWEB_WEBRTC)
#include "content/public/browser/media_stream_request.h"
#include "third_party/blink/public/common/mediastream/media_stream_request.h"
#include "libcef/browser/permission/alloy_permission_manager.h"
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

// This class is used to handle the permission request which just needs
// a callback with bool parameter to indicate the permission granted or not. It
// works with PermissionRequestHandler. The specific permission request should
// implement the CefAccessRequest.
class AlloyAccessRequest : public CefAccessRequest {
 public:
  AlloyAccessRequest(const AlloyAccessRequest&) = delete;
  AlloyAccessRequest& operator=(const AlloyAccessRequest&) = delete;

  enum Resources {
    GEOLOCATION = 1 << 0,
    VIDEO_CAPTURE = 1 << 1,
    AUDIO_CAPTURE = 1 << 2,
    PROTECTED_MEDIA_ID = 1 << 3,
    MIDI_SYSEX = 1 << 4,
    CLIPBOARD_READ_WRITE = 1 << 5,
    CLIPBOARD_SANITIZED_WRITE = 1 << 6,
    SENSORS = 1 << 7,
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
    NOTIFICATION = 1 << 8,
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  };

  AlloyAccessRequest(const CefString& origin,
                     int resources,
                     cef_permission_callback_t callback);
  ~AlloyAccessRequest() override;

  CefString Origin() override;

  int ResourceAcessId() override;

  void ReportRequestResult(bool allowed) override;

 private:
  CefString origin_;
  int resources_;
  cef_permission_callback_t callback_;

  IMPLEMENT_REFCOUNTING(AlloyAccessRequest);
};

#if BUILDFLAG(ARKWEB_SENSOR)
class AlloySensorAccessRequest : public CefAccessRequest {
 public:
  AlloySensorAccessRequest(CefBrowserHostBase* const browser,
                           const CefString& origin,
                           cef_permission_callback_t callback);

  AlloySensorAccessRequest(const AlloySensorAccessRequest&) = delete;
  AlloySensorAccessRequest& operator=(const AlloySensorAccessRequest&) = delete;

  ~AlloySensorAccessRequest() override;

  // CefAccessRequest implementation.
  CefString Origin() override;
  int ResourceAcessId() override;
  void ReportRequestResult(bool allowed) override;

 private:
  CefRefPtr<CefBrowserHostBase> browser_;
  CefString origin_;
  cef_permission_callback_t callback_;
  IMPLEMENT_REFCOUNTING(AlloySensorAccessRequest);
};
#endif  // BUILDFLAG(ARKWEB_SENSOR)

#if BUILDFLAG(ARKWEB_WEBRTC)
namespace content {
  using PermissionStatus = blink::mojom::PermissionStatus;
}

class AlloyMediaAccessRequest : public CefAccessRequest {
 public:
  static std::map<GURL, int32_t> camera_permission_;
  static std::map<GURL, int32_t> microphone_permission_;

  AlloyMediaAccessRequest(CefBrowserHostBase* const browser,
                          const content::MediaStreamRequest& request,
                          content::MediaResponseCallback callback);

  AlloyMediaAccessRequest(const AlloyMediaAccessRequest&) = delete;
  AlloyMediaAccessRequest& operator=(const AlloyMediaAccessRequest&) = delete;

  ~AlloyMediaAccessRequest() override;

  // CefAccessRequest implementation.
  CefString Origin() override;
  int ResourceAcessId() override;
  void ReportRequestResult(bool allowed) override;
  GURL GetMediaAccessRequestOriginAsURL();

 private:
  CefRefPtr<CefBrowserHostBase> browser_;
  const content::MediaStreamRequest request_;
  content::MediaResponseCallback callback_;
  GURL requesting_origin_;

  IMPLEMENT_REFCOUNTING(AlloyMediaAccessRequest);
};

class AlloyScreenCaptureAccessRequest : public CefScreenCaptureAccessRequest {
 public:
  AlloyScreenCaptureAccessRequest(CefBrowserHostBase* const browser,
                                  const content::MediaStreamRequest& request,
                                  content::MediaResponseCallback callback);

  AlloyScreenCaptureAccessRequest(const AlloyScreenCaptureAccessRequest&) =
      delete;
  AlloyScreenCaptureAccessRequest& operator=(
      const AlloyScreenCaptureAccessRequest&) = delete;

  ~AlloyScreenCaptureAccessRequest() override;

  // CefScreenCaptureAccessRequest implementation.
  CefString Origin() override;
  void SetCaptureMode(int32_t mode) override;
  void SetCaptureSourceId(int32_t sourceId) override;
  void ReportRequestResult(bool allowed) override;

 private:
  CefRefPtr<CefBrowserHostBase> browser_;
  const content::MediaStreamRequest request_;
  content::MediaResponseCallback callback_;
  int32_t mode_;
  int32_t sourceId_;
  int32_t audioSourceId_;

  IMPLEMENT_REFCOUNTING(AlloyScreenCaptureAccessRequest);
};
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#endif  // CEF_LIBCEF_BROWSER_PERMISSION_ALLOY_ACCESS_REQUEST_H_
