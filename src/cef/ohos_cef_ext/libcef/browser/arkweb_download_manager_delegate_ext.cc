#include "cef/ohos_cef_ext/libcef/browser/arkweb_download_manager_delegate_ext.h"

#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"
#include "cef/ohos_cef_ext/libcef/browser/arkweb_browser_host_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/cef_download_item_impl_ext.h"
#include "chrome/common/chrome_constants.h"
#include "content/public/common/content_switches.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/download_item_utils.h"
#include "content/public/browser/web_contents.h"
#include "include/cef_download_handler.h"
#include "libcef/browser/alloy/alloy_browser_host_impl.h"
#include "libcef/browser/context.h"
#include "libcef/browser/download_item_impl.h"
#include "libcef/browser/thread_util.h"
#include "net/base/filename_util.h"
#include "third_party/blink/public/mojom/choosers/file_chooser.mojom.h"

using content::DownloadManager;
using content::WebContents;
using download::DownloadItem;

namespace {
// Helper function to retrieve the CefDownloadHandler.
CefRefPtr<CefDownloadHandler>
GetDownloadHandler(CefRefPtr<CefBrowserHostBase> browser) {
  CefRefPtr<CefClient> client = browser->GetClient();
  if (client.get()) {
    return client->GetDownloadHandler();
  }
  return nullptr;
}

void RunDownloadTargetCallback(download::DownloadTargetCallback callback,
                               const base::FilePath &path) {
  download::DownloadTargetInfo target_info;
  target_info.target_path = path;
  target_info.intermediate_path = path;
  std::move(callback).Run(std::move(target_info));
}
} // namespace

// CefDownloadItemCallback implementation.
class ArkWebDownloadItemCallbackImpl : public CefDownloadItemCallback {
 public:
  explicit ArkWebDownloadItemCallbackImpl(
      const base::WeakPtr<DownloadManager>& manager,
      uint32_t download_id)
      : manager_(manager), download_id_(download_id) {}

  ArkWebDownloadItemCallbackImpl(const ArkWebDownloadItemCallbackImpl&) =
      delete;
  ArkWebDownloadItemCallbackImpl& operator=(
      const ArkWebDownloadItemCallbackImpl&) = delete;

  void Cancel() override {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&ArkWebDownloadItemCallbackImpl::DoCancel, this));
  }

  void Pause() override {
    CEF_POST_TASK(CEF_UIT, base::BindOnce(
                               &ArkWebDownloadItemCallbackImpl::DoPause, this));
  }

  void Resume() override {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&ArkWebDownloadItemCallbackImpl::DoResume, this));
  }

 private:
  void DoCancel() {
    if (download_id_ <= 0) {
      return;
    }

    if (manager_) {
      DownloadItem* item = manager_->GetDownload(download_id_);
      bool nweb_ex_download_enabled =
          base::CommandLine::ForCurrentProcess()->HasSwitch(
              switches::kEnableNwebExDownload);
      if (item && (item->GetState() == DownloadItem::IN_PROGRESS ||
                   (item->GetState() == DownloadItem::INTERRUPTED &&
                    nweb_ex_download_enabled))) {
        item->Cancel(true);
      }
    }

    download_id_ = 0;
  }

  void DoPause() {
    if (download_id_ <= 0) {
      return;
    }

    if (manager_) {
      DownloadItem* item = manager_->GetDownload(download_id_);
      if (item && item->GetState() == DownloadItem::IN_PROGRESS) {
        item->Pause();
      }
    }
  }

  void DoResume() {
    if (download_id_ <= 0) {
      return;
    }

    if (manager_) {
      DownloadItem* item = manager_->GetDownload(download_id_);
      if (item && item->CanResume()) {
        item->Resume(true);
      }
    }
  }

  base::WeakPtr<DownloadManager> manager_;
  uint32_t download_id_;

  IMPLEMENT_REFCOUNTING(ArkWebDownloadItemCallbackImpl);
};

