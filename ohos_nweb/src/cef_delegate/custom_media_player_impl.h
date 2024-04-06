
#ifndef OHOS_NWEB_SRC_CEF_DELEGATE_CUSTOM_MEDIA_PLAYER_IMPL_H_
#define OHOS_NWEB_SRC_CEF_DELEGATE_CUSTOM_MEDIA_PLAYER_IMPL_H_

#include "cef/include/cef_custom_media_player_delegate.h"
#include "cef/include/internal/cef_ptr.h"
#include "ohos_nweb/include/nweb_native_media_player.h"

namespace OHOS::NWeb {

class CustomMediaPlayerImpl : public CefCustomMediaPlayerDelegate {
 public:
  explicit CustomMediaPlayerImpl(std::shared_ptr<NWebNativeMediaPlayerBridge> bridge);
  ~CustomMediaPlayerImpl() override;

  void UpdateLayerRect(int x, int y, int width, int height) override;
  void Play() override;
  void Pause() override;
  void Seek(double target_time) override;
  void SetVolume(double volume) override;
  void SetMuted(bool muted) override;
  void SetPlaybackRate(double rate) override;
  void Release() override;
  void EnterFullscreen() override;
  void ExitFullscreen() override;

 private:
  std::shared_ptr<NWebNativeMediaPlayerBridge> bridge_;
};

} // namespace
#endif // OHOS_NWEB_SRC_CEF_DELEGATE_CUSTOM_MEDIA_PLAYER_IMPL_H_
