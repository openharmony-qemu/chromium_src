/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_MEDIA_PLAYER_CLIENT_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_MEDIA_PLAYER_CLIENT_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_media_player.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"
#include "ui/gfx/color_space.h"

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "base/containers/flat_map.h"
#include "media/base/renderer.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif // IS_ARKWEB_EXT

namespace media {
class RemotePlaybackClientWrapper;
}  // namespace media

namespace blink {

class BLINK_PLATFORM_EXPORT WebMediaPlayerClient {
 public:
  // Returns the remote playback client associated with the media element, if
  // any.
  virtual media::RemotePlaybackClientWrapper* RemotePlaybackClientWrapper() {
    return nullptr;
  }

  // Returns the DOMNodeId of the DOM element hosting this media player.
  virtual int GetElementId() = 0;

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  virtual bool IsMuted() { return false; }
  virtual bool IsCustomVideoPlayerEnabled() { return false; }
  virtual bool ShouldCustomVideoPlayerOverlay() { return false; }
  virtual bool ShouldShowMediaControls() { return false; }
  virtual std::string GetMediaFormat() { return ""; }
  virtual void RestartForPrimitive() {}
  virtual Vector<media::Renderer::MediaSourceInfo> GetRemainSourceInfos() {
    return {};
  }
  virtual Vector<WebString> GetMediaControlsList() { return {}; }
  virtual base::flat_map<std::string, std::string> GetElementAttributes() {
    return {};
  }
  virtual std::string GetOutgoingReferrerString() { return std::string(); }

  virtual void UpdatePlaybackStatus(uint32_t status) {}
  virtual void UpdateVolume(double volume) {}
  virtual void UpdateMuted(bool muted) {}
  virtual void UpdatePlaybackRate(double playback_rate) {}

  virtual gfx::Rect GetVideoRect() { return gfx::Rect(); }
  virtual void OnLayerRectChange(const gfx::Rect& rect) {}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  // Notify the client that the avsession should be destoryed.
  virtual void DidEndAVSession(bool is_hidden) = 0;
#endif // ARKWEB_MEDIA_AVSESSION

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
  virtual void DidPlayerGone() = 0;
#endif
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  virtual bool IsVideoAssistantEnabled() { return false; }
  virtual void OnLayerBoundsChange(const gfx::Rect& bounds) {}
  virtual void OnPageVisibilityChanged() {}
  virtual bool IsCustomMediaPlayerEnabled() const { return false; }
  virtual void OnSupportVideoSurfaceChanged(
      bool support, std::string decoder_name) {}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
  virtual void OnDmaBufferSeekTo(base::TimeDelta dmabuf_pause_time) {}
#endif  // ARKWEB_MEDIA_DMABUF

#if BUILDFLAG(ARKWEB_BFCACHE)
  virtual bool IsMediaResumeFromBFCachePage() const { return true; }
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  virtual void ScheduleVideoFreezeEvent() = 0;
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
  virtual uint16_t hbsMediaPreloadTime() { return 0; }
  virtual uint16_t hbsMediaMaxCacheTime() { return 0; }
  virtual uint16_t hbsMediaMinCacheTime() { return 0; }
  virtual uint16_t hbsMediaBitrate() { return 0; }
  virtual uint16_t hbsMediaMoovSize() { return 0; }
  virtual std::string videoId() { return ""; }
#endif  // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION
#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  virtual void ReportVideoExperienceToBI() {}
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

 protected:
  ~WebMediaPlayerClient() = default;

 private:
  friend class MediaPlayerClient;
  WebMediaPlayerClient() = default;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_MEDIA_PLAYER_CLIENT_H_
