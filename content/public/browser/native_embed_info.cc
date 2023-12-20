/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 */

#include "content/public/browser/native_embed_info.h"

namespace content {

NativeEmbedInfo::NativeEmbedInfo(int native_embed_id,
                                 GURL url,
                                 std::string element_id,
                                 std::string native_type,
                                 std::string native_source,
                                 gfx::Size size)
    : native_embed_id(native_embed_id),
      url(url),
      embed_element_id(element_id),
      native_type(native_type),
      native_source(native_source),
      size(size) {}

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
      << ", native source: " << info.native_source
      << ", native size: " << info.size.ToString();
  return out;
}

}  // namespace content
