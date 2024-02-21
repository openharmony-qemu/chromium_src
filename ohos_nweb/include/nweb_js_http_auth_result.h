// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NWEB_JS_HTTP_AUTH_RESULT_H
#define NWEB_JS_HTTP_AUTH_RESULT_H

#include <string>
#include "nweb_export.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebJSHttpAuthResult {
public:
    virtual ~NWebJSHttpAuthResult() = default;
    /**
     * @brief Handle a confirmation response from the user.
     */
    virtual bool Confirm(const std::string& userName, const std::string& pwd) = 0;
    /**
     * @brief Handle the result if the user cancelled the dialog.
     */
    virtual void Cancel() = 0;
    /**
     * @brief Handle a IsHttpAuthInfoSaved response from the user.
     */
    virtual bool IsHttpAuthInfoSaved() = 0;
};
}

#endif