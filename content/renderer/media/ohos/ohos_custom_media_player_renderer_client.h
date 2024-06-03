
#ifndef CONTENT_RENDERER_MEDIA_OHOS_OHOS_CUSTOM_MEDIA_PLAYER_RENDERER_CLIENT_H_
#define CONTENT_RENDERER_MEDIA_OHOS_OHOS_CUSTOM_MEDIA_PLAYER_RENDERER_CLIENT_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "content/common/content_export.h"
#include "media/base/native_texture_wrapper.h"
#include "media/base/media_resource.h"
#include "media/base/renderer.h"
#include "media/base/renderer_client.h"
#include "media/base/video_renderer_sink.h"
#include "media/mojo/clients/mojo_renderer.h"
#include "media/mojo/clients/mojo_renderer_wrapper.h"
#include "media/mojo/mojom/renderer_extensions.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"

namespace content {

class CONTENT_EXPORT OHOSCustomMediaPlayerRendererClient
    : public media::mojom::CustomMediaPlayerRendererClientExtension,
      public media::MojoRendererWrapper {
 public:
  using RendererExtension = media::mojom::MediaPlayerRendererExtension;
  using ClientExtension = media::mojom::CustomMediaPlayerRendererClientExtension;

  OHOSCustomMediaPlayerRendererClient(
      mojo::PendingRemote<RendererExtension> renderer_extension_remote,
      mojo::PendingReceiver<ClientExtension> client_extension_receiver,
      scoped_refptr<base::SequencedTaskRunner> media_taks_runner,
      scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
      std::unique_ptr<media::MojoRenderer> mojo_renderer,
      media::ScopedNativeTextureWrapper native_texture_wrapper,
      media::VideoRendererSink* sink);

  OHOSCustomMediaPlayerRendererClient(
      const OHOSCustomMediaPlayerRendererClient&) = delete;
  OHOSCustomMediaPlayerRendererClient& operator=(
      const OHOSCustomMediaPlayerRendererClient&) = delete;

  ~OHOSCustomMediaPlayerRendererClient() override;


  // media::Renderer implementation
  void Initialize(media::MediaResource* media_resource,
                  media::RendererClient* client,
                  media::PipelineStatusCallback init_cb) override;

  media::RendererType GetRendererType() override;

  void SetSurfaceCreatedCallback(
      media::Renderer::SurfaceCreatedCallback cb) override;
  void SetUpdatePlaybackStatusCallback(
      media::Renderer::UpdatePlaybackStatusCallback cb) override;
  void SetUpdateVolumeCallback(
      media::Renderer::UpdateVolumeCallback cb) override;
  void SetUpdateMutedCallback(
      media::Renderer::UpdateMutedCallback cb) override;
  void SetUpdatePlaybackRateCallback(
      media::Renderer::UpdatePlaybackRateCallback cb) override;

  // media::mojom::CustomMediaPlayerRendererClientExtension implementation
  void OnVideoSizeChange(const gfx::Size& size) override;
  void OnDurationChange(base::TimeDelta duration) override;
  void UpdatePlaybackStatus(uint32_t status) override;
  void UpdateVolume(double volume) override;
  void UpdateMuted(bool muted) override;
  void UpdatePlaybackRate(double playback_rate) override;
  void UpdateBufferedEndTime(double buffered_time) override;

  void OnFrameAvailable();

 private:
  void OnStreamTextureWrapperInitialized(media::MediaResource* media_resource,
                                         bool success);
  void OnRemoteRendererInitialized(media::PipelineStatus status);
  void OnGetVideoRect(const gfx::Rect& rect);
  void OnSurfaceCreated(int surface_id);
  void OnSurfaceDestroyed();

  media::MediaResource* media_resource_;

  media::ScopedNativeTextureWrapper native_texture_wrapper_;

  media::RendererClient* client_;

  media::VideoRendererSink* sink_;

  scoped_refptr<base::SequencedTaskRunner> media_task_runner_;

  scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_;

  media::PipelineStatusCallback init_cb_;

  mojo::PendingReceiver<ClientExtension>
    delayed_bind_client_extension_receiver_;
  mojo::PendingRemote<RendererExtension>
    delayed_bind_renderer_extension_remote_;

  // Used to call methods on MediaPlayerRenderer in the browser process.
  mojo::Remote<RendererExtension> renderer_extension_remote_;

  // Used to receive events from MediaPlayerRenderer in the browser process.
  mojo::Receiver<CustomMediaPlayerRendererClientExtension> client_extension_receiver_{this};

  int surface_id_ = -1;
  bool has_sent_surface_id_to_remote_ = false;
  media::Renderer::SurfaceCreatedCallback surface_created_cb_;

  media::Renderer::UpdatePlaybackStatusCallback update_playback_status_cb_;
  media::Renderer::UpdateVolumeCallback update_volume_cb_;
  media::Renderer::UpdateMutedCallback update_muted_cb_;
  media::Renderer::UpdatePlaybackRateCallback update_playback_rate_cb_;

  // NOTE: Weak pointers must be invalidated before all other member variables.
  base::WeakPtrFactory<OHOSCustomMediaPlayerRendererClient> weak_factory_{this};
};

} // namespace

#endif // CONTENT_RENDERER_MEDIA_OHOS_OHOS_CUSTOM_MEDIA_PLAYER_RENDERER_CLIENT_H_
