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
#include "download_api_ext_router.h"

#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/metrics/histogram_macros.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/browser_thread.h"
#include "extensions/browser/extension_function.h"
#include "extensions/browser/extension_function_dispatcher.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/browser/warning_service.h"
#include "net/base/filename_util.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_downloads_cef_delegate.h"

namespace extensions {
std::unordered_map<int, std::shared_ptr<ExDownloadsItemData>>
    extensions::ExDownloadsItemData::download_items_;

namespace {
const char kCanResumeKey[] = "canResume";
const char kDangerKey[] = "danger";
const char kEndTimeKey[] = "endTime";
const char kEndedAfterKey[] = "endedAfter";
const char kEndedBeforeKey[] = "endedBefore";
const char kErrorKey[] = "error";
const char kExistsKey[] = "exists";
const char kFileSizeKey[] = "fileSize";
const char kFilenameKey[] = "filename";
const char kFilenameRegexKey[] = "filenameRegex";
const char kIdKey[] = "id";
const char kMimeKey[] = "mime";
const char kPausedKey[] = "paused";
const char kQueryKey[] = "query";
const char kStartTimeKey[] = "startTime";
const char kStartedAfterKey[] = "startedAfter";
const char kStartedBeforeKey[] = "startedBefore";
const char kStateKey[] = "state";
const char kTotalBytesGreaterKey[] = "totalBytesGreater";
const char kTotalBytesKey[] = "totalBytes";
const char kTotalBytesLessKey[] = "totalBytesLess";
const char kUrlKey[] = "url";
const char kUrlRegexKey[] = "urlRegex";
const char kFinalUrlKey[] = "finalUrl";

ExDownloadsState GetDownloadState(int state) {
  switch (state) {
    case 1:
      return ExDownloadsState::kInProgress;
    case 2:
      return ExDownloadsState::kInterrupted;
    case 3:
      return ExDownloadsState::kComplete;
    default:
      return ExDownloadsState::kStateNone;
  }
}

ExDownloadFilenameConflictAction ConvertConflictAction(
    downloads::FilenameConflictAction action) {
  switch (action) {
    case downloads::FilenameConflictAction::kUniquify:
      return ExDownloadFilenameConflictAction::uniquify;
    case downloads::FilenameConflictAction::kOverwrite:
      return ExDownloadFilenameConflictAction::overwrite;
    case downloads::FilenameConflictAction::kPrompt:
      return ExDownloadFilenameConflictAction::prompt;
    default:
      return ExDownloadFilenameConflictAction::uniquify;
  }
  NOTREACHED();
}

base::Value::Dict DownloadItemsToJSON(const ExDownloadsItem* download_item) {
  extensions::api::downloads::DownloadItem item;
  if (download_item->byExtensionId) {
    item.by_extension_id = download_item->byExtensionId;
  }

  if (download_item->byExtensionName) {
    item.by_extension_name = download_item->byExtensionName;
  }

  if (download_item->endTime) {
    item.end_time = download_item->endTime;
  }

  if (download_item->error) {
    item.error = static_cast<extensions::api::downloads::InterruptReason>(
        download_item->error);
  }

  if (download_item->estimatedEndTime) {
    item.estimated_end_time = download_item->estimatedEndTime;
  }

  item.bytes_received = download_item->bytesReceived;
  item.can_resume = download_item->canResume;
  item.danger = static_cast<extensions::api::downloads::DangerType>(
      download_item->danger);
  item.exists = download_item->exists;
  item.file_size = download_item->fileSize;
  item.filename = download_item->filename;
  item.final_url = download_item->finalUrl;
  item.id = download_item->id;
  item.incognito = download_item->incognito;
  item.mime = download_item->mime;
  item.paused = download_item->paused;
  item.referrer = download_item->referrer;
  item.start_time = download_item->startTime;
  item.state =
      static_cast<extensions::api::downloads::State>(download_item->state);
  item.total_bytes = download_item->totalBytes;
  item.url = download_item->url;
  return item.ToValue();
}

bool IsDownloadDeltaField(const std::string& field) {
  return ((field == kUrlKey) || (field == kFinalUrlKey) ||
          (field == kFilenameKey) || (field == kDangerKey) ||
          (field == kMimeKey) || (field == kStartTimeKey) ||
          (field == kEndTimeKey) || (field == kStateKey) ||
          (field == kCanResumeKey) || (field == kPausedKey) ||
          (field == kErrorKey) || (field == kTotalBytesKey) ||
          (field == kFileSizeKey) || (field == kExistsKey));
}

bool OnDeterminingFilenameWillDispatchCallback(
    bool* any_determiners,
    ExtensionDownloadsEventRouterDataEx* data,
    content::BrowserContext* browser_context,
    mojom::ContextType target_context,
    const Extension* extension,
    const base::Value::Dict* listener_filter,
    std::optional<base::Value::List>& event_args_out,
    mojom::EventFilteringInfoPtr& event_filtering_info_out) {
  *any_determiners = true;
  base::Time installed =
      ExtensionPrefs::Get(browser_context)->GetLastUpdateTime(extension->id());
  data->AddPendingDeterminer(extension->id(), installed);
  return true;
}

bool Fault(bool error, const char* message_in, std::string* message_out) {
  if (!error) {
    return false;
  }
  *message_out = message_in;
  return true;
}

bool InvalidId(ExDownloadsItemData* valid_item, std::string* message_out) {
  return Fault(!valid_item, download_extension_errors::kInvalidId, message_out);
}

void RecordApiFunctions(DownloadsFunctionName function) {
  UMA_HISTOGRAM_ENUMERATION("Download.ApiFunctions", function,
                            DOWNLOADS_FUNCTION_LAST);
}

base::Value::Dict GenerateDeltaForIncompleteDownload(
    ExtensionDownloadsEventRouterDataEx* data,
    ExDownloadsItemData* item_class,
    base::Value::Dict* delta,
    bool* changed,
    ExDownloadsItem* download_item) {
  base::Value::Dict new_json = DownloadItemsToJSON(download_item);
  std::set<std::string> new_fields;
  for (auto kv : new_json) {
    new_fields.insert(kv.first);
    if (!IsDownloadDeltaField(kv.first)) {
      continue;
    }
    const base::Value* old_value = data->json().Find(kv.first);
    if (!old_value || kv.second != *old_value) {
      delta->SetByDottedPath(kv.first + ".current", kv.second.Clone());
      if (old_value) {
        delta->SetByDottedPath(kv.first + ".previous", old_value->Clone());
      }
      *changed = true;
    }
  }

  for (auto kv : data->json()) {
    if ((new_fields.find(kv.first) == new_fields.end()) &&
        IsDownloadDeltaField(kv.first)) {
      // estimatedEndTime disappears after completion, but bytesReceived
      // stays.
      delta->SetByDottedPath(kv.first + ".previous", kv.second.Clone());
      *changed = true;
    }
  }
  return new_json;
}

base::Value::Dict GetOnDownloadUpdateDelta(
    ExtensionDownloadsEventRouterDataEx* data,
    ExDownloadsItemData* item_class,
    base::Value::Dict* new_json,
    bool* changed,
    ExDownloadsItem* download_item) {
  base::Value::Dict delta;
  if (data->is_download_completed()) {
    if (data->is_completed_download_deleted() !=
        item_class->GetFileExternallyRemoved()) {
      DCHECK(!data->is_completed_download_deleted());
      DCHECK(item_class->GetFileExternallyRemoved());
      delta.SetByDottedPath("exists.current", false);
      delta.SetByDottedPath("exists.previous", true);
      *changed = true;
    }
  } else {
    *new_json = GenerateDeltaForIncompleteDownload(data, item_class, &delta,
                                                   changed, download_item);
  }
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::OnDownloadUpdated changed: "
            << *changed;
  delta.Set("id", item_class->GetId());
  return delta;
}

}  // namespace

void ExDownloadsItemData::Remove(int download_id) {
  ExDownloadsItemData* item = Get(download_id);
  if (item != nullptr) {
    ExtensionDownloadsEventRouterDataEx* data =
        ExtensionDownloadsEventRouterDataEx::Get(item);
    if (data) {
      ExtensionDownloadsEventRouterDataEx::Remove(item);
    }
    download_items_.erase(download_id);
  }
}

ExDownloadsItemData* ExDownloadsItemData::Get(int download_id) {
  auto it = download_items_.find(download_id);
  if (it != download_items_.end()) {
    return it->second.get();
  }
  return nullptr;
}

const char DownloadedByExtensionEx::kKey[] =
    "DownloadItem DownloadedByExtensionEx";

DownloadedByExtensionEx* DownloadedByExtensionEx::Get(
    ExDownloadsItemData* item) {
  base::SupportsUserData::Data* data = item->GetUserData(kKey);
  return (data == nullptr) ? nullptr
                           : static_cast<DownloadedByExtensionEx*>(data);
}

DownloadedByExtensionEx::DownloadedByExtensionEx(ExDownloadsItemData* item,
                                                 const std::string& id,
                                                 const std::string& name)
    : id_(id), name_(name) {
  item->SetUserData(kKey, base::WrapUnique(this));
}

ExtensionDownloadsEventRouterDataEx* ExtensionDownloadsEventRouterDataEx::Get(
    ExDownloadsItemData* download_item) {
  base::SupportsUserData::Data* data = download_item->GetUserData(kKey);
  return (data == nullptr)
             ? nullptr
             : static_cast<ExtensionDownloadsEventRouterDataEx*>(data);
}

void ExtensionDownloadsEventRouterDataEx::Remove(
    ExDownloadsItemData* download_item) {
  download_item->RemoveUserData(kKey);
}

ExtensionDownloadsEventRouterDataEx::ExtensionDownloadsEventRouterDataEx(
    ExDownloadsItemData* download_item,
    base::Value::Dict json_item)
    : updated_(0),
      changed_fired_(0),
      json_(std::move(json_item)),
      creator_conflict_action_(downloads::FilenameConflictAction::kUniquify),
      determined_conflict_action_(downloads::FilenameConflictAction::kUniquify),
      is_download_completed_(GetDownloadState(download_item->GetState()) ==
                             ExDownloadsState::kComplete),
      is_completed_download_deleted_(download_item->GetFileExternallyRemoved()),
      download_id_(download_item->GetId()) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  download_item->SetUserData(kKey, base::WrapUnique(this));
}

