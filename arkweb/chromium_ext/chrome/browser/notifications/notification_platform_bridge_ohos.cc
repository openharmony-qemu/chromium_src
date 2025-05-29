/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "chrome/browser/notifications/notification_platform_bridge_ohos.h"

#include <algorithm>
#include <memory>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "base/barrier_closure.h"
#include "base/callback_list.h"
#include "base/containers/contains.h"
#include "base/environment.h"
#include "base/files/file_path.h"
#include "base/files/file_path_watcher.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/i18n/number_formatting.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/ref_counted_memory.h"
#include "base/strings/escape.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/sequenced_task_runner.h"
#include "base/time/time.h"
#include "base/version.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/lifetime/termination_notification.h"
#include "chrome/browser/notifications/notification_display_service_impl.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/themes/theme_service.h"
#include "chrome/browser/themes/theme_service_factory.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/notifications/notification_operation.h"
#include "chrome/grit/chrome_unscaled_resources.h"
#include "chrome/grit/generated_resources.h"
#include "components/dbus/thread_linux/dbus_thread_linux.h"
#include "components/url_formatter/elide_url.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "skia/ext/image_operations.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/image/image_skia_rep.h"
#include "ui/message_center/public/cpp/notification.h"
#if BUILDFLAG(ARKWEB_NOTIFICATION)
#include "ohos_nweb/src/nweb_notification_manager_delegate.h"
#include "ohos_nweb/src/capi/nweb_notification_options_item.h"
#endif // ARKWEB_NOTIFICATION

#if BUILDFLAG(ARKWEB_NOTIFICATION)
extern "C" {
void* __real_malloc(size_t);
}       // extern "C"
#endif

namespace {

std::map<std::string, std::unique_ptr<ProfileNotification>> g_profile_notifications_;

ProfileNotification* FindProfileNotification(
    const std::string& id) {
  auto iter = g_profile_notifications_.find(id);
  if (iter == g_profile_notifications_.end()) {
    return nullptr;
  }

  return (*iter).second.get();
}

void AddProfileNotification(const std::string& id,
    std::unique_ptr<ProfileNotification> profile_notification) {
  DCHECK(g_profile_notifications_.find(id) == g_profile_notifications_.end());
  g_profile_notifications_[id] = std::move(profile_notification);
}

void Add(const std::string& id, const message_center::Notification& notification,
         Profile* profile) {
  auto profile_notification_ptr =
      std::make_unique<ProfileNotification>(profile, notification);

  AddProfileNotification(id, std::move(profile_notification_ptr));
}

void RemoveProfileNotification(
    const std::string& notification_id) {
  auto it = g_profile_notifications_.find(notification_id);
  if (it == g_profile_notifications_.end()) {
    return;
  }
  g_profile_notifications_.erase(it);
}

bool CancelById(
    const std::string& id,
    ProfileNotification::ProfileID profile_id) {
  std::string profile_notification_id =
      ProfileNotification::GetProfileNotificationId(id, profile_id);

  auto iter = g_profile_notifications_.find(profile_notification_id);
  if (iter == g_profile_notifications_.end()) {
    return false;
  }

  RemoveProfileNotification(iter->first);
  return true;
}

// A NotificationDelegate that passes through click actions to the notification
// display service (and on to the appropriate handler). This is a temporary
// class to ease the transition from NotificationDelegate to
// NotificationHandler.
// TODO(estade): also handle other NotificationDelegate actions as needed.
class PassThroughDelegate : public message_center::NotificationDelegate {
 public:
  PassThroughDelegate(Profile* profile,
                      const message_center::Notification& notification,
                      NotificationHandler::Type notification_type)
      : profile_(profile),
        notification_(notification),
        notification_type_(notification_type) {
    DCHECK_NE(notification_type, NotificationHandler::Type::TRANSIENT);
  }
  PassThroughDelegate(const PassThroughDelegate&) = delete;
  PassThroughDelegate& operator=(const PassThroughDelegate&) = delete;

