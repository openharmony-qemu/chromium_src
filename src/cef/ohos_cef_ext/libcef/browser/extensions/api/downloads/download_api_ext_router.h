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
#ifndef CEF_LIBCEF_BROWSER_EXTENSIONS_DOWNLOADS_API_EXT_ROUTER_H_
#define CEF_LIBCEF_BROWSER_EXTENSIONS_DOWNLOADS_API_EXT_ROUTER_H_

#include "base/memory/raw_ptr.h"
#include "base/strings/utf_string_conversions.h"
#include "base/supports_user_data.h"
#include "base/values.h"
#include "chrome/browser/extensions/api/downloads/downloads_api.h"
#include "chrome/common/extensions/api/downloads.h"
#include "components/download/public/common/download_item.h"
#include "extensions/common/extension_id.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_downloads_types.h"

namespace extensions {
namespace downloads = api::downloads;

struct dispatchEventInfo {
  events::HistogramValue histogram_value;
  const std::string& event_name;
  bool include_incognito;
  Event::WillDispatchCallback will_dispatch_callback;
  base::Value arg;
  raw_ptr<content::BrowserContext> browser_context;
};

struct DetermineFilenameInternalInfo {
  const base::FilePath& filename;
  downloads::FilenameConflictAction conflict_action;
  const ExtensionId& suggesting_extension_id;
  const base::Time& suggesting_install_time;
  const ExtensionId& incumbent_extension_id;
  const base::Time& incumbent_install_time;
  raw_ptr<ExtensionId> winner_extension_id;
  raw_ptr<base::FilePath> determined_filename;
  raw_ptr<downloads::FilenameConflictAction> determined_conflict_action;
  raw_ptr<WarningSet> warnings;
};

struct DetermineFilenameInfo {
  raw_ptr<content::BrowserContext> browser_context;
  bool include_incognito;
  const ExtensionId& ext_id;
  int download_id;
  const base::FilePath& const_filename;
  downloads::FilenameConflictAction conflict_action;
  raw_ptr<std::string> error;
};

/* downloadItemData */
class ExDownloadsItemData : public base::SupportsUserData {
 public:
  explicit ExDownloadsItemData(const ExDownloadsItem& item)
      : byExtensionId_(item.byExtensionId.value_or("")),
        byExtensionName_(item.byExtensionName.value_or("")),
        bytesReceived_(item.bytesReceived),
        canResume_(item.canResume),
        danger_(item.danger),
        endTime_(item.endTime.value_or("")),
        error_(item.error),
        estimatedEndTime_(item.estimatedEndTime.value_or("")),
        exists_(item.exists),
        fileSize_(item.fileSize),
        filename_(item.filename),
        finalUrl_(item.finalUrl),
        download_id_(item.id),
        incognito_(item.incognito),
        mime_(item.mime),
        paused_(item.paused),
        referrer_(item.referrer),
        startTime_(item.startTime),
        state_(item.state),
        totalBytes_(item.totalBytes),
        downloadUrl_(item.url) {
    download_items_[download_id_] = std::shared_ptr<ExDownloadsItemData>(this);
  }

  ~ExDownloadsItemData() {
    Remove(download_id_);
  }

  static ExDownloadsItemData* Get(int download_id);
  static void Remove(int download_id);

  std::string GetByExtensionId() const { return byExtensionId_; }
  std::string GetByExtensionName() const { return byExtensionName_; }
  double GetBytesReceived() const { return bytesReceived_; }
  bool CanResume() const { return canResume_; }
  int GetDanger() const { return danger_; }
  std::string GetEndTime() const { return endTime_; }
  int GetError() const { return error_; }
  std::string GetEstimatedEndTime() const { return estimatedEndTime_; }
  bool IsExists() const { return exists_; }
  double GetFileSize() const { return fileSize_; }
  std::string GetFilename() const { return filename_; }
  std::string GetFinalUrl() const { return finalUrl_; }
  int GetId() const { return download_id_; }
  bool GetIsIncognito() const { return incognito_; }
  std::string GetMime() const { return mime_; }
  bool IsPaused() const { return paused_; }
  std::string GetReferrer() const { return referrer_; }
  std::string GetStartTime() const { return startTime_; }
  int GetState() const { return state_; }
  double GetTotalBytes() const { return totalBytes_; }
  std::string GetUrl() const { return downloadUrl_; }
  bool GetFileExternallyRemoved() const { return !exists_; }

