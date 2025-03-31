// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_RENDERER_H_
#define MEDIA_BASE_RENDERER_H_

#include "base/functional/callback.h"
#include "base/time/time.h"
#include "media/base/buffering_state.h"
#include "media/base/demuxer_stream.h"
#include "media/base/media_export.h"
#include "media/base/pipeline_status.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
#include "base/containers/flat_map.h"
#include "media/base/action_reason.h"
#endif // OHOS_CUSTOM_VIDEO_PLAYER

namespace media {

class CdmContext;
class MediaResource;
class RendererClient;

// Types of media::Renderer.
// WARNING: These values are reported to metrics. Entries should not be
// renumbered and numeric values should not be reused. When adding new entries,
// also update media::mojom::RendererType & tools/metrics/histograms/enums.xml.
enum class RendererType {
  kRendererImpl = 0,     // RendererImplFactory
  kMojo = 1,             // MojoRendererFactory
  kMediaPlayer = 2,      // MediaPlayerRendererClientFactory
  kCourier = 3,          // CourierRendererFactory
  kFlinging = 4,         // FlingingRendererClientFactory
  kCast = 5,             // CastRendererClientFactory
  kMediaFoundation = 6,  // MediaFoundationRendererClientFactory
  // kFuchsia = 7,       // Deprecated
  kRemoting = 8,       // RemotingRendererFactory for remoting::Receiver
  kCastStreaming = 9,  // PlaybackCommandForwardingRendererFactory
  kContentEmbedderDefined = 10,  // Defined by the content embedder
  kTest = 11,                    // Renderer implementations used in tests
#if BUILDFLAG(IS_OHOS)
  kNative = 12,
#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  kOHOSCustomMediaPlayer,
#endif // OHOS_CUSTOM_VIDEO_PLAYER
  kOHOSMediaPlayer,      // OHOSMediaPlayerRendererFactory
  kMaxValue = kOHOSMediaPlayer,
#else
  kMaxValue = kTest,
#endif  // BUILDFLAG(IS_OHOS)
};

// Get the name of the Renderer for `renderer_type`. The returned name could be
// the actual Renderer class name or a descriptive name.
std::string MEDIA_EXPORT GetRendererName(RendererType renderer_type);

class MEDIA_EXPORT Renderer {
 public:
  Renderer();

  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  // Stops rendering and fires any pending callbacks.
  virtual ~Renderer();

  // Initializes the Renderer with |media_resource|, executing |init_cb| upon
  // completion. |media_resource| must be valid for the lifetime of the Renderer
  // object.  |init_cb| must only be run after this method has returned. Firing
  // |init_cb| may result in the immediate destruction of the caller, so it must
  // be run only prior to returning.
  virtual void Initialize(MediaResource* media_resource,
                          RendererClient* client,
#ifdef OHOS_VIDEO_ASSISTANT
                          RequestSurfaceCB request_surface_cb,
                          VideoDecoderChangedCB decoder_changed_cb,
#endif // OHOS_VIDEO_ASSISTANT
                          PipelineStatusCallback init_cb) = 0;

#if BUILDFLAG(IS_OHOS)
  virtual void Initialize(CreateTextureCB create_texture_cb,
                          DestroyTextureCB destroy_texture_cb);
#endif

#if defined(OHOS_MEDIA_POLICY)
  virtual void SetNativeWindowSurface(int native_window_id);
#endif // OHOS_MEDIA_POLICY

#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  virtual void SetSurfaceId(int surface_id, const gfx::Rect& rect);
  virtual void SetMediaPlayerState(bool is_suspend, int suspend_type);
#endif // OHOS_CUSTOM_VIDEO_PLAYER

  // Associates the |cdm_context| with this Renderer for decryption (and
  // decoding) of media data, then fires |cdm_attached_cb| with whether the
  // operation succeeded.
  using CdmAttachedCB = base::OnceCallback<void(bool)>;
  virtual void SetCdm(CdmContext* cdm_context, CdmAttachedCB cdm_attached_cb);

  // Specifies a latency hint from the site. Renderers should clamp the hint
  // value to reasonable min and max and use the resulting value as a target
  // latency such that the buffering state reaches HAVE_ENOUGH when this amount
  // of decoded data is buffered. A nullopt hint indicates the user is clearing
  // their preference and the renderer should restore its default buffering
  // thresholds.
  virtual void SetLatencyHint(absl::optional<base::TimeDelta> latency_hint) = 0;

  // Sets whether pitch adjustment should be applied when the playback rate is
  // different than 1.0.
  virtual void SetPreservesPitch(bool preserves_pitch);

