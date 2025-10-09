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

#ifndef ARKWEB_EXT_CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_IMPL_H_
#define ARKWEB_EXT_CONTENT_BROWSER_MEDIA_SESSION_MEDIA_SESSION_IMPL_H_

#include "arkweb/build/features/features.h"
#include "content/browser/media/session/media_session_impl.h"

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "content/browser/media/session/media_session_ohos.h"
#endif  // BUILDFLAG(ARKWEB_MEDIA_AVSESSION)

namespace content {
class MediaSessionImpl;

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
class MediaSessionOHOS;
#endif  // BUILDFLAG(ARKWEB_MEDIA_AVSESSION)

class MediaSessionImplUtils {
public:
    raw_ptr<MediaSessionImpl> impl = nullptr;
    MediaSessionImplUtils(MediaSessionImpl* impl);

    void CheckPosition(std::optional<media_session::MediaPosition> position);

    void IsSuspended();

    void PushBackMediaImage(std::vector<media_session::MediaImage>& artwork);

    void SetMediaTitle(media_session::MediaMetadata& metadata);

    void DoEndSessionWhenHide();
};

}

#endif  // ARKWEB_EXT_THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_FULLSCREEN_CONTROLLER_H_
