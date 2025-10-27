
#ifndef CEF_LIBCEF_BROWSER_ALLOY_CUSTOM_MEDIA_PLAYER_PROXY_H_
#define CEF_LIBCEF_BROWSER_ALLOY_CUSTOM_MEDIA_PLAYER_PROXY_H_

#include "content/public/browser/custom_media_player.h"
#include "include/internal/cef_ptr.h"
#include "ohos_cef_ext/include/cef_custom_media_player_delegate.h"

class CustomMediaPlayerProxy : public content::CustomMediaPlayer {
 public:
  explicit CustomMediaPlayerProxy(
      CefOwnPtr<CefCustomMediaPlayerDelegate> delegate);
  ~CustomMediaPlayerProxy() override;

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
  void ResumeMediaPlayer() override;
  void SuspendMediaPlayer(int suspend_type) override;

 private:
  CefOwnPtr<CefCustomMediaPlayerDelegate> delegate_;
};

#endif  // CEF_LIBCEF_BROWSER_ALLOY_CUSTOM_MEDIA_PLAYER_PROXY_H_