// ArkWebBeforeDownloadCallback implementation.
class ArkWebBeforeDownloadCallbackImpl : public CefBeforeDownloadCallback {
 public:
  ArkWebBeforeDownloadCallbackImpl(
      const base::WeakPtr<DownloadManager>& manager,
      uint32_t download_id,
      const base::FilePath& suggested_name,
      download::DownloadTargetCallback callback)
      : manager_(manager),
        download_id_(download_id),
        suggested_name_(suggested_name),
        callback_(std::move(callback)) {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
    download_id_for_ohos_ = download_id;
#endif
  }

  ArkWebBeforeDownloadCallbackImpl(const ArkWebBeforeDownloadCallbackImpl&) =
      delete;
  ArkWebBeforeDownloadCallbackImpl& operator=(
      const ArkWebBeforeDownloadCallbackImpl&) = delete;

  void Continue(const CefString& download_path, bool show_dialog) override {
    if (CEF_CURRENTLY_ON_UIT()) {
      if (download_id_ <= 0) {
        return;
      }

      if (manager_) {
        base::FilePath path = base::FilePath(download_path);
        CEF_POST_USER_VISIBLE_TASK(
            base::BindOnce(&ArkWebBeforeDownloadCallbackImpl::GenerateFilename,
                           manager_, download_id_, suggested_name_, path,
                           show_dialog, std::move(callback_)));
      }

      download_id_ = 0;
    } else {
      CEF_POST_TASK(CEF_UIT,
                    base::BindOnce(&ArkWebBeforeDownloadCallbackImpl::Continue,
                                   this, download_path, show_dialog));
    }
  }

  bool IsDetached() const { return callback_.is_null(); }
  [[nodiscard]] download::DownloadTargetCallback Detach() {
    return std::move(callback_);
  }

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  void Cancel() override {
    if (CEF_CURRENTLY_ON_UIT()) {
      DoCancel();
    } else {
      CEF_POST_TASK(
          CEF_UIT,
          base::BindOnce(&ArkWebBeforeDownloadCallbackImpl::DoCancel, this));
    }
  }

  void Pause() override {
    if (CEF_CURRENTLY_ON_UIT()) {
      DoPause();
    } else {
      CEF_POST_TASK(
          CEF_UIT,
          base::BindOnce(&ArkWebBeforeDownloadCallbackImpl::DoPause, this));
    }
  }

  void Resume() override {
    if (CEF_CURRENTLY_ON_UIT()) {
      DoResume();
    } else {
      CEF_POST_TASK(
          CEF_UIT,
          base::BindOnce(&ArkWebBeforeDownloadCallbackImpl::DoResume, this));
    }
  }
#endif

