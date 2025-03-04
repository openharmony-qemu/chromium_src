
#ifndef CONTENT_RENDERER_MEDIA_OHOS_OHOS_CUSTOM_MEDIA_PLAYER_RENDERER_CLIENT_FACTORY_H_
#define CONTENT_RENDERER_MEDIA_OHOS_OHOS_CUSTOM_MEDIA_PLAYER_RENDERER_CLIENT_FACTORY_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "content/common/content_export.h"
#include "media/base/native_texture_wrapper.h"
#include "media/base/renderer_factory.h"
#include "media/mojo/clients/mojo_renderer_factory.h"

namespace media {
class MojoRendererFactory;
}

namespace content {

class CONTENT_EXPORT OHOSCustomMediaPlayerRendererClientFactory
    : public media::RendererFactory {
 public:
  using GetNativeTextureWrapperCB =
      base::RepeatingCallback<media::ScopedNativeTextureWrapper()>;

  OHOSCustomMediaPlayerRendererClientFactory(
      scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
      std::unique_ptr<media::MojoRendererFactory> mojo_renderer_factory,
      const GetNativeTextureWrapperCB& get_native_texture_wrapper_cb);

  ~OHOSCustomMediaPlayerRendererClientFactory() override;

  std::unique_ptr<media::Renderer> CreateRenderer(
      const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
      const scoped_refptr<base::TaskRunner>& worker_task_runner,
      media::AudioRendererSink* audio_renderer_sink,
      media::VideoRendererSink* video_renderer_sink,
      media::RequestOverlayInfoCB request_surface_cb,
      const gfx::ColorSpace& target_color_space) override;

  media::MediaResource::Type GetRequiredMediaResourceType() override;

  std::unique_ptr<media::Renderer> CreateCustomRenderer(
      const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
      const scoped_refptr<base::TaskRunner>& worker_task_runner,
      media::AudioRendererSink* audio_renderer_sink,
      media::VideoRendererSink* video_renderer_sink,
      media::RequestOverlayInfoCB request_surface_cb,
      const gfx::ColorSpace& target_color_space,
      int player_id) override;

 private:
  GetNativeTextureWrapperCB get_native_texture_wrapper_cb_;

  scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_;

  std::unique_ptr<media::MojoRendererFactory> mojo_renderer_factory_;
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_OHOS_OHOS_CUSTOM_MEDIA_PLAYER_RENDERER_CLIENT_FACTORY_H_
