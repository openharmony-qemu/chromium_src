/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#ifndef OHOS_ADAPTER_COMMON_CALLBACK_SHARED_WRAPPER_H
#define OHOS_ADAPTER_COMMON_CALLBACK_SHARED_WRAPPER_H
 
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
 
namespace ohos::adapter::common {
 
// The class is designed for security reinforcement of NDK interface callback functions,
// with template parameters accepting pointer types of std::shared_ptr.
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
 
} // namespace  ohos::adapter::common
 
#endif // OHOS_ADAPTER_COMMON_CALLBACK_SHARED_WRAPPER_H