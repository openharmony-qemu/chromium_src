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

#include "webview_function.h"
#include "cj_lambda.h"

#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;

std::unordered_map<std::string, std::function<void(std::function<void(void)>)>> onceType = {
    {"webInited", RegisterWebInitedCallback},
};

int32_t FfiOnce(char* cType, void (*callbackRef)(void))
{
    std::string type = std::string(cType);
    if (onceType.find(type) == onceType.end()) {
        return PARAM_CHECK_ERROR;
    }
    std::function<void(void)> callback = CJLambda::Create(callbackRef);
    onceType.find(type)->second(callback);
    return NO_ERROR;
}

void RegisterWebInitedCallback(std::function<void(void)> callback)
{
    WebRunInitedCallback *runWebInitedCallbackObj = new (std::nothrow) WebRunInitedCallbackImpl(callback);
    if (runWebInitedCallbackObj == nullptr) {
        return;
    }
    OhosAdapterHelper::GetInstance().GetInitWebAdapter()->SetRunWebInitedCallback(std::move(runWebInitedCallbackObj));
}

void WebRunInitedCallbackImpl::RunInitedCallback()
{
    callbackRef_();
}
} // namespace NWeb
} // namespace OHOS
