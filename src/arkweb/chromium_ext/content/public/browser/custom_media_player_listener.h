
#ifndef CONTENT_PUBLIC_BROWSER_CUSTOM_MEDIA_PLAYER_LISTENER_H_
#define CONTENT_PUBLIC_BROWSER_CUSTOM_MEDIA_PLAYER_LISTENER_H_

#include <cstdint>
#include <string>

namespace content {

class CustomMediaPlayerListener {
 public:
  enum class PlaybackStatus : uint32_t {
    PAUSED = 0,
    PLAYING,
  };
  enum class NetworkState : uint32_t {
    EMPTY = 0,
    IDLE,
    LOADING,
    NETWORK_ERROR,
  };
  enum class ReadyState : uint32_t {
    HAVE_NOTHING = 0,
    HAVE_METADATA,
    HAVE_CURRENT_DATA,
    HAVE_FUTURE_DATA,
    HAVE_ENOUGH_DATA,
  };
  enum class VideoError : uint32_t {
    NETWORK_ERROR = 1,
    FORMAT_ERROR,
    DECODE_ERROR,
  };

 public:
  virtual ~CustomMediaPlayerListener() = default;
  virtual void OnStatusChanged(uint32_t status) = 0;
  virtual void OnVolumeChanged(double volume) = 0;
  virtual void OnMutedChanged(bool muted) = 0;
  virtual void OnPlaybackRateChanged(double playback_rate) = 0;
  virtual void OnDurationChanged(double duration) = 0;
  virtual void OnTimeUpdate(double current_time) = 0;
  virtual void OnBufferedEndTimeChanged(double buffered_time) = 0;
  virtual void OnEnded() = 0;
  virtual void OnNetworkStateChanged(uint32_t state) = 0;
  virtual void OnReadyStateChanged(uint32_t state) = 0;
  virtual void OnFullscreenChanged(bool fullscreen) = 0;
  virtual void OnSeeking() = 0;
  virtual void OnSeekFinished() = 0;
  virtual void OnError(uint32_t error_code, const std::string& error_msg) = 0;
  virtual void OnVideoSizeChanged(int width, int height) = 0;
};

}  // namespace content

#endif  // CONTENT_PUBLIC_BROWSER_CUSTOM_MEDIA_PLAYER_LISTENER_H_