void ExtensionDownloadsEventRouterDataEx::BeginFilenameDetermination(
    ExtensionDownloadsEventRouterEx::FilenameChangedCallback filename_changed) {
  LOG(INFO)
      << "ExtensionDownloadsEventRouterEx::BeginFilenameDetermination start";
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  ClearPendingDeterminers();
  filename_changed_ = std::move(filename_changed);
  determined_filename_ = creator_suggested_filename_;
  determined_conflict_action_ = creator_conflict_action_;
  // determiner_.install_time should default to 0 so that creator suggestions
  // should be lower priority than any actual onDeterminingFilename listeners.

  // Ensure that the callback is called within a time limit.
  weak_ptr_factory_ = std::make_unique<
      base::WeakPtrFactory<ExtensionDownloadsEventRouterDataEx>>(this);
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
      FROM_HERE,
      base::BindOnce(
          &ExtensionDownloadsEventRouterDataEx::DetermineFilenameTimeout,
          weak_ptr_factory_->GetWeakPtr()),
      base::Seconds(determine_filename_timeout_s_));
}

void ExtensionDownloadsEventRouterDataEx::CallFilenameCallback() {
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::CallFilenameCallback start";
  if (!filename_changed_) {
    return;
  }

  LOG(INFO) << "ExtensionDownloadsEventRouterEx::CallFilenameCallback "
               "determined_filename_: "
            << determined_filename_.value();
  FilenameSuggestion suggestion = {
      determined_filename_.value(),
      ConvertConflictAction(determined_conflict_action_)};

  std::move(filename_changed_).Run(suggestion);
}

