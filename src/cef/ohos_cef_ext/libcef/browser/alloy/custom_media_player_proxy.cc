
#include "cef/ohos_cef_ext/libcef/browser/alloy/custom_media_player_proxy.h"

CustomMediaPlayerProxy::CustomMediaPlayerProxy(
    CefOwnPtr<CefCustomMediaPlayerDelegate> delegate)
    : delegate_(std::move(delegate)) {}
CustomMediaPlayerProxy::~CustomMediaPlayerProxy() = default;

void CustomMediaPlayerProxy::UpdateLayerRect(int x,
                                             int y,
                                             int width,
                                             int height) {
  if (delegate_) {
    delegate_->UpdateLayerRect(x, y, width, height);
  }
}
void CustomMediaPlayerProxy::Play() {
  if (delegate_) {
    delegate_->Play();
  }
}
void CustomMediaPlayerProxy::Pause() {
  if (delegate_) {
    delegate_->Pause();
  }
}
void CustomMediaPlayerProxy::Seek(double target_time) {
  if (delegate_) {
    delegate_->Seek(target_time);
  }
}
void CustomMediaPlayerProxy::SetVolume(double volume) {
  if (delegate_) {
    delegate_->SetVolume(volume);
  }
}
void CustomMediaPlayerProxy::SetMuted(bool muted) {
  if (delegate_) {
    delegate_->SetMuted(muted);
  }
}
void CustomMediaPlayerProxy::SetPlaybackRate(double rate) {
  if (delegate_) {
    delegate_->SetPlaybackRate(rate);
  }
}
void CustomMediaPlayerProxy::Release() {
  if (delegate_) {
    delegate_->Release();
  }
}
void CustomMediaPlayerProxy::EnterFullscreen() {
  if (delegate_) {
    delegate_->EnterFullscreen();
  }
}
void CustomMediaPlayerProxy::ExitFullscreen() {
  if (delegate_) {
    delegate_->ExitFullscreen();
  }
}
void CustomMediaPlayerProxy::ResumeMediaPlayer() {
  if (delegate_) {
    delegate_->ResumeMediaPlayer();
  }
}
void CustomMediaPlayerProxy::SuspendMediaPlayer(int suspend_type) {
  if (delegate_) {
    delegate_->SuspendMediaPlayer(suspend_type);
  }
}
