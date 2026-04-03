// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef ARKWEB_COMPONENTS_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_ITEM_IMPL_EXT_H_
#define ARKWEB_COMPONENTS_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_ITEM_IMPL_EXT_H_

#include <stdint.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "base/files/file_path.h"
#include "base/functional/callback_forward.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "base/threading/thread_checker.h"
#include "base/threading/thread_checker_impl.h"
#include "base/time/time.h"
#include "components/download/public/common/download_item.h"
#include "components/download/public/common/download_item_impl.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "services/device/public/mojom/wake_lock_provider.mojom.h"
#include "services/network/public/mojom/fetch_api.mojom-shared.h"
#include "url/gurl.h"
#include "url/origin.h"

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
#include "base/supports_user_data.h"
#endif

namespace download {

// See download_item.h for usage.
class COMPONENTS_DOWNLOAD_EXPORT ArkWebDownloadItemImplExt : public DownloadItemImpl {
 public:
  friend DownloadItemImpl;
#if BUILDFLAG(ARKWEB_UNITTESTS)
  friend class ArkWebDownloadItemImplExtTest;
#endif
  ArkWebDownloadItemImplExt *AsArkWebDownloadItemImplExt() override { return this; }

  // Constructing from persistent store:
  // |net_log| is constructed externally for our use.
  ArkWebDownloadItemImplExt(
      DownloadItemImplDelegate* delegate,
      const std::string& guid,
      uint32_t id,
      const base::FilePath& current_path,
      const base::FilePath& target_path,
      const std::vector<GURL>& url_chain,
      const GURL& referrer_url,
      const std::string& serialized_embedder_download_data,
      const GURL& tab_url,
      const GURL& tab_referrer_url,
      const std::optional<url::Origin>& request_initiator,
      const std::string& mime_type,
      const std::string& original_mime_type,
      base::Time start_time,
      base::Time end_time,
      const std::string& etag,
      const std::string& last_modified,
      int64_t received_bytes,
      int64_t total_bytes,
      int32_t auto_resume_count,
      const std::string& hash,
      DownloadItem::DownloadState state,
      DownloadDangerType danger_type,
      DownloadInterruptReason interrupt_reason,
      bool paused,
      bool allow_metered,
      bool opened,
      base::Time last_access_time,
      bool transient,
      const std::vector<DownloadItem::ReceivedSlice>& received_slices,
      int64_t range_request_from,
      int64_t range_request_to,
      std::unique_ptr<DownloadEntry> download_entry)
      : DownloadItemImpl(
          delegate,
          guid,
          id,
          current_path,
          target_path,
          url_chain,
          referrer_url,
          serialized_embedder_download_data,
          tab_url,
          tab_referrer_url,
          request_initiator,
          mime_type,
          original_mime_type,
          start_time,
          end_time,
          etag,
          last_modified,
          received_bytes,
          total_bytes,
          auto_resume_count,
          hash,
          state,
          danger_type,
          interrupt_reason,
          paused,
          allow_metered,
          opened,
          last_access_time,
          transient,
          received_slices,
          range_request_from,
          range_request_to,
          std::move(download_entry)) {}

  // Constructing for a regular download.
  // |net_log| is constructed externally for our use.
  ArkWebDownloadItemImplExt(DownloadItemImplDelegate* delegate,
                            uint32_t id,
                            const DownloadCreateInfo& info)
      : DownloadItemImpl(delegate,
                        id,
                        info) {}

  // Constructing for the "Save Page As..." feature:
  // |net_log| is constructed externally for our use.
  ArkWebDownloadItemImplExt(DownloadItemImplDelegate* delegate,
                            uint32_t id,
                            const base::FilePath& path,
                            const GURL& url,
                            const std::string& mime_type,
                            DownloadJob::CancelRequestCallback cancel_request_callback)
      : DownloadItemImpl(delegate,
                         id,
                         path,
                         url,
                         mime_type,
                         std::move(cancel_request_callback)) {}

  ArkWebDownloadItemImplExt(const ArkWebDownloadItemImplExt&) = delete;
  ArkWebDownloadItemImplExt& operator=(const ArkWebDownloadItemImplExt&) = delete;

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  ~ArkWebDownloadItemImplExt() override;
  void ReadDownloadData(
      const std::string& guid,
      const int32_t read_size,
      base::OnceCallback<void(const std::vector<uint8_t>&)> callback) override;
  const std::string& GetRequestMethod() const;
  bool IsBeforeInProgress() const;
  bool IsDownloadInProgress() const;

  struct COMPONENTS_DOWNLOAD_EXPORT RequestMethodData
      : public base::SupportsUserData::Data {
    std::string request_method_;
    RequestMethodData(std::string request_method) {
      request_method_ = request_method;
    }
  };

  struct COMPONENTS_DOWNLOAD_EXPORT NWebIdData
      : public base::SupportsUserData::Data {
    int nweb_id_;
    NWebIdData(int nweb_id) { nweb_id_ = nweb_id; }
  };
  void Cancel(bool user_cancel) override;

  void UpdateFullPath(const base::FilePath& new_path);
  void UpdateFullPathOverWriteState(bool overwrite_state);
  bool IsFullPathOverWrite() const; 
  void SetByExtensionId(const std::string& extension_id);
  void SetByExtensionName(const std::string& extension_name);
  void SetConflictAction(int conflict_action);
  void SetContextType(const std::string& context_type);
  std::string GetByExtensionId() const;
  std::string GetByExtensionName() const;
  int GetConflictAction() const;
  std::string GetContextType() const;
#endif  //  BUILDFLAG(ARKWEB_EXT_DOWNLOAD)

 private:
#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  void AutoResume();
  void IsNeedAutoResume(DownloadInterruptReason reason, ResumeMode& resume_mode, bool& need_auto_resume);
  bool CheckIsNeedAutoResume(DownloadInterruptReason reason);
  bool IsAllowedAutoResume();
  void ReadDownloadDataInternal(
      const std::string& guid,
      const int32_t read_size,
      base::OnceCallback<void(const std::vector<uint8_t>&)> callback);

  void ReadDataFromDownloadFileDone(const std::vector<uint8_t>& data);

  void ReadDownloadDataAndRunCallback(int32_t size);

  void RunCallbackIfStateMatch();

  void RunCallbackIfExistsCallback();

  void RegisterReadDownloadCallback(base::OnceCallback<void()> callback,
                                    int32_t size);

  base::OnceCallback<void(const std::vector<uint8_t>&)>
      read_download_callback_from_ui_;
  uint32_t read_download_size_;
  std::string extension_id_;
  std::string extension_name_;
  std::string context_type_;
  int conflict_action_{0};
  bool is_full_path_overwrite_ = false;
#endif

  base::WeakPtrFactory<ArkWebDownloadItemImplExt> weak_ptr_factory_{this};
};

}  // namespace download

#endif  // ARKWEB_COMPONENTS_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_ITEM_IMPL_EXT_H_
