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
 
#include "arkweb/chromium_ext/content/browser/media/session/media_session_impl_utils.h"
 
#include <algorithm>
#include <memory>
#include <utility>
 
#include "arkweb/build/features/features.h"
#include "base/memory/ptr_util.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/media_session.h"
#include "content/public/browser/media_session_client.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/common/content_client.h"
#include "media/audio/audio_device_description.h"
#include "media/base/media_content_type.h"
#include "media/base/media_switches.h"

namespace content {
 
MediaSessionImplUtils::MediaSessionImplUtils(MediaSessionImpl* impl) {
  this->impl = impl;
}
 
void MediaSessionImplUtils::CheckPosition(std::optional<media_session::MediaPosition> position) {
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
    if (impl->session_ohos_) {
      impl->session_ohos_->MediaSessionPositionChanged(position);
    }
#endif  // BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
}
 
void MediaSessionImplUtils::IsSuspended() {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (!impl->IsSuspended()) {
    return;
  }
#endif
}
 
void MediaSessionImplUtils::PushBackMediaImage(std::vector<media_session::MediaImage> artwork) {
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  std::string attrib_image_url = impl->web_contents()->GetVideoPoster();
  media_session::MediaImage mediaImage;
  mediaImage.src = GURL(attrib_image_url);
  artwork.push_back(mediaImage);
#endif  // ARKWEB_MEDIA_AVSESSION
}
 
void MediaSessionImplUtils::SetMediaTitle(media_session::MediaMetadata& metadata) {
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  if (impl->web_contents()) {
    std::u16string u16title =
        base::UTF8ToUTF16(impl->web_contents()->GetMediaTitle());
    if (!u16title.empty()) {
      metadata.title = u16title;
    }
  }
#endif  // ARKWEB_MEDIA_AVSESSION
}

void MediaSessionImplUtils::DoEndSessionWhenHide() {
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  if (!impl->IsPauseByAvsession()) {
    LOG(INFO) << "MediaSessionImplUtils: DoEndSessionWhenHide.";
    impl->EndSessionWhenHide();
  }
  if (impl->IsSuspended()) {
    return;
  }
#endif // ARKWEB_MEDIA_AVSESSION
}

}  // namespace blink
