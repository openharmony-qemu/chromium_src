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

#include <cstdint>
#include <memory>
#include <string>
#include "webview_utils.h"

namespace OHOS {
namespace Webview {
    char* MallocCString(const std::string& origin)
    {
        if (origin.empty()) {
            return nullptr;
        }
        auto len = origin.length() + 1;
        char* res = static_cast<char*>(malloc(sizeof(char) * len));
        if (res == nullptr) {
            return nullptr;
        }
        return std::char_traits<char>::copy(res, origin.c_str(), len);
    }

    uint8_t* MallocUInt8(const std::string& origin)
    {
        auto len = origin.length();
        if (len <= 0) {
            return nullptr;
        }
        char* res = static_cast<char*>(malloc(sizeof(char) * len));
        if (res == nullptr) {
            return nullptr;
        }
        return reinterpret_cast<uint8_t*>(std::char_traits<char>::copy(res, origin.c_str(), len));
    }

    char** VectorToCArrString(const std::vector<std::string>& vec)
    {
        if (vec.size() == 0) {
            return nullptr;
        }
        char** result = static_cast<char**>(malloc(sizeof(char*) * vec.size()));
        if (result == nullptr) {
            return nullptr;
        }
        for (size_t i = 0; i < vec.size(); i++) {
            result[i] = MallocCString(vec[i]);
        }
        return result;
    }

    uint8_t* VectorToCArrUI8(const std::vector<uint8_t> vec)
    {
        if (vec.size() == 0) {
            return nullptr;
        }
        uint8_t* result = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * vec.size()));
        if (result == nullptr) {
            return nullptr;
        }
        for (size_t i = 0; i < vec.size(); i++) {
            result[i] = vec[i];
        }
        return result;
    }

    std::vector<std::string> CArrStringToVector(CArrString cArrStr)
    {
        std::vector<std::string> vec;
        if (cArrStr.head == nullptr || cArrStr.size <= 0) {
            return vec;
        }
        for (int64_t i = 0; i < cArrStr.size; i++) {
            if (cArrStr.head[i] != nullptr) {
                vec.emplace_back(cArrStr.head[i]);
            }
        }
        return vec;
    }
}
}