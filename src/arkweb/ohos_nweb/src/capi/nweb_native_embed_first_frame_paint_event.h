/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_NWEB_SRC_CAPI_NWEB_NATIVE_EMBED_FIRST_FRAME_PAINT_EVENT_H_
#define OHOS_NWEB_SRC_CAPI_NWEB_NATIVE_EMBED_FIRST_FRAME_PAINT_EVENT_H_

#include <string>

struct NWebNativeEmbedFirstFramePaintEvent final {
  std::string embed_id;
  std::string surface_id;
  std::string embed_id_attribute;

  NWebNativeEmbedFirstFramePaintEvent(const std::string& embed_id_str,
                                      const std::string& surface_id_str,
                                      const std::string& embed_id_attribute_str)
      : embed_id(embed_id_str),
        surface_id(surface_id_str),
        embed_id_attribute(embed_id_attribute_str) {}
};

#endif  // OHOS_NWEB_SRC_CAPI_NWEB_NATIVE_EMBED_FIRST_FRAME_PAINT_EVENT_H_
