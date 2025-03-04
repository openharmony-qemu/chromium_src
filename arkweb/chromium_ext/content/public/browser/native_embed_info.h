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

#ifndef CONTENT_PUBLIC_BROWSER_NATIVE_EMBED_INFO_H_
#define CONTENT_PUBLIC_BROWSER_NATIVE_EMBED_INFO_H_

#include <map>
#include <string>

#include "content/common/content_export.h"
#include "ui/gfx/geometry/rect.h"
#include "url/gurl.h"

namespace content {

struct CONTENT_EXPORT NativeEmbedInfo {
  enum TagState {
    TAG_STATE_CREATE = 0,
    TAG_STATE_CHANGE = 1,
    TAG_STATE_DESTROY = 2,
    TAG_STATE_ENTER_BFCACHE = 3,
    TAG_STATE_LEAVE_BFCACHE = 4
  };

  NativeEmbedInfo();
  NativeEmbedInfo(int native_embed_id,
                  GURL url,
                  std::string& element_id,
                  std::string& native_type,
                  std::string& native_source,
                  std::string& tag,
                  gfx::Rect& rect,
                  std::map<std::string, std::string>& params);
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
  std::string tag;
  gfx::Rect rect;
  std::map<std::string, std::string> params;
};

CONTENT_EXPORT std::ostream& operator<<(std::ostream& out,
                                        const NativeEmbedInfo& info);

}  // namespace content

#endif  // CONTENT_PUBLIC_BROWSER_NATIVE_EMBED_INFO_H_
