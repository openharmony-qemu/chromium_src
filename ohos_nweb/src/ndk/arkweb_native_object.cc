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

#include "arkweb_native_object.h"

#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "base/logging.h"
#include "nweb_impl.h"

namespace OHOS::NWeb {
static std::unordered_map<std::string,
                          std::shared_ptr<OHOS::NWeb::ArkWebNativeObject>>
    g_NativeWebMap;
static std::shared_mutex g_NativeWebMapSharedLock;

bool ArkWebNativeObject::FireValidCallback() {
  auto nwebSharedPtr = GetWebSharedPtr();
  if (nwebSharedPtr) {
    auto destroyCallback = [weak = GetWeakPtr()]() {
      if (auto webObjectPtr = weak.lock()) {
        webObjectPtr->FireDestroyCallback();
      }
    };
    nwebSharedPtr->RegisterNativeDestroyCallback(std::move(destroyCallback));

    auto loadStartCallback = [weak = GetWeakPtr()]() {
      if (auto webObjectPtr = weak.lock()) {
        webObjectPtr->FireLoadStartCallback();
      }
    };
    nwebSharedPtr->RegisterNativeLoadStartCallback(
        std::move(loadStartCallback));

    auto loadEndCallback = [weak = GetWeakPtr()]() {
      if (auto webObjectPtr = weak.lock()) {
        webObjectPtr->FireLoadEndCallback();
      }
    };
    nwebSharedPtr->RegisterNativeLoadEndCallback(std::move(loadEndCallback));
  } else {
    LOG(ERROR) << "NativeArkWeb nweb shared pointer is nullptr";
  }

  if (!validCallback_) {
    LOG(ERROR) << "NativeArkWeb valid callback is nullptr";
    return false;
  }
  validCallback_();
  return true;
}

bool ArkWebNativeObject::FireLoadStartCallback() {
  if (!loadStartCallback_) {
    LOG(ERROR) << "NativeArkWeb load start callback is nullptr";
    return false;
  }
  loadStartCallback_();
  return true;
}

bool ArkWebNativeObject::FireLoadEndCallback() {
  if (!loadEndCallback_) {
    LOG(ERROR) << "NativeArkWeb load end callback is nullptr";
    return false;
  }
  loadEndCallback_();
  return true;
}

bool ArkWebNativeObject::FireDestroyCallback() {
  if (!destroyCallback_) {
    LOG(ERROR) << "NativeArkWeb destroy callback is nullptr";
    return false;
  }
  destroyCallback_();

  std::unique_lock<std::shared_mutex> lock(g_NativeWebMapSharedLock);
  g_NativeWebMap.erase(webTag_);

  return true;
}

bool ArkWebNativeObject::SetWebWeakPtr(
    std::shared_ptr<NWebImpl>& nwebSharedPtr) {
  if (!nwebSharedPtr) {
    LOG(ERROR) << "NativeArkWeb nweb shared pointer is nullptr";
    return false;
  }
  nwebWeakPtr_ = nwebSharedPtr;
  return true;
}

std::shared_ptr<NWebImpl> ArkWebNativeObject::GetWebSharedPtr() {
  if (auto nwebSharedPtr = nwebWeakPtr_.lock()) {
    return nwebSharedPtr;
  }
  return nullptr;
}

std::weak_ptr<ArkWebNativeObject> ArkWebNativeObject::GetWeakPtr() {
  return shared_from_this();
}

void ArkWebNativeObject::BindWebTagToWebInstance(int32_t id,
                                                 const char* webTag) {
  LOG(INFO) << "NativeArkWeb BindWebTagToWebInstance webTag:" << webTag;

  if (auto webObject = ArkWebNativeObject::GetWebInstanceByWebTag(webTag);
      webObject != nullptr) {
    std::shared_ptr<NWebImpl> nwebShared = NWebImpl::GetNWebSharedPtr(id);
    webObject->SetWebWeakPtr(nwebShared);
    webObject->FireValidCallback();
  } else {
    std::unique_lock<std::shared_mutex> lock(g_NativeWebMapSharedLock);
    g_NativeWebMap[webTag] = std::make_shared<ArkWebNativeObject>(webTag);
  }
}

const std::shared_ptr<OHOS::NWeb::ArkWebNativeObject>
ArkWebNativeObject::GetWebInstanceByWebTag(const char* webTag) {
  if (!webTag) {
    LOG(ERROR) << "NativeArkWeb GetWebInstanceByWebTag webTag is null";
    return nullptr;
  }

  std::shared_lock<std::shared_mutex> lock(g_NativeWebMapSharedLock);
  if (auto it = g_NativeWebMap.find(webTag); it != g_NativeWebMap.end()) {
    return it->second;
  }
  return nullptr;
}

}  // namespace OHOS::NWeb