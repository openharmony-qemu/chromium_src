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

#ifndef ARK_WEB_BASE_REF_COUNTED_H_
#define ARK_WEB_BASE_REF_COUNTED_H_
#pragma once

#include <atomic>
#include <memory>

namespace OHOS::ArkWeb {

///
/// Class that implements atomic reference counting.
///
class ArkWebRefCount {
public:
    ArkWebRefCount() : ref_count_(0) {}

    ArkWebRefCount(const ArkWebRefCount&) = delete;
    ArkWebRefCount& operator=(const ArkWebRefCount&) = delete;

    /**
     * @brief The reference count increment 1.
     */
    void IncreRef() const
    {
        ref_count_.fetch_add(1, std::memory_order_relaxed);
    }

    /**
     * @brief The reference count decrement 1 and returns true if the reference
     *        count is 0.
     */
    bool DecreRef() const
    {
        return ref_count_.fetch_sub(1, std::memory_order_acq_rel) == 1;
    }

    /**
     * @brief Returns the reference count.
     */
    int GetRefCount() const
    {
        return ref_count_.load(std::memory_order_acquire);
    }

private:
    mutable std::atomic_int ref_count_;
};

///
/// All ref-counted framework classes must extend this class.
///
class ArkWebBaseRefCounted {
public:
    /**
     * @brief The reference count increment 1. Should be called for every new copy
     *        of a pointer to a given object.
     */
    virtual void IncreRef() const = 0;

    /**
     * @brief The reference count decrement 1 and delete the object when the
     *        reference count is 0
     */
    virtual void DecreRef() const = 0;

protected:
    virtual ~ArkWebBaseRefCounted() {}
};

///
/// Macro that provides a reference counting implementation for classes
/// extending ArkWebBaseRefCounted.
///
#define IMPLEMENT_REFCOUNTING(ClassName)                \
public:                                                 \
    void IncreRef() const override                      \
    {                                                   \
        ref_count_.IncreRef();                          \
    }                                                   \
    void DecreRef() const override                      \
    {                                                   \
        if (ref_count_.DecreRef()) {                    \
            delete static_cast<const ClassName*>(this); \
        }                                               \
    }                                                   \
                                                        \
private:                                                \
    ArkWebRefCount ref_count_

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_BASE_REF_COUNTED_H_
