
#ifndef CEF_INCLUDE_CEF_CUSTOM_MEDIA_PLAYER_DELEGATE_H_
#define CEF_INCLUDE_CEF_CUSTOM_MEDIA_PLAYER_DELEGATE_H_

#include <cstdint>
#include <string>

class CefCustomMediaPlayerDelegate {
 public:
  virtual ~CefCustomMediaPlayerDelegate() = default;

  virtual void UpdateLayerRect(int x, int y, int width, int height) = 0;
  virtual void Play() = 0;
  virtual void Pause() = 0;
  virtual void Seek(double target_time) = 0;
  virtual void SetVolume(double volume) = 0;
  virtual void SetMuted(bool muted) = 0;
  virtual void SetPlaybackRate(double rate) = 0;
  virtual void Release() = 0;
  virtual void EnterFullscreen() = 0;
  virtual void ExitFullscreen() = 0;
  virtual void ResumeMediaPlayer() = 0;
  virtual void SuspendMediaPlayer(int suspend_type) = 0;
};

#endif  // CEF_INCLUDE_CEF_CUSTOM_MEDIA_PLAYER_DELEGATE_H_