  void SettingsClick() override {
    NotificationDisplayServiceImpl::GetForProfile(profile_)
        ->ProcessNotificationOperation(
        NotificationOperation::kSettings, notification_type_,
        notification_.origin_url(), notification_.id(), std::nullopt,
        std::nullopt, std::nullopt /* by_user */, base::DoNothing());
  }

  void DisableNotification() override {
    NotificationDisplayServiceImpl::GetForProfile(profile_)
        ->ProcessNotificationOperation(
        NotificationOperation::kDisablePermission, notification_type_,
        notification_.origin_url(), notification_.id(),
        std::nullopt /* action_index */, std::nullopt /* reply */,
        std::nullopt /* by_user */, base::DoNothing());
  }

  void Close(bool by_user) override {
    NotificationDisplayServiceImpl::GetForProfile(profile_)
        ->ProcessNotificationOperation(
        NotificationOperation::kClose, notification_type_,
        notification_.origin_url(), notification_.id(),
        std::nullopt /* action_index */, std::nullopt /* reply */,
        by_user, base::DoNothing());
  }

  void Click(const std::optional<int>& button_index,
             const std::optional<std::u16string>& reply) override {
    NotificationDisplayServiceImpl::GetForProfile(profile_)
        ->ProcessNotificationOperation(
        NotificationOperation::kClick, notification_type_,
        notification_.origin_url(), notification_.id(), button_index, reply,
        std::nullopt /* by_user */, base::DoNothing());
  }

  raw_ptr<Profile> GetProfile() const { return profile_; }
  message_center::Notification GetNotification() const { return notification_; }
  NotificationHandler::Type GetNotificationType() const { return notification_type_; }

 protected:
  ~PassThroughDelegate() override = default;

 private:
  raw_ptr<Profile> profile_;
  message_center::Notification notification_;
  NotificationHandler::Type notification_type_;
};

#if BUILDFLAG(ARKWEB_NOTIFICATION)
NWebNotificationOptionsItemIconColorType GetColorTypeFromSkBitmap(
    const SkBitmap& bitmap) {
  NWebNotificationOptionsItemIconColorType colorType;

  switch (bitmap.colorType()) {
      case kRGBA_8888_SkColorType:
          colorType = NWebNotificationOptionsItemIconColorType::RGBA_8888;
          break;
      case kBGRA_8888_SkColorType:
          colorType = NWebNotificationOptionsItemIconColorType::BGRA_8888;
          break;
      default:
          colorType = NWebNotificationOptionsItemIconColorType::UNKNOWN;
          break;
  }
  return colorType;
}

NWebNotificationOptionsItemIconAlphaType GetAlphaTypeFromSkBitmap(
    const SkBitmap& bitmap) {
  NWebNotificationOptionsItemIconAlphaType alphaType;

  switch (bitmap.alphaType()) {
      case kOpaque_SkAlphaType:
        alphaType = NWebNotificationOptionsItemIconAlphaType::OPAQUE;
        break;
      case kPremul_SkAlphaType:
        alphaType = NWebNotificationOptionsItemIconAlphaType::PREMUL;
        break;
      case kUnpremul_SkAlphaType:
        alphaType = NWebNotificationOptionsItemIconAlphaType::UNPREMUL;
        break;
      default:
        alphaType = NWebNotificationOptionsItemIconAlphaType::UNKNOWN;
        break;
  }
  return alphaType;
}

NWebNotificationOptionsItemIconBitmap CreateIconBitmapFromImage(
    const SkBitmap& bitmap) {
  NWebNotificationOptionsItemIconBitmap iconBitmap;

  iconBitmap.colorType = GetColorTypeFromSkBitmap(bitmap);
  iconBitmap.alphaType = GetAlphaTypeFromSkBitmap(bitmap);
  iconBitmap.width = bitmap.width();
  iconBitmap.height = bitmap.height();
  iconBitmap.bitmap = static_cast<uint8_t *>(bitmap.getPixels());

  return iconBitmap;
}

NWebNotificationOptionsItemIcon CreateFromImageSkiaReps(
    const std::vector<gfx::ImageSkiaRep>& imageSkiaReps) {
  NWebNotificationOptionsItemIcon actionIcon;
  for (auto rep: imageSkiaReps) {
    double scale = rep.scale();
#if defined(ADDRESS_SANITIZER) || defined(HWADDRESS_SANITIZER)
    actionIcon.bitmaps[scale] =
        new NWebNotificationOptionsItemIconBitmap(
            CreateIconBitmapFromImage(rep.GetBitmap()));
#else
    NWebNotificationOptionsItemIconBitmap* addr =
        (NWebNotificationOptionsItemIconBitmap*)__real_malloc(
            sizeof(NWebNotificationOptionsItemIconBitmap));
    actionIcon.bitmaps[scale] =
        new (addr) NWebNotificationOptionsItemIconBitmap(
            CreateIconBitmapFromImage(rep.GetBitmap()));
#endif
  }
  return actionIcon;
}
#endif // ARKWEB_NOTIFICATION

}  // namespace