 private:
  static void GenerateFilename(base::WeakPtr<DownloadManager> manager,
                               uint32_t download_id,
                               const base::FilePath& suggested_name,
                               const base::FilePath& download_path,
                               bool show_dialog,
                               download::DownloadTargetCallback callback) {
    CEF_REQUIRE_BLOCKING();

    base::FilePath suggested_path = download_path;
    if (!suggested_path.empty()) {
      // Create the directory if necessary.
      base::FilePath dir_path = suggested_path.DirName();
      if (!base::DirectoryExists(dir_path) &&
          !base::CreateDirectory(dir_path)) {
        DCHECK(false) << "failed to create the download directory";
        suggested_path.clear();
      }
    }

    if (suggested_path.empty()) {
      if (base::PathService::Get(base::DIR_TEMP, &suggested_path)) {
        // Use the temp directory.
        suggested_path = suggested_path.Append(suggested_name);
      } else {
        // Use the current working directory.
        suggested_path = suggested_name;
      }
    }

    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&ArkWebBeforeDownloadCallbackImpl::ChooseDownloadPath,
                       manager, download_id, suggested_path, show_dialog,
                       std::move(callback)));
  }

  static void ChooseDownloadPath(base::WeakPtr<DownloadManager> manager,
                                 uint32_t download_id,
                                 const base::FilePath& suggested_path,
                                 bool show_dialog,
                                 download::DownloadTargetCallback callback) {
    if (!manager) {
      return;
    }

    DownloadItem* item = manager->GetDownload(download_id);
    if (!item || item->GetState() != DownloadItem::IN_PROGRESS) {
      return;
    }

    bool handled = false;

    if (show_dialog) {
      WebContents* web_contents =
          content::DownloadItemUtils::GetWebContents(item);
      CefRefPtr<CefBrowserHostBase> browser =
          CefBrowserHostBase::GetBrowserForContents(web_contents);
      if (browser.get()) {
        handled = true;

        blink::mojom::FileChooserParams params;
        params.mode = blink::mojom::FileChooserParams::Mode::kSave;
        if (!suggested_path.empty()) {
          params.default_file_name = suggested_path;
          if (!suggested_path.Extension().empty()) {
            params.accept_types.push_back(
                CefString(suggested_path.Extension()));
          }
        }

        browser->RunFileChooserForBrowser(
            params,
            base::BindOnce(
                &ArkWebBeforeDownloadCallbackImpl::ChooseDownloadPathCallback,
                std::move(callback)));
      }
    }

    if (!handled) {
      RunDownloadTargetCallback(std::move(callback), suggested_path);
    }
  }

  static void ChooseDownloadPathCallback(
      download::DownloadTargetCallback callback,
      const std::vector<base::FilePath>& file_paths) {
    DCHECK_LE(file_paths.size(), (size_t)1);

    base::FilePath path;
    if (file_paths.size() > 0) {
      path = file_paths.front();
    }

    // The download will be cancelled if |path| is empty.
    RunDownloadTargetCallback(std::move(callback), path);
  }

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  void DoCancel() {
    if (download_id_for_ohos_ <= 0) {
      return;
    }

    if (manager_) {
      DownloadItem* item = manager_->GetDownload(download_id_for_ohos_);
      if (item && item->GetState() == DownloadItem::IN_PROGRESS) {
        item->Cancel(true);
      }
    }

    download_id_for_ohos_ = 0;
  }

  void DoPause() {
    if (download_id_for_ohos_ <= 0) {
      return;
    }

    if (manager_) {
      DownloadItem* item = manager_->GetDownload(download_id_for_ohos_);
      if (item && item->GetState() == DownloadItem::IN_PROGRESS) {
        item->Pause();
      }
    }
  }

  void DoResume() {
    if (download_id_for_ohos_ <= 0) {
      return;
    }

    if (manager_) {
      DownloadItem* item = manager_->GetDownload(download_id_for_ohos_);
      if (item) {
        if (item->CanResume()) {
          item->Resume(true);
        } else {
          LOG(WARNING) << "ArkWebBeforeDownloadCallbackImpl::DoResume failed"
                       << " and will cancel this download_item";
          item->Cancel(true);
        }
      }
    }
  }
#endif

  base::WeakPtr<DownloadManager> manager_;
  uint32_t download_id_;
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  uint32_t download_id_for_ohos_;
#endif
  base::FilePath suggested_name_;
  download::DownloadTargetCallback callback_;

  IMPLEMENT_REFCOUNTING(ArkWebBeforeDownloadCallbackImpl);
};
ArkWebCefDownloadManagerDelegateExt::ArkWebCefDownloadManagerDelegateExt(
    DownloadManager* manager)
    : CefDownloadManagerDelegateImpl(manager, true) {}

void ArkWebCefDownloadManagerDelegateExt::OnDownloadUpdated(
    DownloadItem* download) {
  CefRefPtr<CefDownloadItemImpl> download_item(
      new CefDownloadItemImplExt(download));
  CefRefPtr<CefDownloadItemCallback> callback(
      new ArkWebDownloadItemCallbackImpl(manager_ptr_factory_.GetWeakPtr(),
                                         download->GetId()));
  CefRefPtr<CefDownloadHandler> download_handler_per_context =
      net_service::NetHelpers::GetDownloadHandler();
  if (download_handler_per_context) {
    download_handler_per_context->OnDownloadUpdated(
        nullptr, download_item.get(), callback);
  } else {
    LOG(ERROR) << "download_handler_for_browser_context not set";
  }
}

