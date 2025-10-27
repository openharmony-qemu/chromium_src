// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_NATIVE_EMBED_FIRST_PAINT_EVENT_H_
#define CONTENT_COMMON_NATIVE_EMBED_FIRST_PAINT_EVENT_H_

#include <string>

namespace content {

struct NativeEmbedFirstPaintEvent {
  std::string embed_id;
  std::string surface_id;
  std::string embed_id_attribute;
};

}  // namespace content

#endif  // CONTENT_COMMON_NATIVE_EMBED_FIRST_PAINT_EVENT_H_