void ExtensionDownloadsEventRouterDataEx::ClearPendingDeterminers() {
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::ClearPendingDeterminers start";
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  determined_filename_.clear();
  determined_conflict_action_ = downloads::FilenameConflictAction::kUniquify;
  determiner_ = DeterminerInfo();
  filename_changed_.Reset();
  weak_ptr_factory_.reset();
  determiners_.clear();
}

void ExtensionDownloadsEventRouterDataEx::DeterminerRemoved(
    const ExtensionId& extension_id) {
  LOG(INFO) << "ExtensionDownloadsEventRouterDataEx::DeterminerRemoved";
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  for (auto iter = determiners_.begin(); iter != determiners_.end();) {
    if (iter->extension_id == extension_id) {
      iter = determiners_.erase(iter);
    } else {
      ++iter;
    }
  }
  // If we just removed the last unreported determiner, then we need to call a
  // callback.
  CheckAllDeterminersCalled();
}

void ExtensionDownloadsEventRouterDataEx::AddPendingDeterminer(
    const ExtensionId& extension_id,
    const base::Time& installed) {
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::AddPendingDeterminer start";
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  for (auto& determiner : determiners_) {
    if (determiner.extension_id == extension_id) {
      determiner.install_time = installed;
      return;
    }
  }
  determiners_.push_back(DeterminerInfo(extension_id, installed));
}

