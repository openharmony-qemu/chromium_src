/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/content/public/browser/native_embed_info.h"

namespace content {

NativeEmbedInfo::NativeEmbedInfo(int native_embed_id,
                                 GURL url,
                                 std::string& element_id,
                                 std::string& native_type,
                                 std::string& native_source,
                                 std::string& tag,
                                 gfx::Rect& rect,
                                 std::map<std::string, std::string>& param_list)
    : native_embed_id(native_embed_id),
      url(url),
      embed_element_id(element_id),
      native_type(native_type),
      native_source(native_source),
      tag(tag),
      rect(rect),
      params(param_list) {}

NativeEmbedInfo::NativeEmbedInfo(const NativeEmbedInfo& other) = default;
NativeEmbedInfo& NativeEmbedInfo::operator=(const NativeEmbedInfo& other) =
    default;

NativeEmbedInfo::NativeEmbedInfo(NativeEmbedInfo&& other) = default;
NativeEmbedInfo& NativeEmbedInfo::operator=(NativeEmbedInfo&& other) = default;

NativeEmbedInfo::NativeEmbedInfo() = default;
NativeEmbedInfo::~NativeEmbedInfo() = default;

std::ostream& operator<<(std::ostream& out, const NativeEmbedInfo& info) {
  out << "[NativeEmbed] native embed id: " << info.native_embed_id
      << ", url: " << info.url.path()
      << ", embed element id: " << info.embed_element_id
      << ", native type: " << info.native_type
      << ", native source: " << info.native_source << ", tag: " << info.tag
      << ", native rect: " << info.rect.ToString();
  return out;
}

}  // namespace content
                       