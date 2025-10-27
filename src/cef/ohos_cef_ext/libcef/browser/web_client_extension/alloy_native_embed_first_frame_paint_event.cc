// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/web_client_extension/alloy_native_embed_first_frame_paint_event.h"

#include "content/common/native_embed_first_paint_event.h"

AlloyNativeEmbedFirstFramePaintEvent::AlloyNativeEmbedFirstFramePaintEvent(
    const content::NativeEmbedFirstPaintEvent& event)
    : embed_id_(event.embed_id),
      surface_id_(event.surface_id),
      embed_id_attribute_(event.embed_id_attribute) {}

AlloyNativeEmbedFirstFramePaintEvent::~AlloyNativeEmbedFirstFramePaintEvent() =
    default;

CefString AlloyNativeEmbedFirstFramePaintEvent::GetEmbedId() {
  return embed_id_;
}
CefString AlloyNativeEmbedFirstFramePaintEvent::GetSurfaceId() {
  return surface_id_;
}
CefString AlloyNativeEmbedFirstFramePaintEvent::GetEmbedIdAttribute() {
  return embed_id_attribute_;
}
 