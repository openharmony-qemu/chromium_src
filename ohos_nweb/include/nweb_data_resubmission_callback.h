// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NWEB_DATA_RESUBMISSION_CALLBACK_H
#define NWEB_DATA_RESUBMISSION_CALLBACK_H

#include <string>
#include "nweb_export.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebDataResubmissionCallback {
public:
    virtual ~NWebDataResubmissionCallback() = default;
    /**
     * @brief Resend the data.
     */
    virtual void Resend() = 0;
    /**
     * @brief Do not resend data.
     */
    virtual void Cancel() = 0;
};
}

#endif // NWEB_DATA_RESUBMISSION_CALLBACK_H