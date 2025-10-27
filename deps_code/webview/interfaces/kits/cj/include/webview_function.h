/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WEBVIEW_FUNCTION_H
#define WEBVIEW_FUNCTION_H

#include <functional>
#include <unordered_map>

#include "ohos_adapter_helper.h"
#include "nweb_value_callback.h"
#include "ohos_init_web_adapter.h"

namespace OHOS {
namespace NWeb {

int32_t FfiOnce(char* cType, void (*callbackRef)(void));

void RegisterWebInitedCallback(std::function<void(void)> callback);

class WebRunInitedCallbackImpl : public WebRunInitedCallback {
public:
    explicit WebRunInitedCallbackImpl(std::function<void(void)> callback) : callbackRef_(callback) {}
    ~WebRunInitedCallbackImpl() override {}
    void RunInitedCallback() override;

private:
    std::function<void(void)> callbackRef_ = nullptr;
};

} // namespace NWeb
} // namespace OHOS

#endif // NWEB_NAPI_WEBVIEW_FUNCTION_H
