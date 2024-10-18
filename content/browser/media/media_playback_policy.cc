// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/media/media_playback_policy.h"

namespace content {

MediaPlaybackPolicy::MediaPlaybackPolicy() = default;
MediaPlaybackPolicy::~MediaPlaybackPolicy() = default;

// static
MediaPlaybackPolicy* MediaPlaybackPolicy::Instance() {
  static base::NoDestructor<MediaPlaybackPolicy> s_media_playback_policy;
  return s_media_playback_policy.get();
}

void MediaPlaybackPolicy::AddObserver(Observer* observer) {
  observers_.AddObserver(observer);
}

void MediaPlaybackPolicy::RemoveObserver(Observer* observer) {
  observers_.RemoveObserver(observer);
}

void MediaPlaybackPolicy::AllowPlaybacWithMobileData() {
  if (playback_with_mobile_data_allowed_) {
    return;
  }
  playback_with_mobile_data_allowed_ = true;
  for (auto& observer : observers_) {
    observer.OnPlaybackWithMobileDataAllowedPolicyChanged();
  }
}

bool MediaPlaybackPolicy::IsPlaybacWithMobileDataAllowed() {
  return playback_with_mobile_data_allowed_;
}

} // namespace
