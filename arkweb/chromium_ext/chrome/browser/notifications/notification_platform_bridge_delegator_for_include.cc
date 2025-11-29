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

namespace {

const char kCrashedNotificationPrefix[] = "app.background.crashed.";
bool IsExtensionCrashNotification(const std::string& id) {
  return base::StartsWith(id, kCrashedNotificationPrefix);
}

}  // namespace

NotificationPlatformBridge*
NotificationPlatformBridgeDelegator::GetBridgeForType(
    NotificationHandler::Type type,
    const std::string& id) {
  if (system_bridge_ && type == NotificationHandler::Type::TRANSIENT &&
      IsExtensionCrashNotification(id)) {
    return system_bridge_;
  }
  return GetBridgeForType(type);
}
