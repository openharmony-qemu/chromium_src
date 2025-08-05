/* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef BASE_OHOS_MOCK_SYS_INFO_UTILS_EXT_H_
#define BASE_OHOS_MOCK_SYS_INFO_UTILS_EXT_H_ 

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace base {
namespace ohos {
class SystemPropertiesMock {
public:
    static SystemPropertiesMock& getInstance() {
        static SystemPropertiesMock instance;
        return instance;
    };
    MOCK_METHOD(bool, IsPcDeviceMock, (), ());
};

#ifdef __cplusplus
extern "C" {
#endif
bool __wrap_IsPcDevice() {
    return SystemPropertiesMock::getInstance().IsPcDeviceMock();
}
#ifdef __cplusplus
}
#endif
}
}

#endif // BASE_OHOS_NWEB_ENGINE_EVENT_LOGGER_CODE_H_