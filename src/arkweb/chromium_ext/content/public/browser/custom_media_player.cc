
#include "content/public/browser/custom_media_player.h"

namespace content {

int32_t CustomMediaPlayer::Initialize(const std::string& surface_id) {
  return 0;
}

void CustomMediaPlayer::UpdateLayerRect(int x, int y, int width, int height) {}
void CustomMediaPlayer::Play() {}
void CustomMediaPlayer::Pause() {}
void CustomMediaPlayer::Seek(double target_time) {}
void CustomMediaPlayer::SetVolume(double volume) {}
void CustomMediaPlayer::SetMuted(bool muted) {}
void CustomMediaPlayer::SetPlaybackRate(double rate) {}
void CustomMediaPlayer::Release() {}
void CustomMediaPlayer::EnterFullscreen() {}
void CustomMediaPlayer::ExitFullscreen() {}
void CustomMediaPlayer::ResumeMediaPlayer() {}
void CustomMediaPlayer::SuspendMediaPlayer(int suspend_type) {}

}  // namespace content