// static
std::unique_ptr<NotificationPlatformBridge> NotificationPlatformBridge::Create() {
  return std::make_unique<NotificationPlatformBridgeOhos>();
}

// static
bool NotificationPlatformBridge::CanHandleType(
    NotificationHandler::Type notification_type) {
  return (notification_type == NotificationHandler::Type::WEB_PERSISTENT) ||
      (notification_type == NotificationHandler::Type::WEB_NON_PERSISTENT) ||
      (notification_type == NotificationHandler::Type::EXTENSION);
}

NotificationPlatformBridgeOhos::~NotificationPlatformBridgeOhos() = default;

void NotificationPlatformBridgeOhos::Display(
    NotificationHandler::Type notification_type,
    Profile* profile,
    const message_center::Notification& notification,
    std::unique_ptr<NotificationCommon::Metadata> metadata) {
  ProfileManager* profile_manager = g_browser_process->profile_manager();
  LOG(INFO) << "NotificationPlatformBridgeOhos::Display: "
            << "id=" << notification.id()
            << "; title=" << notification.title()
            << "; message=" << notification.message()
            << "; type=" << (int)notification_type
            << "; profileId=" << GetProfileId(profile)
            << "; manager=" << (void*)profile_manager;

  message_center::Notification notification_with_delegate(notification);
  notification_with_delegate.set_delegate(base::WrapRefCounted(
      new PassThroughDelegate(profile, notification, notification_type)));
  Add(notification.id(), notification_with_delegate, profile);

#if BUILDFLAG(ARKWEB_NOTIFICATION)
  std::shared_ptr<NWebNotificationOptionsItem> options =
      std::make_shared<NWebNotificationOptionsItem>();

  options->id = notification.id();
  options->originUrl = notification.origin_url().spec();
  options->title = base::UTF16ToUTF8(notification.title());
  options->body = base::UTF16ToUTF8(notification.message());

  for (const auto& button : notification.buttons()) {
    options->buttons.push_back(base::UTF16ToUTF8(button.title));
  }

  options->requireInteraction = notification.never_timeout();
  NWebNotificationOptionsItemIcon icon = CreateFromImageSkiaReps(
      notification.icon().GetImage().AsImageSkia().image_reps());
#if defined(ADDRESS_SANITIZER) || defined(HWADDRESS_SANITIZER)
  options->icon = new NWebNotificationOptionsItemIcon(icon);
#else
  NWebNotificationOptionsItemIcon* addr =
      (NWebNotificationOptionsItemIcon*)__real_malloc(sizeof(icon));
  options->icon = new (addr) NWebNotificationOptionsItemIcon(icon);
#endif
  OHOS::NWeb::NWebNotificationManagerDelegate::OnShowNotification(options);
  options->icon->bitmaps =
      std::map<double, NWebNotificationOptionsItemIconBitmap*>();
#endif // ARKWEB_NOTIFICATION
}