bool ExtensionDownloadsEventRouterDataEx::DeterminerAlreadyReported(
    const ExtensionId& extension_id) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  for (auto& determiner : determiners_) {
    if (determiner.extension_id == extension_id) {
      return determiner.reported;
    }
  }
  return false;
}

void ExtensionDownloadsEventRouterDataEx::CreatorSuggestedFilename(
    const base::FilePath& filename,
    downloads::FilenameConflictAction conflict_action) {
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::CreatorSuggestedFilename";
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  creator_suggested_filename_ = filename;
  creator_conflict_action_ = conflict_action;
}

bool ExtensionDownloadsEventRouterDataEx::DeterminerCallback(
    content::BrowserContext* browser_context,
    const ExtensionId& extension_id,
    const base::FilePath& filename,
    downloads::FilenameConflictAction conflict_action) {
  LOG(INFO)
      << "ExtensionDownloadsEventRouterEx::DeterminerCallback start filename: "
      << filename.value();
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  bool found_info = false;
  for (auto& determiner : determiners_) {
    if (determiner.extension_id == extension_id) {
      found_info = true;
      if (determiner.reported) {
        return false;
      }
      determiner.reported = true;
      // Do not use filename if another determiner has already overridden the
      // filename and they take precedence. Extensions that were installed
      // later take precedence over previous extensions.

      if (filename.empty() &&
          (conflict_action == downloads::FilenameConflictAction::kUniquify)) {
        break;
      }
      WarningSet warnings;
      ExtensionId winner_extension_id;
      ExtensionDownloadsEventRouterEx::DetermineFilenameInternal(
          {filename, conflict_action, determiner.extension_id,
           determiner.install_time, determiner_.extension_id,
           determiner_.install_time, &winner_extension_id,
           &determined_filename_, &determined_conflict_action_, &warnings});
      if (!warnings.empty()) {
        WarningService::NotifyWarningsOnUI(browser_context, warnings);
      }
      if (winner_extension_id == determiner.extension_id) {
        determiner_ = determiner;
      }
      break;
    }
  }
  if (!found_info) {
    return false;
  }
  CheckAllDeterminersCalled();
  return true;
}

void ExtensionDownloadsEventRouterDataEx::CheckAllDeterminersCalled() {
  for (auto& determiner : determiners_) {
    if (!determiner.reported) {
      return;
    }
  }
  CallFilenameCallback();

  // Don't clear determiners_ immediately in case there's a second listener
  // for one of the extensions, so that DetermineFilename can return
  // kTooManyListeners. After a few seconds, DetermineFilename will return
  // kUnexpectedDeterminer instead of kTooManyListeners so that determiners_
  // doesn't keep hogging memory.
  weak_ptr_factory_ = std::make_unique<
      base::WeakPtrFactory<ExtensionDownloadsEventRouterDataEx>>(this);
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
      FROM_HERE,
      base::BindOnce(
          &ExtensionDownloadsEventRouterDataEx::ClearPendingDeterminers,
          weak_ptr_factory_->GetWeakPtr()),
      base::Seconds(15));
}

