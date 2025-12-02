// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/notreached.h"
#include "chrome/browser/status_icons/status_tray.h"
#include "base/notimplemented.h"

std::unique_ptr<StatusTray> StatusTray::Create() {
  NOTIMPLEMENTED_LOG_ONCE();
  return nullptr;
}