  void SetByExtensionId(const std::string& value) { byExtensionId_ = value; }
  void SetByExtensionName(const std::string& value) {
    byExtensionName_ = value;
  }
  void SetBytesReceived(double value) { bytesReceived_ = value; }
  void SetCanResume(bool value) { canResume_ = value; }
  void SetDanger(int value) { danger_ = value; }
  void SetEndTime(const std::string& value) { endTime_ = value; }
  void SetError(int value) { error_ = value; }
  void SetEstimatedEndTime(const std::string& value) {
    estimatedEndTime_ = value;
  }
  void SetExists(bool value) { exists_ = value; }
  void SetFileSize(double value) { fileSize_ = value; }
  void SetFilename(const std::string& value) { filename_ = value; }
  void SetFinalUrl(const std::string& value) { finalUrl_ = value; }
  void SetId(int value) { download_id_ = value; }
  void SetIncognito(bool value) { incognito_ = value; }
  void SetMime(const std::string& value) { mime_ = value; }
  void SetPaused(bool value) { paused_ = value; }
  void SetReferrer(const std::string& value) { referrer_ = value; }
  void SetStartTime(const std::string& value) { startTime_ = value; }
  void SetState(int value) { state_ = value; }
  void SetTotalBytes(double value) { totalBytes_ = value; }
  void SetUrl(const std::string& value) { downloadUrl_ = value; }

 private:
  std::string byExtensionId_;
  std::string byExtensionName_;
  double bytesReceived_;
  bool canResume_;
  int danger_;
  std::string endTime_;
  int error_;
  std::string estimatedEndTime_;
  bool exists_;
  double fileSize_;
  std::string filename_;
  std::string finalUrl_;
  int download_id_;
  bool incognito_;
  std::string mime_;
  bool paused_;
  std::string referrer_;
  std::string startTime_;
  int state_;
  double totalBytes_;
  std::string downloadUrl_;
  static std::unordered_map<int, std::shared_ptr<ExDownloadsItemData>>
      download_items_;
};

/* downloadExtension */
class DownloadedByExtensionEx : public base::SupportsUserData::Data {
 public:
  static DownloadedByExtensionEx* Get(ExDownloadsItemData* item);

  DownloadedByExtensionEx(ExDownloadsItemData* item,
                          const std::string& id,
                          const std::string& name);

  DownloadedByExtensionEx(const DownloadedByExtensionEx&) = delete;
  DownloadedByExtensionEx& operator=(const DownloadedByExtensionEx&) = delete;

  const std::string& id() const { return id_; }
  const std::string& name() const { return name_; }

 private:
  static const char kKey[];

