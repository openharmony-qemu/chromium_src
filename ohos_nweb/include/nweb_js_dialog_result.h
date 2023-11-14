// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NWEB_JS_DISLOG_RESULT_H
#define NWEB_JS_DISLOG_RESULT_H

#include <string>
#include "nweb_export.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebJSDialogResult {
public:
    virtual ~NWebJSDialogResult() = default;
    /**
     * @brief Handle a confirmation response from the user.
     */
    virtual void Confirm() = 0;
    /**
     * @brief Handle a confirmation response from the user with input message.
     *
     * @param message confirm message.
     */
    virtual void Confirm(const std::string &message) = 0;
    /**
     * @brief Handle the result if the user cancelled the dialog.
     */
    virtual void Cancel() = 0;
};
}

#endif