int ExtensionDownloadsEventRouterDataEx::determine_filename_timeout_s_ = 15;

ExtensionDownloadsEventRouterDataEx::DeterminerInfo::DeterminerInfo(
    const ExtensionId& e_id,
    const base::Time& installed)
    : extension_id(e_id), install_time(installed), reported(false) {}

ExtensionDownloadsEventRouterDataEx::DeterminerInfo::DeterminerInfo()
    : reported(false) {}

ExtensionDownloadsEventRouterDataEx::DeterminerInfo::~DeterminerInfo() {}

const char ExtensionDownloadsEventRouterDataEx::kKey[] =
    "ExDownloadsItemData ExtensionDownloadsEventRouterDataEx";

/* eventRouter */
ExtensionDownloadsEventRouterEx&
ExtensionDownloadsEventRouterEx::GetInstance() {
  static ExtensionDownloadsEventRouterEx instance;
  return instance;
}

void ExtensionDownloadsEventRouterEx::OnListenerRemoved(
    const EventListenerInfo& details) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (details.event_name != downloads::OnDeterminingFilename::kEventName &&
      details.event_name != downloads::OnCreated::kEventName &&
      details.event_name != downloads::OnChanged::kEventName &&
      details.event_name != downloads::OnErased::kEventName) {
    return;
  }
  bool determiner_removed =
      (details.event_name == downloads::OnDeterminingFilename::kEventName);
  EventRouter* router =
      EventRouter::Get(Profile::FromBrowserContext(details.browser_context));
  bool any_listeners =
      router->HasEventListener(downloads::OnChanged::kEventName) ||
      router->HasEventListener(downloads::OnDeterminingFilename::kEventName);
  if (!determiner_removed && any_listeners) {
    return;
  }

  ExDownloadsItemVector itemsVector =
      OHOS::NWeb::NWebExtensionDownloadCefDelegate::GetInstance()
          .GetAllDownloadItem();
  if (itemsVector.empty()) {
    return;
  }

  for (uint32_t i = 0; i < itemsVector.size(); i++) {
    ExDownloadsItemData* item_class =
        ExDownloadsItemData::Get(itemsVector[i].id);
    if (!item_class) {
      item_class = new ExDownloadsItemData(itemsVector[i]);
    }
    ExtensionDownloadsEventRouterDataEx* data =
        ExtensionDownloadsEventRouterDataEx::Get(item_class);
    if (!data) {
      continue;
    }
    if (determiner_removed) {
      // Notify any items that may be waiting for callbacks from this
      // extension/determiner.  This will almost always be a no-op, however, it
      // is possible for an extension renderer to be unloaded while a download
      // item is waiting for a determiner. In that case, the download item
      // should proceed.
      data->DeterminerRemoved(details.extension_id);
    }
    if (!any_listeners && data->creator_suggested_filename().empty()) {
      ExDownloadsItemData::Remove(item_class->GetId());
    }
  }
}

void ExtensionDownloadsEventRouterEx::DispatchEvent(dispatchEventInfo info) {
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::DispatchEvent start";
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  raw_ptr<Profile> profile =
      static_cast<Profile*>(Profile::FromBrowserContext(info.browser_context));
  if (!EventRouter::Get(info.browser_context)) {
    LOG(INFO)
        << "ExtensionDownloadsEventRouterEx::DispatchEvent get profile error";
    return;
  }
  base::Value::List args;
  args.Append(std::move(info.arg));
  // The downloads system wants to share on-record events with off-record
  // extension renderers even in incognito_split_mode because that's how
  // chrome://downloads works. The "restrict_to_profile" mechanism does not
  // anticipate this, so it does not automatically prevent sharing off-record
  // events with on-record extension renderers.
  // TODO(lazyboy): When |restrict_to_browser_context| is nullptr, this will
  // broadcast events to unrelated profiles, not just incognito. Fix this
  // by introducing "include incognito" option to Event constructor.
  // https://crbug.com/726022.
  Profile* restrict_to_browser_context =
      (info.include_incognito && !profile->IsOffTheRecord()) ? nullptr
                                                             : profile.get();
  auto event =
      std::make_unique<Event>(info.histogram_value, info.event_name,
                              std::move(args), restrict_to_browser_context);
  event->will_dispatch_callback = std::move(info.will_dispatch_callback);
  EventRouter::Get(info.browser_context)->BroadcastEvent(std::move(event));
}