void NotificationPlatformBridgeOhos::Close(
    Profile* profile,
    const std::string& notification_id) {
  LOG(INFO) << "NotificationPlatformBridgeOhos::Close: "
            << "id=" << notification_id;
#if BUILDFLAG(ARKWEB_NOTIFICATION)
  OHOS::NWeb::NWebNotificationManagerDelegate::OnCloseNotification(notification_id);
#endif // ARKWEB_NOTIFICATION
  CancelById(notification_id, ProfileNotification::GetProfileID(profile));
}

void NotificationPlatformBridgeOhos::GetDisplayed(
    Profile* profile,
    GetDisplayedNotificationsCallback callback) const {
  std::set<std::string> displayed_notifications;
  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(std::move(callback), std::move(displayed_notifications),
                     false /* supports_synchronization */));
}

void NotificationPlatformBridgeOhos::GetDisplayedForOrigin(
    Profile* profile,
    const GURL& origin,
    GetDisplayedNotificationsCallback callback) const {
  std::set<std::string> displayed_notifications;
  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(std::move(callback), std::move(displayed_notifications),
                     false /* supports_synchronization */));
}

void NotificationPlatformBridgeOhos::SetReadyCallback(
    NotificationBridgeReadyCallback callback) {
  std::move(callback).Run(true);
}

void NotificationPlatformBridgeOhos::DisplayServiceShutDown(
    Profile* profile) {}

void NotificationPlatformBridgeOhos::OnShowed(const std::string id) {
  LOG(INFO) << "NotificationPlatformBridgeOhos::OnShowed: " << "id=" << id;
  ProfileNotification* profile_notification = FindProfileNotification(id);
  if (!profile_notification) {
    LOG(INFO) << "NotificationPlatformBridgeOhos::OnShowed profile_notification invalid";
    return;
  }

  const message_center::Notification& notification = FindProfileNotification(id)->notification();
  PassThroughDelegate* delegate = static_cast<PassThroughDelegate*>(notification.delegate());
  NotificationHandler* handler = NotificationDisplayServiceImpl::GetForProfile(delegate->GetProfile())
      ->GetNotificationHandler(delegate->GetNotificationType());
  if (handler) {
    handler->OnShow(delegate->GetProfile(), delegate->GetNotification().id());
  }
}

void NotificationPlatformBridgeOhos::OnClosed(const std::string id) {
  LOG(INFO) << "NotificationPlatformBridgeOhos::OnClosed: " << "id=" << id;
  ProfileNotification* profile_notification = FindProfileNotification(id);
  if (!profile_notification) {
    LOG(INFO) << "NotificationPlatformBridgeOhos::OnClosed profile_notification invalid";
    return;
  }

  const message_center::Notification& notification = FindProfileNotification(id)->notification();
  PassThroughDelegate* delegate = static_cast<PassThroughDelegate*>(notification.delegate());
  delegate->Close(false);

  CancelById(id, ProfileNotification::GetProfileID(delegate->GetProfile()));
}

void NotificationPlatformBridgeOhos::OnClicked(const std::string id, int buttonIndex) {
  LOG(INFO) << "NotificationPlatformBridgeOhos::OnClicked: " << "id=" << id
            << "; buttonIndex=" << buttonIndex;
  ProfileNotification* profile_notification = FindProfileNotification(id);
  if (!profile_notification) {
    LOG(INFO) << "NotificationPlatformBridgeOhos::OnClicked profile_notification invalid";
    return;
  }

  const message_center::Notification& notification = FindProfileNotification(id)->notification();
  PassThroughDelegate* delegate = static_cast<PassThroughDelegate*>(notification.delegate());
  if (buttonIndex >= 0) {
    delegate->Click(buttonIndex, std::nullopt);
  } else {
    delegate->Click(std::nullopt, std::nullopt);
  }
}
