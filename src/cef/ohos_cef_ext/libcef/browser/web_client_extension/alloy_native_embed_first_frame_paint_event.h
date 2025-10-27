// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_WEB_CLIENT_EXTENSION_ALLOY_NATIVE_EMBED_FIRST_FRAME_PAINT_EVENT_H_
#define CEF_LIBCEF_BROWSER_WEB_CLIENT_EXTENSION_ALLOY_NATIVE_EMBED_FIRST_FRAME_PAINT_EVENT_H_

#include <string>

#include "include/cef_native_embed_first_frame_paint_event.h"

namespace content {
struct NativeEmbedFirstPaintEvent;
}

class AlloyNativeEmbedFirstFramePaintEvent
    : public CefNativeEmbedFirstFramePaintEvent {
 public:
  AlloyNativeEmbedFirstFramePaintEvent(
      const content::NativeEmbedFirstPaintEvent& event);
  AlloyNativeEmbedFirstFramePaintEvent(
      const AlloyNativeEmbedFirstFramePaintEvent&) = delete;
  AlloyNativeEmbedFirstFramePaintEvent& operator=(
      const AlloyNativeEmbedFirstFramePaintEvent&) = delete;
  virtual ~AlloyNativeEmbedFirstFramePaintEvent();

  CefString GetEmbedId() override;
  CefString GetSurfaceId() override;
  CefString GetEmbedIdAttribute() override;

 private:
  CefString embed_id_;
  CefString surface_id_;
  CefString embed_id_attribute_;

  IMPLEMENT_REFCOUNTING(AlloyNativeEmbedFirstFramePaintEvent);
};

#endif  // CEF_LIBCEF_BROWSER_WEB_CLIENT_EXTENSION_ALLOY_NATIVE_EMBED_FIRST_FRAME_PAINT_EVENT_H_
