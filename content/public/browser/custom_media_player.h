
#ifndef CONTENT_PUBLIC_BROWSER_CUSTOM_MEDIA_PLAYER_H_
#define CONTENT_PUBLIC_BROWSER_CUSTOM_MEDIA_PLAYER_H_

#include <cstdint>
#include <string>

namespace content {

class CustomMediaPlayer {
 public:
  virtual ~CustomMediaPlayer() = default;

  virtual int32_t Initialize(const std::string& surface_id);
  virtual void UpdateLayerRect(int x, int y, int width, int height);
  virtual void Play();
  virtual void Pause();
  virtual void Seek(double target_time);
  virtual void SetVolume(double volume);
  virtual void SetMuted(bool muted);
  virtual void SetPlaybackRate(double rate);
  virtual void Release();
  virtual void EnterFullscreen();
  virtual void ExitFullscreen();
};

} // namespace

#endif // CONTENT_PUBLIC_BROWSER_CUSTOM_MEDIA_PLAYER_H_
