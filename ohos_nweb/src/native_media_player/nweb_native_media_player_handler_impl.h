
#ifndef OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_NATIVE_MEDIA_PLAYER_HANDLER_IMPL_H_
#define OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_NATIVE_MEDIA_PLAYER_HANDLER_IMPL_H_

#include "cef/include/cef_media_player_listener.h"
#include "nweb_native_media_player.h"

namespace OHOS::NWeb {

class NWebNativeMediaPlayerHandlerImpl : public NWebNativeMediaPlayerHandler {
 public:
  explicit NWebNativeMediaPlayerHandlerImpl(CefOwnPtr<CefMediaPlayerListener> listener);
  ~NWebNativeMediaPlayerHandlerImpl() override;

  void HandleStatusChanged(PlaybackStatus status) override;
  void HandleVolumeChanged(double volume) override;
  void HandleMutedChanged(bool muted) override;
  void HandlePlaybackRateChanged(double playback_rate) override;
  void HandleDurationChanged(double duration) override;
  void HandleTimeUpdate(double current_time) override;
  void HandleBufferedEndTimeChanged(double buffered_time) override;
  void HandleEnded() override;
  void HandleNetworkStateChanged(NetworkState state) override;
  void HandleReadyStateChanged(ReadyState state) override;
  void HandleFullScreenChanged(bool isFullScreen) override;
  void HandleSeeking() override;
  void HandleSeekFinished() override;
  void HandleError(MediaError error_code, const std::string& error_msg) override;
  void HandleVideoSizeChanged(double width, double height) override;
 private:
  CefOwnPtr<CefMediaPlayerListener> listener_;
};

} // namespace

#endif // OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_NATIVE_MEDIA_PLAYER_HANDLER_IMPL_H_
