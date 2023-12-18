/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 */

#ifndef CONTENT_PUBLIC_BROWSER_NATIVE_EMBED_INFO_H_
#define CONTENT_PUBLIC_BROWSER_NATIVE_EMBED_INFO_H_

#include <string>

#include "content/common/content_export.h"
#include "ui/gfx/geometry/size.h"
#include "url/gurl.h"

namespace content {

struct CONTENT_EXPORT NativeEmbedInfo {
  enum TagState { TAG_STATE_CREATE, TAG_STATE_CHANGE, TAG_STATE_DESTROY };

  NativeEmbedInfo();
  NativeEmbedInfo(int native_embed_id,
                  GURL url,
                  std::string element_id,
                  std::string native_type,
                  std::string native_source,
                  gfx::Size size);
  NativeEmbedInfo(const NativeEmbedInfo& other);
  NativeEmbedInfo& operator=(const NativeEmbedInfo& other);
  NativeEmbedInfo(NativeEmbedInfo&& other);
  NativeEmbedInfo& operator=(NativeEmbedInfo&& other);
  ~NativeEmbedInfo();

  int native_embed_id = -1;
  GURL url;
  std::string embed_element_id;
  std::string native_type;
  std::string native_source;
  gfx::Size size;
};

CONTENT_EXPORT std::ostream& operator<<(std::ostream& out,
                                        const NativeEmbedInfo& info);

}  // namespace content

#endif  // CONTENT_PUBLIC_BROWSER_NATIVE_EMBED_INFO_H_
