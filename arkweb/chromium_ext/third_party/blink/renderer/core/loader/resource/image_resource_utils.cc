/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/loader/resource/image_resource_utils.h"

#include "third_party/blink/renderer/core/loader/resource/image_resource.h"
#include "third_party/blink/renderer/platform/loader/fetch/unique_identifier.h"
#include "third_party/blink/renderer/platform/weborigin/security_origin.h"

namespace blink {
ImageResourceUtils::ImageResourceUtils(ImageResource* impl)
{
  this->imageResource = impl;
}

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
ImageResource* ImageResourceUtils::CreateForOfflineResource(const KURL& kurl,
                                                            const KURL& origin_url)
{
  ResourceRequest request(kurl);
  request.SetInspectorId(CreateUniqueIdentifier());
  request.SetRequestorOrigin(SecurityOrigin::Create(origin_url));

  ResourceLoaderOptions options(nullptr);

  auto image_resource = MakeGarbageCollected<ImageResource>(
      request, options, ImageResourceContent::CreateNotStarted());

  image_resource->NotifyStartLoad();
  return image_resource;
}
#endif

void ImageResourceUtils::Trace(Visitor* visitor) const {
  visitor->Trace(imageResource);
}
}  // namespace blink