/* onCreated */
void ExtensionDownloadsEventRouterEx::OnDownloadCreated(
    ExDownloadsItem* download_item,
    content::BrowserContext* browser_context) {
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::OnDownloadCreated";
  EventRouter* router =
      EventRouter::Get(Profile::FromBrowserContext(browser_context));
  if (!router || (!router->HasEventListener(downloads::OnCreated::kEventName) &&
                  !router->HasEventListener(downloads::OnChanged::kEventName) &&
                  !router->HasEventListener(
                      downloads::OnDeterminingFilename::kEventName))) {
    LOG(INFO) << "ExtensionDownloadsEventRouterEx::OnDownloadCreated router"
              << router << " or have no listener.";
    return;
  }

  base::Value::Dict jsonItem = DownloadItemsToJSON(download_item);
  DispatchEvent({events::DOWNLOADS_ON_CREATED, downloads::OnCreated::kEventName,
                 true, Event::WillDispatchCallback(),
                 base::Value(jsonItem.Clone()), browser_context});

  ExDownloadsItemData* item_class = ExDownloadsItemData::Get(download_item->id);
  if (!item_class) {
    LOG(INFO) << "ExtensionDownloadsEventRouterEx::OnDownloadCreated "
                 "!item_class";
    item_class = new ExDownloadsItemData(*download_item);
  }

  if (!ExtensionDownloadsEventRouterDataEx::Get(item_class) &&
      (router->HasEventListener(downloads::OnChanged::kEventName) ||
       router->HasEventListener(
           downloads::OnDeterminingFilename::kEventName))) {
    new ExtensionDownloadsEventRouterDataEx(
        item_class,
        GetDownloadState(item_class->GetState()) == ExDownloadsState::kComplete
            ? base::Value::Dict()
            : std::move(jsonItem));
  }
  UpdateDownloadsItemData(item_class,
                          const_cast<ExDownloadsItem*>(download_item));
}

/* onChanged */
void ExtensionDownloadsEventRouterEx::UpdateDownloadsItemData(
    ExDownloadsItemData* item_class,
    const ExDownloadsItem* download_item) {
  if (item_class == nullptr || download_item == nullptr) {
    return;
  }
  item_class->SetBytesReceived(download_item->bytesReceived);
  item_class->SetCanResume(download_item->canResume);
  item_class->SetDanger(download_item->danger);
  item_class->SetEndTime(
      download_item->endTime.value_or(item_class->GetEndTime()));
  item_class->SetError(download_item->error);
  item_class->SetEstimatedEndTime(download_item->estimatedEndTime.value_or(
      item_class->GetEstimatedEndTime()));
  item_class->SetExists(download_item->exists);
  item_class->SetFileSize(download_item->fileSize);
  item_class->SetFilename(download_item->filename);
  item_class->SetFinalUrl(download_item->finalUrl);
  item_class->SetId(download_item->id);
  item_class->SetIncognito(download_item->incognito);
  item_class->SetMime(download_item->mime);
  item_class->SetPaused(download_item->paused);
  item_class->SetReferrer(download_item->referrer);
  item_class->SetStartTime(download_item->startTime);
  item_class->SetState(download_item->state);
  item_class->SetTotalBytes(download_item->totalBytes);
  // Intentionally skip updating URL field to preserve correct value.
  // |download_item->url| is untrusted and should not overwrite |item_class|
  // data.
}

