/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_ERRORS_H
#define NWEB_ERRORS_H

namespace OHOS::NWeb {
    enum NWebErrNo {
        NWEB_ERR = -1,
        NWEB_OK = 0,

        NWEB_INVALID_URL = 17100002,
        NWEB_INVALID_RESOURCE = 17100003,
        NWEB_FUNCTION_NOT_ENABLE = 17100004,
        NWEB_INVALID_COOKIE_VALUE = 17100005,
        NWEB_INVALID_ORIGIN = 17100011,

    };

    enum NWebNavigationEntryUpdateResult {
      UPDATE_OK = 0,
      ERR_WRONG_OFFSET = -1,
      ERR_OTHER = -2,
    };
}  // namespace OHOS::NWeb

#endif  // NWEB_ERRORS_H
