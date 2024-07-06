
#include "custom_media_player_impl.h"

#include "base/logging.h"

namespace OHOS::NWeb {

CustomMediaPlayerImpl::CustomMediaPlayerImpl(
    std::shared_ptr<NWebNativeMediaPlayerBridge> bridge)
    : bridge_(std::move(bridge)) {}

CustomMediaPlayerImpl::~CustomMediaPlayerImpl() {}

void CustomMediaPlayerImpl::UpdateLayerRect(int x, int y, int width, int height) {
  if (bridge_) {
    bridge_->UpdateRect(x, y, width, height);
  }
}
void CustomMediaPlayerImpl::Play() {
  LOG(INFO) << "CustomMediaPlayerImpl::Play, bridge_[" << (!!bridge_) << "]";
  if (bridge_) {
    bridge_->Play();
  }
}
void CustomMediaPlayerImpl::Pause() {
  LOG(INFO) << "CustomMediaPlayerImpl::Pause, bridge_[" << (!!bridge_) << "]";
  if (bridge_) {
    bridge_->Pause();
  }
}
void CustomMediaPlayerImpl::Seek(double target_time) {
  LOG(INFO) << "CustomMediaPlayerImpl::Seek(" << target_time << "), bridge_[" << (!!bridge_) << "]";
  if (bridge_) {
    bridge_->Seek(target_time);
  }
}
void CustomMediaPlayerImpl::SetVolume(double volume) {
  LOG(INFO) << "CustomMediaPlayerImpl::SetVolume(" << volume << "), bridge_[" << (!!bridge_) << "]";
  if (bridge_) {
    bridge_->SetVolume(volume);
  }
}
void CustomMediaPlayerImpl::SetMuted(bool muted) {
  LOG(INFO) << "CustomMediaPlayerImpl::SetMuted(" << muted << "), bridge_[" << (!!bridge_) << "]";
  if (bridge_) {
    bridge_->SetMuted(muted);
  }
}
void CustomMediaPlayerImpl::SetPlaybackRate(double rate) {
  LOG(INFO) << "CustomMediaPlayerImpl::SetPlaybackRate(" << rate << "), bridge_[" << (!!bridge_) << "]";
  if (bridge_) {
    bridge_->SetPlaybackRate(rate);
  }
}
void CustomMediaPlayerImpl::Release() {
  LOG(INFO) << "CustomMediaPlayerImpl::Release, bridge_[" << (!!bridge_) << "]";
  if (bridge_) {
    bridge_->Release();
  }
}
void CustomMediaPlayerImpl::EnterFullscreen() {
  LOG(INFO) << "CustomMediaPlayerImpl::EnterFullscreen, bridge_[" << (!!bridge_) << "]";
  if (bridge_) {
    bridge_->EnterFullScreen();
  }
}
void CustomMediaPlayerImpl::ExitFullscreen() {
  LOG(INFO) << "CustomMediaPlayerImpl::ExitFullscreen, bridge_[" << (!!bridge_) << "]";
  if (bridge_) {
    bridge_->ExitFullScreen();
  }
}
void CustomMediaPlayerImpl::ResumeMediaPlayer() {
  LOG(INFO) << "CustomMediaPlayerImpl::ResumeMediaPlayer, bridge_[" << (!!bridge_) << "]";
  if (bridge_) {
    bridge_->ResumeMediaPlayer();
  }
}
void CustomMediaPlayerImpl::SuspendMediaPlayer(int suspend_type) {
  LOG(INFO) << "CustomMediaPlayerImpl::SuspendMediaPlayer, bridge_[" << (!!bridge_) << "]";
  if (bridge_) {
    bridge_->SuspendMediaPlayer(static_cast<SuspendType>(suspend_type));
  }
}

} // namespace
