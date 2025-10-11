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

#include <memory>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "arkweb/chromium_ext/chrome/browser/notifications/notification_platform_bridge_ohos.h"
#include "base/files/file_path.h"
#include "base/files/scoped_temp_dir.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/memory/ref_counted.h"
#include "chrome/test/base/testing_profile.h"
#include "content/public/test/browser_task_environment.h"
#include "ui/gfx/image/image_unittest_util.h"
#include "ui/message_center/public/cpp/notification_delegate.h"
#include "third_party/skia/include/core/SkAlphaType.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/codec/SkCodec.h"
#include "third_party/skia/include/core/SkData.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/encode/SkPngEncoder.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/core/SkUnPreMultiply.h"
#include "third_party/skia/include/core/SkBitmap.h"

namespace {
constexpr int IMAGE_SIZE = 1024;
constexpr int VAL_ONE = -1;
constexpr int VAL_TWO = 0;
}

class NotificationPlatformBridgeOhosTest: public testing::TestWithParam<base::FilePath> {};

TEST_F(NotificationPlatformBridgeOhosTest, CanHandleType) {
  std::unique_ptr<NotificationPlatformBridge> bridge = NotificationPlatformBridge::Create();
  ASSERT_TRUE(bridge);
  EXPECT_TRUE(bridge->CanHandleType(NotificationHandler::Type::WEB_PERSISTENT));
  EXPECT_TRUE(bridge->CanHandleType(NotificationHandler::Type::WEB_NON_PERSISTENT));
  EXPECT_TRUE(bridge->CanHandleType(NotificationHandler::Type::EXTENSION));
  EXPECT_FALSE(bridge->CanHandleType(NotificationHandler::Type::ANNOUNCEMENT));
}

TEST_F(NotificationPlatformBridgeOhosTest, Display001) {
  std::unique_ptr<NotificationPlatformBridge> bridge = NotificationPlatformBridge::Create();
  ASSERT_TRUE(bridge);

  std::string notification_key("notificationid");
  message_center::RichNotificationData optional_fields;
  optional_fields.settings_button_handler = message_center::SettingsButtonHandler::DELEGATE;

  SkBitmap bitmap;
  bool ret = bitmap.tryAllocPixels(SkImageInfo::Make(IMAGE_SIZE, IMAGE_SIZE,
                                kRGBA_8888_SkColorType, kOpaque_SkAlphaType));
  EXPECT_TRUE(ret);
  ui::ImageModel image_model = ui::ImageModel::FromImage(gfx::Image::CreateFrom1xBitmap(bitmap));

  message_center::Notification notification(
        message_center::NOTIFICATION_TYPE_CUSTOM,
        notification_key, u"title", u"message",
        image_model, u"arc", GURL(),
        message_center::NotifierId(message_center::NotifierType::ARC_APPLICATION, "ARC_NOTIFICATION"),
        optional_fields,
        nullptr);

  testing::internal::CaptureStderr();
  bridge->Display(NotificationHandler::Type::WEB_PERSISTENT, nullptr, notification, nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("id=notificationid"), std::string::npos);
}

TEST_F(NotificationPlatformBridgeOhosTest, Display002) {
  std::unique_ptr<NotificationPlatformBridge> bridge = NotificationPlatformBridge::Create();
  ASSERT_TRUE(bridge);

  std::string notification_key("notificationid");
  message_center::RichNotificationData optional_fields;
  optional_fields.settings_button_handler = message_center::SettingsButtonHandler::DELEGATE;

  SkBitmap bitmap;
  bool ret = bitmap.tryAllocPixels(SkImageInfo::Make(IMAGE_SIZE, IMAGE_SIZE,
                                kRGBA_8888_SkColorType, kPremul_SkAlphaType));
  EXPECT_TRUE(ret);
  ui::ImageModel image_model = ui::ImageModel::FromImage(gfx::Image::CreateFrom1xBitmap(bitmap));

  message_center::Notification notification(
        message_center::NOTIFICATION_TYPE_CUSTOM,
        notification_key, u"title", u"message",
        image_model, u"arc", GURL(),
        message_center::NotifierId(message_center::NotifierType::ARC_APPLICATION, "ARC_NOTIFICATION"),
        optional_fields,
        nullptr);

  testing::internal::CaptureStderr();
  bridge->Display(NotificationHandler::Type::WEB_PERSISTENT, nullptr, notification, nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("id=notificationid"), std::string::npos);
}

TEST_F(NotificationPlatformBridgeOhosTest, Display003) {
  std::unique_ptr<NotificationPlatformBridge> bridge = NotificationPlatformBridge::Create();
  ASSERT_TRUE(bridge);

  std::string notification_key("notificationid");
  message_center::RichNotificationData optional_fields;
  optional_fields.settings_button_handler = message_center::SettingsButtonHandler::DELEGATE;

  SkBitmap bitmap;
  bool ret = bitmap.tryAllocPixels(SkImageInfo::Make(IMAGE_SIZE, IMAGE_SIZE,
                                kRGBA_8888_SkColorType, kUnpremul_SkAlphaType));
  EXPECT_TRUE(ret);
  ui::ImageModel image_model = ui::ImageModel::FromImage(gfx::Image::CreateFrom1xBitmap(bitmap));

  message_center::Notification notification(
        message_center::NOTIFICATION_TYPE_CUSTOM,
        notification_key, u"title", u"message",
        image_model, u"arc", GURL(),
        message_center::NotifierId(message_center::NotifierType::ARC_APPLICATION, "ARC_NOTIFICATION"),
        optional_fields,
        nullptr);

  testing::internal::CaptureStderr();
  bridge->Display(NotificationHandler::Type::WEB_PERSISTENT, nullptr, notification, nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("id=notificationid"), std::string::npos);

  testing::internal::CaptureStderr();
  bridge->Close(nullptr ,"id");
  log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("id=id"), std::string::npos);

  testing::internal::CaptureStderr();
  bridge->Close(nullptr ,notification_key);
  log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("id=notificationid"), std::string::npos);
}

TEST_F(NotificationPlatformBridgeOhosTest, Display004) {
  std::unique_ptr<NotificationPlatformBridge> bridge = NotificationPlatformBridge::Create();
  ASSERT_TRUE(bridge);

  std::string notification_key("notificationid");
  message_center::RichNotificationData optional_fields;
  optional_fields.settings_button_handler = message_center::SettingsButtonHandler::DELEGATE;

  SkBitmap bitmap;
  bool ret = bitmap.tryAllocPixels(SkImageInfo::Make(IMAGE_SIZE, IMAGE_SIZE,
                                kRGBA_8888_SkColorType, kLastEnum_SkAlphaType));
  EXPECT_TRUE(ret);
  ui::ImageModel image_model = ui::ImageModel::FromImage(gfx::Image::CreateFrom1xBitmap(bitmap));

  message_center::Notification notification(
        message_center::NOTIFICATION_TYPE_CUSTOM,
        notification_key, u"title", u"message",
        image_model, u"arc", GURL(),
        message_center::NotifierId(message_center::NotifierType::ARC_APPLICATION, "ARC_NOTIFICATION"),
        optional_fields,
        nullptr);

  testing::internal::CaptureStderr();
  bridge->Display(NotificationHandler::Type::WEB_PERSISTENT, nullptr, notification, nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("id=notificationid"), std::string::npos);
}