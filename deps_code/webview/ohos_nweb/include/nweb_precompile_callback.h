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

#ifndef NWEB_PRECOMPILE_CALLBACK_H
#define NWEB_PRECOMPILE_CALLBACK_H

#include "nweb_value_callback.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebPrecompileCallback : public NWebMessageValueCallback {
public:
    NWebPrecompileCallback() = default;
    ~NWebPrecompileCallback() = default;

    void OnReceiveValue(std::shared_ptr<NWebMessage> value) override {
        if (callback_) {
            callback_(value->GetInt64());
        }
    }

    void OnReceiveValueV2(std::shared_ptr<NWebHapValue> value) override {
        if (callback_) {
            callback_(value->GetInt());
        }
    }

    void SetCallback(const std::function<void(int64_t)> &&callback) {
        callback_ = callback;
    }

private:
    std::function<void(int64_t)> callback_;
};
}  // namespace OHOS::NWeb
#endif // NWEB_PRECOMPILE_CALLBACK_H