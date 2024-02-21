// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 
#ifndef NWEB_CONTROLLER_HANDLER_H
#define NWEB_CONTROLLER_HANDLER_H
 
#include <string>
#include "nweb_export.h"
 
namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebControllerHandler {
public:
    virtual ~NWebControllerHandler() = default;
 
    virtual void SetNWebHandlerById(int32_t nwebId) = 0;
 
    virtual int32_t GetNWebHandlerId() = 0;
 
    virtual bool IsFrist() = 0;
 
    virtual int32_t GetId() = 0;
};
}

#endif
