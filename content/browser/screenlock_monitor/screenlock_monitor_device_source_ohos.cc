// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/screenlock_monitor/screenlock_monitor_device_source.h"
#include "ohos/adapter/screenlock_monitor/screenlock_monitor_adapter.h"

namespace content {

using ScreenlockMonitorAdapter = ohos::adapter::ScreenlockMonitorAdapter;

namespace {

void OnScreenlockNotificationReceived(const std::string& event_type) {
  ScreenlockMonitorSource::ScreenlockEvent screenlock_event;
  if (event_type == "locked") {
    screenlock_event = ScreenlockMonitorSource::SCREEN_LOCK_EVENT;
  } else if (event_type == "unlocked") {
    screenlock_event = ScreenlockMonitorSource::SCREEN_UNLOCK_EVENT;
  } else {
    return;
  }

  ScreenlockMonitorSource::ProcessScreenlockEvent(screenlock_event);
}

}  //  namespace

void ScreenlockMonitorDeviceSource::StartListeningForScreenlock() {
  ScreenlockMonitorAdapter::GetInstance().StartListeningForScreenlock(
      OnScreenlockNotificationReceived);
}

void ScreenlockMonitorDeviceSource::StopListeningForScreenlock() {
  ScreenlockMonitorAdapter::GetInstance().StopListeningForScreenlock();
}

}  // namespace content
