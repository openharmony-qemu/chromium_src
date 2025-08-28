/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef CALLBACK_SHARED_WRAPPER_H
#define CALLBACK_SHARED_WRAPPER_H

#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace OHOS::NWeb {

// The class is designed for security reinforcement of NDK interface callback functions,
// with template parameters accepting pointer types of the std::shared_ptr.
template <typename T>
class CallbackSharedWrapper {
public:
    CallbackSharedWrapper();

    ~CallbackSharedWrapper();

    // Add the std::shared_ptr pointer with Callback to the map.
    size_t AddCallback(const std::shared_ptr<T>& callback);

    // Clear the Callback at the specified index in the map.
    void Clear(size_t index);

    // Get the Callback at the specified index in the map.
    std::shared_ptr<T> GetCallback(size_t index);

    // Get the length of the map.
    size_t Size() const;
private:
    using CallbackMap = std::map<size_t, std::shared_ptr<T>>;
    CallbackMap callback_map_;

    size_t NextIndex() {
        return nextIndex++;
    }
    static size_t nextIndex;

    mutable std::shared_mutex mtx_;
};

template <typename T>
size_t CallbackSharedWrapper<T>::nextIndex = 1;

template <typename T>
CallbackSharedWrapper<T>::CallbackSharedWrapper() {
}

template <typename T>
CallbackSharedWrapper<T>::~CallbackSharedWrapper() {
}

template <typename T>
size_t CallbackSharedWrapper<T>::AddCallback(const std::shared_ptr<T>& callback) {
    std::unique_lock<std::shared_mutex> lock(mtx_);
    if (!callback) {
        return 0;
    }
    size_t index = NextIndex();
    callback_map_[index] = callback;
    return index;
}

template <typename T>
void CallbackSharedWrapper<T>::Clear(size_t index) {
    std::unique_lock<std::shared_mutex> lock(mtx_);
    callback_map_.erase(index);
}

template <typename T>
std::shared_ptr<T> CallbackSharedWrapper<T>::GetCallback(size_t index) {
    std::shared_lock<std::shared_mutex> lock(mtx_);
    auto callback_map_iter = callback_map_.find(index);
    if (callback_map_iter != callback_map_.end()) {
        return callback_map_iter->second;
    }
    return nullptr;
}

template <typename T>
size_t CallbackSharedWrapper<T>::Size() const {
    std::shared_lock<std::shared_mutex> lock(mtx_);
    return callback_map_.size();
}

} // namespace OHOS::NWeb

#endif // CALLBACK_SHARED_WRAPPER_H