  std::string id_;
  std::string name_;
};

class ExtensionDownloadsEventRouterEx
    : public extensions::EventRouter::Observer {
 public:
  typedef base::OnceCallback<void(const FilenameSuggestion& suggestion)>
      FilenameChangedCallback;

  ExtensionDownloadsEventRouterEx() = default;
  ~ExtensionDownloadsEventRouterEx() = default;
  static ExtensionDownloadsEventRouterEx& GetInstance();

  void OnDownloadCreated(ExDownloadsItem* download_item,
                         content::BrowserContext* browser_context);
  void OnDownloadUpdated(ExDownloadsItem* download_item,
                         content::BrowserContext* browser_context);
  void UpdateDownloadsItemData(ExDownloadsItemData* item_class,
                               const ExDownloadsItem* download_item);
  void OnDownloadRemoved(int download_id,
                         content::BrowserContext* browser_context);

  void OnDeterminingFilename(const ExDownloadsItem* item,
                             const char* suggested_path,
                             FilenameChangedCallback filename_changed,
                             content::BrowserContext* browser_context);

  static bool DetermineFilename(DetermineFilenameInfo info);

  static void DetermineFilenameInternal(DetermineFilenameInternalInfo info);

  // extensions::EventRouter::Observer.
  void OnListenerRemoved(const extensions::EventListenerInfo& details) override;

 private:
  ExtensionDownloadsEventRouterEx(const ExtensionDownloadsEventRouterEx&) =
      delete;
  ExtensionDownloadsEventRouterEx& operator=(
      const ExtensionDownloadsEventRouterEx&) = delete;

  void DispatchEvent(dispatchEventInfo info);
};

/* event router data */
class ExtensionDownloadsEventRouterDataEx
    : public base::SupportsUserData::Data {
 public:
  static ExtensionDownloadsEventRouterDataEx* Get(
      ExDownloadsItemData* download_item);

  static void Remove(ExDownloadsItemData* download_item);

  explicit ExtensionDownloadsEventRouterDataEx(
      ExDownloadsItemData* download_item,
      base::Value::Dict json_item);

  ExtensionDownloadsEventRouterDataEx(
      const ExtensionDownloadsEventRouterDataEx&) = delete;
  ExtensionDownloadsEventRouterDataEx& operator=(
      const ExtensionDownloadsEventRouterDataEx&) = delete;

  ~ExtensionDownloadsEventRouterDataEx() override = default;

  void set_is_download_completed(bool is_download_completed) {
    is_download_completed_ = is_download_completed;
  }
  void set_is_completed_download_deleted(bool is_completed_download_deleted) {
    is_completed_download_deleted_ = is_completed_download_deleted;
  }
  bool is_download_completed() { return is_download_completed_; }
  bool is_completed_download_deleted() {
    return is_completed_download_deleted_;
  }
  const base::Value::Dict& json() const { return json_; }
  void set_json(base::Value::Dict json_item) { json_ = std::move(json_item); }

  void OnItemUpdated() { ++updated_; }
  void OnChangedFired() { ++changed_fired_; }

  void BeginFilenameDetermination(
      ExtensionDownloadsEventRouterEx::FilenameChangedCallback
          filename_changed);

  void DetermineFilenameTimeout() { CallFilenameCallback(); }

  void CallFilenameCallback();

  void ClearPendingDeterminers();

  void DeterminerRemoved(const ExtensionId& extension_id);

  void AddPendingDeterminer(const ExtensionId& extension_id,
                            const base::Time& installed);

  bool DeterminerAlreadyReported(const ExtensionId& extension_id);

  void CreatorSuggestedFilename(
      const base::FilePath& filename,
      downloads::FilenameConflictAction conflict_action);

  base::FilePath creator_suggested_filename() const {
    return creator_suggested_filename_;
  }

  downloads::FilenameConflictAction creator_conflict_action() const {
    return creator_conflict_action_;
  }

  void ResetCreatorSuggestion() {
    DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
    creator_suggested_filename_.clear();
    creator_conflict_action_ = downloads::FilenameConflictAction::kUniquify;
  }

  // Returns false if this |extension_id| was not expected or if this
  // |extension_id| has already reported. The caller is responsible for
  // validating |filename|.
  bool DeterminerCallback(content::BrowserContext* browser_context,
                          const ExtensionId& extension_id,
                          const base::FilePath& filename,
                          downloads::FilenameConflictAction conflict_action);

 private:
  static int determine_filename_timeout_s_;

  struct DeterminerInfo {
    DeterminerInfo();
    DeterminerInfo(const ExtensionId& e_id, const base::Time& installed);
    ~DeterminerInfo();

    ExtensionId extension_id;
    base::Time install_time;
    bool reported;
  };
  typedef std::vector<DeterminerInfo> DeterminerInfoVector;

  static const char kKey[];

  // This is safe to call even while not waiting for determiners to call back;
  // in that case, the callbacks will be null so they won't be Run.
  void CheckAllDeterminersCalled();

  int updated_;
  int changed_fired_;
  // Dictionary representing the current state of the download. It is cleared
  // when download completes.
  base::Value::Dict json_;

  ExtensionDownloadsEventRouterEx::FilenameChangedCallback filename_changed_;

  DeterminerInfoVector determiners_;

  base::FilePath creator_suggested_filename_;
  downloads::FilenameConflictAction creator_conflict_action_;
  base::FilePath determined_filename_;
  downloads::FilenameConflictAction determined_conflict_action_;
  DeterminerInfo determiner_;

  // Whether a download is complete and whether the completed download is
  // deleted.
  bool is_download_completed_;
  bool is_completed_download_deleted_;

  int download_id_;

  std::unique_ptr<base::WeakPtrFactory<ExtensionDownloadsEventRouterDataEx>>
      weak_ptr_factory_;
};
}  // namespace extensions

#endif