void ExtensionDownloadsEventRouterEx::OnDownloadUpdated(
    ExDownloadsItem* download_item,
    content::BrowserContext* browser_context) {
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::OnDownloadUpdated start";
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  EventRouter* router =
      EventRouter::Get(Profile::FromBrowserContext(browser_context));
  if (!router->HasEventListener(downloads::OnChanged::kEventName)) {
    LOG(INFO) << "router->HasEventListener(downloads::OnChanged::kEventName)";
    return;
  }

  ExDownloadsItemData* item_class = ExDownloadsItemData::Get(download_item->id);
  if (!item_class) {
    LOG(INFO)
        << "ExtensionDownloadsEventRouterEx::OnDownloadUpdated !item_class";
    item_class = new ExDownloadsItemData(*download_item);
  } else {
    const Extension* extension =
        ExtensionRegistry::Get(browser_context)
            ->GetExtensionById(item_class->GetByExtensionId(),
                               ExtensionRegistry::EVERYTHING);
    if (extension) {
      item_class->SetByExtensionName(extension->name());
    }
  }

  ExtensionDownloadsEventRouterDataEx* data =
      ExtensionDownloadsEventRouterDataEx::Get(item_class);
  if (!data) {
    LOG(INFO) << "ExtensionDownloadsEventRouterEx::OnDownloadUpdated !data";
    data = new ExtensionDownloadsEventRouterDataEx(item_class,
                                                   base::Value::Dict());
  }
  base::Value::Dict new_json;
  bool changed = false;
  base::Value::Dict delta = GetOnDownloadUpdateDelta(
      data, item_class, &new_json, &changed, download_item);
  UpdateDownloadsItemData(item_class,
                          const_cast<ExDownloadsItem*>(download_item));

  data->set_is_download_completed(GetDownloadState(item_class->GetState()) ==
                                  ExDownloadsState::kComplete);

  data->set_is_completed_download_deleted(
      item_class->GetFileExternallyRemoved());
  data->set_json(std::move(new_json));
  data->OnItemUpdated();
  if (changed) {
    DispatchEvent({events::DOWNLOADS_ON_CHANGED,
                   downloads::OnChanged::kEventName, true,
                   Event::WillDispatchCallback(), base::Value(std::move(delta)),
                   browser_context});
    data->OnChangedFired();
  }
}

/* onErased */
void ExtensionDownloadsEventRouterEx::OnDownloadRemoved(
    int download_id,
    content::BrowserContext* browser_context) {
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::OnDownloadRemoved start";
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  ExDownloadsItemData::Remove(download_id);
  DispatchEvent({events::DOWNLOADS_ON_ERASED, downloads::OnErased::kEventName,
                 true, Event::WillDispatchCallback(), base::Value(download_id),
                 browser_context});
}

/* onDeterminingFilename */
void ExtensionDownloadsEventRouterEx::OnDeterminingFilename(
    const ExDownloadsItem* download_item,
    const char* suggested_path,
    FilenameChangedCallback filename_changed_callback,
    content::BrowserContext* browser_context) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::OnDeterminingFilename start"
            << ", download_id: " << download_item->id
            << ", state: " << download_item->state
            << ", start-time: " << download_item->startTime;

  ExDownloadsItemData* item_class = ExDownloadsItemData::Get(download_item->id);
  if (!item_class) {
    LOG(INFO)
        << "ExtensionDownloadsEventRouterEx::OnDeterminingFilename !item_class";
    item_class = new ExDownloadsItemData(*download_item);
  } else {
    UpdateDownloadsItemData(item_class, download_item);
  }
  LOG(INFO)
      << "ExtensionDownloadsEventRouterEx::OnDeterminingFilename class state: "
      << item_class->GetState();

  ExtensionDownloadsEventRouterDataEx* data =
      ExtensionDownloadsEventRouterDataEx::Get(item_class);
  if (!data) {
    LOG(INFO) << "ExtensionDownloadsEventRouterEx::OnDeterminingFilename !data";
    data = new ExtensionDownloadsEventRouterDataEx(item_class,
                                                   base::Value::Dict());
  }
  data->BeginFilenameDetermination(std::move(filename_changed_callback));
  bool any_determiners = false;
  base::Value::Dict json = DownloadItemsToJSON(download_item);
  // Override 'url' field with trusted value from |item_class| since
  // |download_item->url| is known to be unreliable.
  json.Set(kUrlKey, item_class->GetUrl());
  if (suggested_path) {
    json.Set("filename", suggested_path);
  }
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::OnDeterminingFilename will "
               "dispatchEvent";
  DispatchEvent({events::DOWNLOADS_ON_DETERMINING_FILENAME,
                 downloads::OnDeterminingFilename::kEventName, false,
                 base::BindRepeating(&OnDeterminingFilenameWillDispatchCallback,
                                     &any_determiners, data),
                 base::Value(std::move(json)), browser_context});
  if (!any_determiners) {
    data->CallFilenameCallback();
    data->ClearPendingDeterminers();
    data->ResetCreatorSuggestion();
  }
}

