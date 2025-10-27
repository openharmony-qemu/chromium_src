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

#ifndef ARK_WEB_SCOPED_REFPTR_H_
#define ARK_WEB_SCOPED_REFPTR_H_

template<class T>
class ark_web_scoped_refptr {
public:
    ark_web_scoped_refptr() = default;

    ark_web_scoped_refptr(T* p) : ptr_(p)
    {
        if (ptr_) {
            ptr_->IncreRef();
        }
    }

    ark_web_scoped_refptr(const ark_web_scoped_refptr& r) : ark_web_scoped_refptr(r.ptr_) {}

    ~ark_web_scoped_refptr()
    {
        if (ptr_) {
            ptr_->DecreRef();
        }
    }

    T* get() const
    {
        return ptr_;
    }

    T* operator->() const
    {
        return ptr_;
    }

    explicit operator bool() const
    {
        return ptr_ != nullptr;
    }

    ark_web_scoped_refptr& operator=(std::nullptr_t)
    {
        return *this;
    }

    ark_web_scoped_refptr& operator=(T* p)
    {
        return *this = ark_web_scoped_refptr(p);
    }

    ark_web_scoped_refptr& operator=(ark_web_scoped_refptr r) noexcept
    {
        std::swap(ptr_, r.ptr_);
        return *this;
    }

private:
    T* ptr_ = nullptr;
};

#endif // ARK_WEB_SCOPED_REFPTR_H_