  // Sets a flag indicating whether the audio stream was played with user
  // activation.
  virtual void SetWasPlayedWithUserActivation(
      bool was_played_with_user_activation);

  // The following functions must be called after Initialize().

  // Discards any buffered data, executing |flush_cb| when completed.
  virtual void Flush(base::OnceClosure flush_cb) = 0;

  // Starts rendering from |time|.
  virtual void StartPlayingFrom(base::TimeDelta time) = 0;

  // Updates the current playback rate. The default playback rate should be 0.
  virtual void SetPlaybackRate(double playback_rate) = 0;

  // Sets the output volume. The default volume should be 1.
  virtual void SetVolume(float volume) = 0;

  // Returns the current media time.
  //
  // This method must be safe to call from any thread.
  virtual base::TimeDelta GetMediaTime() = 0;

  // Provides a list of DemuxerStreams correlating to the tracks which should
  // be played. An empty list would mean that any playing track of the same
  // type should be flushed and disabled. Any provided Streams should be played
  // by whatever mechanism the subclass of Renderer choses for managing it's AV
  // playback.
  virtual void OnSelectedVideoTracksChanged(
      const std::vector<DemuxerStream*>& enabled_tracks,
      base::OnceClosure change_completed_cb);
  virtual void OnEnabledAudioTracksChanged(
      const std::vector<DemuxerStream*>& enabled_tracks,
      base::OnceClosure change_completed_cb);

  // Signal to the renderer that there has been a client request to access a
  // VideoFrame. This signal may be used by the renderer to ensure it is
  // operating in a mode which produces a VideoFrame usable by the client.
  // E.g., the MediaFoundationRendererClient on Windows has two modes
  // of operation: Frame Server & Direct Composition. Direct Composition mode
  // does not produce a VideoFrame with an accessible 'data' buffer, so clients
  // cannot access the underlying image data. In order for
  // MediaFoundationRendererClient to produce a VideoFrame with 'data'
  // accessible by the client it must switch to operate in Frame Server mode.
  virtual void OnExternalVideoFrameRequest();

  // Returns the type of the Renderer implementation. Marked as pure virtual to
  // enforce RendererType registration for all Renderer implementations.
  // Note: New implementation should update RendererType.
  virtual RendererType GetRendererType() = 0;

#if defined(OHOS_MEDIA_POLICY)
  using OnGetRectCallback = base::RepeatingCallback<void(const gfx::Rect&)>; 
  using NativeWindowCreatedCallback = base::OnceCallback<void(OnGetRectCallback)>;
  virtual void SetNativeWindowCreatedCallback(NativeWindowCreatedCallback cb) {}
#endif // OHOS_MEDIA_POLICY

#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  struct MediaSourceInfo {
    std::string media_source;
    std::string media_format;
  };

  virtual void SetMediaSourceList(const std::vector<MediaSourceInfo>& source_infos) {}
  virtual void SetMediaControls(bool show_media_controls,
      const std::vector<std::string>& controls_list) {}
  virtual void SetMuted(bool muted) {}
  virtual void SetPoster(const std::string& poster_url) {}
  virtual void SetAttributes(base::flat_map<std::string, std::string> attributes) {}
  virtual void SetReferrer(const std::string& referrer) {}
  using SurfaceCreatedCallback = base::OnceCallback<void(int, OnGetRectCallback)>;
  virtual void SetSurfaceCreatedCallback(SurfaceCreatedCallback cb) {}
  using UpdatePlaybackStatusCallback = base::RepeatingCallback<void(uint32_t)>;
  virtual void SetUpdatePlaybackStatusCallback(UpdatePlaybackStatusCallback cb) {}
  using UpdateVolumeCallback = base::RepeatingCallback<void(double)>;
  virtual void SetUpdateVolumeCallback(UpdateVolumeCallback cb) {}
  using UpdateMutedCallback = base::RepeatingCallback<void(bool)>;
  virtual void SetUpdateMutedCallback(UpdateMutedCallback cb) {}
  using UpdatePlaybackRateCallback = base::RepeatingCallback<void(double)>;
  virtual void SetUpdatePlaybackRateCallback(UpdatePlaybackRateCallback cb) {}

  virtual void SetIsAudio(bool is_audio) {}

  virtual void SetPlaybackRateWithReason(double playback_rate, ActionReason reason) {}
#endif // OHOS_CUSTOM_VIDEO_PLAYER
#ifdef OHOS_VIDEO_ASSISTANT
  virtual void SetVideoSurface(int32_t surface_id) {}
#endif // OHOS_VIDEO_ASSISTANT
};

}  // namespace media

#endif  // MEDIA_BASE_RENDERER_H_
