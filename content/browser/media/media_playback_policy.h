// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_MEDIA_MEDIA_PLAYBACK_POLICY_H_
#define CONTENT_BROWSER_MEDIA_MEDIA_PLAYBACK_POLICY_H_

#include "base/no_destructor.h"
#include "base/observer_list.h"
#include "base/scoped_observation.h"

namespace content {

class MediaPlaybackPolicy final {
 public:
  class Observer {
    public:
      virtual ~Observer() {}
      virtual void OnPlaybackWithMobileDataAllowedPolicyChanged() {}
  };
  static MediaPlaybackPolicy* Instance();

  void AddObserver(Observer*);
  void RemoveObserver(Observer*);

  void AllowPlaybacWithMobileData();
  bool IsPlaybacWithMobileDataAllowed();

 private:
  friend class base::NoDestructor<MediaPlaybackPolicy>;

  MediaPlaybackPolicy();
  ~MediaPlaybackPolicy();

  bool playback_with_mobile_data_allowed_ = false;

  base::ObserverList<Observer>::Unchecked observers_;
};

} // namespace

#endif // CONTENT_BROWSER_MEDIA_MEDIA_PLAYBACK_POLICY_H_
