/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef BASE_OHOS_LOGGER_H_
#define BASE_OHOS_LOGGER_H_

#include <memory>
#include <string>

#include "base/base_export.h"
#include "ohos_nweb/src/capi/nweb_logger_callback.h"

namespace ohos {

namespace logger {
BASE_EXPORT void SetMainProcessMode(bool is_main_process);
BASE_EXPORT void SetLoggerCallback(std::shared_ptr<NWebLoggerCallback> loggerCallback);
BASE_EXPORT void ReportRendererLog(int policy, const std::string& msg);
}  // namespace logger

}  // namespace ohos
#endif
