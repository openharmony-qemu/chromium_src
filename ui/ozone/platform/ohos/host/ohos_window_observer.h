// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_WINDOW_OBSERVER_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_WINDOW_OBSERVER_H_

#include "base/observer_list_types.h"

namespace ui {

class OhosWindow;

// Observers for window management notifications.
class OhosWindowObserver : public base::CheckedObserver {
 public:
  // Called when |window| has been added.
  virtual void OnWindowAdded(OhosWindow* window);

  // Called when |window| has been removed.
  virtual void OnWindowRemoved(OhosWindow* window);

 protected:
  ~OhosWindowObserver() override;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_WINDOW_OBSERVER_H_