void ExtensionDownloadsEventRouterEx::DetermineFilenameInternal(
    DetermineFilenameInternalInfo info) {
  DCHECK(
      !info.filename.empty() ||
      (info.conflict_action != downloads::FilenameConflictAction::kUniquify));
  DCHECK(!info.suggesting_extension_id.empty());

  if (info.incumbent_extension_id.empty()) {
    *info.winner_extension_id = info.suggesting_extension_id;
    *info.determined_filename = info.filename;
    *info.determined_conflict_action = info.conflict_action;
    return;
  }

  if (info.suggesting_install_time < info.incumbent_install_time) {
    *info.winner_extension_id = info.incumbent_extension_id;
    info.warnings->insert(Warning::CreateDownloadFilenameConflictWarning(
        info.suggesting_extension_id, info.incumbent_extension_id,
        info.filename, *info.determined_filename));
    return;
  }

  *info.winner_extension_id = info.suggesting_extension_id;
  info.warnings->insert(Warning::CreateDownloadFilenameConflictWarning(
      info.incumbent_extension_id, info.suggesting_extension_id,
      *info.determined_filename, info.filename));
  *info.determined_filename = info.filename;
  *info.determined_conflict_action = info.conflict_action;
}

bool ExtensionDownloadsEventRouterEx::DetermineFilename(
    DetermineFilenameInfo info) {
  LOG(INFO) << "ExtensionDownloadsEventRouterEx::DetermineFilename start"
            << ", download_id: " << info.download_id
            << ", const_filename: " << info.const_filename.value()
            << ", conflict_action: " << (int)info.conflict_action;
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  RecordApiFunctions(DOWNLOADS_FUNCTION_DETERMINE_FILENAME);

  ExDownloadsItemData* item_class = ExDownloadsItemData::Get(info.download_id);
  ExtensionDownloadsEventRouterDataEx* data =
      item_class ? ExtensionDownloadsEventRouterDataEx::Get(item_class)
                 : nullptr;
  // maxListeners=1 in downloads.idl and suggestCallback in
  // downloads_custom_bindings.js should prevent duplicate DeterminerCallback
  // calls from the same renderer, but an extension may have more than one
  // renderer, so don't DCHECK(!reported).

  if (InvalidId(item_class, info.error) ||
      Fault(GetDownloadState(item_class->GetState()) !=
                ExDownloadsState::kInProgress,
            download_extension_errors::kNotInProgress, info.error) ||
      Fault(!data, download_extension_errors::kUnexpectedDeterminer,
            info.error) ||
      Fault(data->DeterminerAlreadyReported(info.ext_id),
            download_extension_errors::kTooManyListeners, info.error)) {
    return false;
  }
  base::FilePath::StringType filename_str(info.const_filename.value());
  // Allow windows-style directory separators on all platforms.
  std::replace(filename_str.begin(), filename_str.end(),
               FILE_PATH_LITERAL('\\'), FILE_PATH_LITERAL('/'));
  base::FilePath filename(filename_str);
  bool valid_filename = net::IsSafePortableRelativePath(filename);
  filename =
      (valid_filename ? filename.NormalizePathSeparators() : base::FilePath());
  // If the invalid filename check is moved to before DeterminerCallback(), then
  // it will block forever waiting for this ext_id to report.
  if (Fault(!data->DeterminerCallback(info.browser_context, info.ext_id,
                                      filename, info.conflict_action),
            download_extension_errors::kUnexpectedDeterminer, info.error) ||
      Fault((!info.const_filename.empty() && !valid_filename),
            download_extension_errors::kInvalidFilename, info.error)) {
    return false;
  }
  return true;
}
}  // namespace extensions