void ArkWebCefDownloadManagerDelegateExt::OnDownloadCreated(
    DownloadManager* manager,
    DownloadItem* item) {
  GetOrAssociateBrowser(item);
}

bool ArkWebCefDownloadManagerDelegateExt::DetermineDownloadTarget(
    DownloadItem* item,
    download::DownloadTargetCallback* callback) {
  // For new-created download, full path is empty.
  // For resumed download, full path has value. So skip call
  // onBeforeDownload when full path is no empty.
  // The delegate should return |true| if it
  // will determine the target information and will invoke |callback|. The
  // callback may be invoked directly (synchronously). If this function returns
  //|false|, the download manager will continue the download using a default
  // target path.
  if (!item->GetFullPath().empty()) {
    return false;
  }

  // This callback may arrive before OnDownloadCreated, so we allow association
  // from either method.
  CefRefPtr<CefBrowserHostBase> browser = GetOrAssociateBrowser(item);
  int nweb_id = -1;
  if (browser.get()) {
    nweb_id = browser->AsArkWebBrowserHostExtImpl()->GetNWebId();
  } else {
    LOG(ERROR) << "DetermineDownloadTarget associate browser failed";
  }
  base::FilePath suggested_name = net::GenerateFileName(
      item->GetURL(), item->GetContentDisposition(), std::string(),
      item->GetSuggestedFilename(), item->GetMimeType(), "download");

  CefRefPtr<CefDownloadItemImpl> download_item(
      new CefDownloadItemImplExt(item, nweb_id));

  CefRefPtr<CefBeforeDownloadCallback> callbackObj(
      new ArkWebBeforeDownloadCallbackImpl(manager_ptr_factory_.GetWeakPtr(),
                                           item->GetId(), suggested_name,
                                           std::move(*callback)));
  CefRefPtr<CefDownloadHandler> download_handler_per_context =
      net_service::NetHelpers::GetDownloadHandler();
  if (download_handler_per_context) {
    download_handler_per_context->OnBeforeDownload(
        browser.get(), download_item.get(), suggested_name.value(),
        callbackObj);
  } else {
    LOG(ERROR) << "find download_handler_per_context failed, cancel download.";
    item->Cancel(false /*user_cancel*/);
  }
  // Call original download handler.
  CefRefPtr<CefDownloadHandler> handler;
  if (browser.get()) {
    handler = GetDownloadHandler(browser);
  }

  if (handler.get()) {
    handler->OnBeforeDownload(browser.get(), download_item.get(),
                              suggested_name.value(), callbackObj);
  }
  std::ignore = download_item->Detach(nullptr);

  return true;
}

CefRefPtr<CefBrowserHostBase>
ArkWebCefDownloadManagerDelegateExt::GetOrAssociateBrowser(
    download::DownloadItem* item) {
  ItemBrowserMap::const_iterator it = item_browser_map_.find(item);
  if (it != item_browser_map_.end()) {
    return it->second.get();
  }

  CefRefPtr<CefBrowserHostBase> browser;
  content::WebContents* contents =
      content::DownloadItemUtils::GetWebContents(item);
  if (contents) {
    browser = CefBrowserHostBase::GetBrowserForContents(contents);
    LOG_IF(WARNING, !browser) << "No CefBrowser for download item";
  }
  if (!browser) {
    // If download item not find associated browser, still add observer.
  } else {
    item_browser_map_.insert(std::make_pair(item, browser.get()));
    // Register as an observer so that we can cancel associated DownloadItems
    // when the browser is destroyed.
    if (!browser->HasObserver(this)) {
      browser->AddObserver(this);
    }
  }
  item->AddObserver(this);

  return browser;
}
