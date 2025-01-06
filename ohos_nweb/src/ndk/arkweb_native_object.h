/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARKWEB_NATIVE_OBJECT_H
#define ARKWEB_NATIVE_OBJECT_H

#include <memory>

#include "nweb_impl.h"
#include "ohos_nweb/src/capi/arkweb_type.h"

namespace OHOS::NWeb {
class ArkWebNativeObject final
    : public std::enable_shared_from_this<ArkWebNativeObject> {
 public:
  using ArkWebLifeTimeFunc = std::function<void(void)>;
  using ArkWebScrollFunc = std::function<void(double, double)>;
  explicit ArkWebNativeObject(const char* webTag) : webTag_(webTag) {}
  ~ArkWebNativeObject() = default;
  void SetValidCallback(ArkWebLifeTimeFunc&& callback) {
    validCallback_ = std::move(callback);
  }
  bool FireValidCallback();
  void SetLoadStartCallback(ArkWebLifeTimeFunc&& callback) {
    loadStartCallback_ = std::move(callback);
  }
  void SetLoadEndCallback(ArkWebLifeTimeFunc&& callback) {
    loadEndCallback_ = std::move(callback);
  }
  void SetDestroyCallback(ArkWebLifeTimeFunc&& callback) {
    destroyCallback_ = std::move(callback);
  }
  void SetScrollCallback(ArkWebScrollFunc&& callback) {
    scrollCallback_ = std::move(callback);
  }
  bool FireLoadStartCallback();
  bool FireLoadEndCallback();
  bool FireDestroyCallback();
  bool FireScrollCallback(double x, double y);
  bool SetWebWeakPtr(std::shared_ptr<NWebImpl>& nwebSharedPtr);
  std::shared_ptr<NWebImpl> GetWebSharedPtr();
  std::weak_ptr<ArkWebNativeObject> GetWeakPtr();

  static void BindWebTagToWebInstance(int32_t id, const char* webTag);
  static const std::shared_ptr<OHOS::NWeb::ArkWebNativeObject>
  GetWebInstanceByWebTag(const char* webTag);

 private:
  std::string webTag_ = std::string();
  ArkWebLifeTimeFunc validCallback_ = nullptr;
  ArkWebLifeTimeFunc loadStartCallback_ = nullptr;
  ArkWebLifeTimeFunc loadEndCallback_ = nullptr;
  ArkWebLifeTimeFunc destroyCallback_ = nullptr;
  ArkWebScrollFunc scrollCallback_ = nullptr;
  std::weak_ptr<NWebImpl> nwebWeakPtr_;
};
}  // namespace OHOS::NWeb

#endif  // ARKWEB_NATIVE_OBJECT_H