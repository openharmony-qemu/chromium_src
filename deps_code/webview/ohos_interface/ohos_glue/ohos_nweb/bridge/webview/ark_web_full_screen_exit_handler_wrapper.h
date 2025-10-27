/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_FULL_SCREEN_EXIT_HANDLER_WRAPPER_H_
#define ARK_WEB_FULL_SCREEN_EXIT_HANDLER_WRAPPER_H_
#pragma once

#include "include/nweb_full_screen_exit_handler.h"
#include "ohos_nweb/include/ark_web_full_screen_exit_handler.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkWebFullScreenExitHandlerWrapper : public OHOS::NWeb::NWebFullScreenExitHandler {
public:
    ArkWebFullScreenExitHandlerWrapper(ArkWebRefPtr<ArkWebFullScreenExitHandler> ark_web_full_screen_exit_handler);
    ~ArkWebFullScreenExitHandlerWrapper() = default;

    /**
     * @brief the handler to exit the full screen mode.
     */
    void ExitFullScreen() override;

private:
    ArkWebRefPtr<ArkWebFullScreenExitHandler> ark_web_full_screen_exit_handler_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_FULL_SCREEN_EXIT_HANDLER_WRAPPER_H_
