// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 
#ifndef NWEB_KEY_EVENT_H
#define NWEB_KEY_EVENT_H
 
#include <string>
#include "nweb_export.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebKeyEvent {
public:
    enum KeyEventAction {
        KEY_DOWN_ACTION = 0,
        KEY_UP_ACTION
    };

    NWebKeyEvent() = default;

    virtual ~NWebKeyEvent() = default;

    int32_t keyCode_;

    int32_t action_;
};
} // namespace OHOS::NWeb

#endif // NWEB_KEY_EVENT_H
