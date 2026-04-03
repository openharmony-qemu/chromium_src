/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

namespace content {

SavePackage::SavePackage(PageImpl& page,
                         SavePageType save_type,
                         const base::FilePath& file_full_path,
                         SavePageExCallback callback)
    : page_(page.GetWeakPtrImpl()),
      page_url_(GetUrlToBeSaved(&page.GetMainDocument())),
      saved_main_file_path_(file_full_path),
      page_isolation_info_(
          page.GetMainDocument().ComputeIsolationInfoForNavigation(
              page_url_,
              page.GetMainDocument().IsCredentialless(),
              page.GetMainDocument()
                  .GetIsolationInfoForSubresources()
                  .nonce())),
      page_is_outermost_main_frame_(
          page.GetMainDocument().IsOutermostMainFrame()),
      title_(GetTitle(page)),
      start_tick_(base::TimeTicks::Now()),
      save_type_(save_type),
      file_name_set_(&base::FilePath::CompareLessIgnoreCase),
      unique_id_(GetNextSavePackageId()),
      callback_(std::move(callback)) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  InternalInit();
}

void SavePackage::GetSaveInfoEx() {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  if (!page_) {
    if (callback_) {
      std::move(callback_).Run(false);
    }
    return;
  }
  base::FilePath website_save_dir = saved_main_file_path_;
  base::FilePath download_save_dir = saved_main_file_path_;

  std::string mime_type =
      static_cast<PageImpl*>(page_.get())->GetContentsMimeType();
  bool can_save_as_complete = CanSaveAsComplete(mime_type);
  if (download::GetDownloadTaskRunner()) {
    download::GetDownloadTaskRunner()->PostTaskAndReplyWithResult(
        FROM_HERE,
        base::BindOnce(&SavePackage::CreateDirectoryOnFileThread, title_,
                      page_url_, can_save_as_complete, mime_type,
                      website_save_dir, download_save_dir),
        base::BindOnce(&SavePackage::ContinueGetSaveInfoEx, base::WrapRefCounted(this),
                      can_save_as_complete));
  }
}

void SavePackage::ContinueGetSaveInfoEx(bool can_save_as_complete,
                                        base::FilePath suggested_path) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  // The WebContents which owns this SavePackage may have disappeared during
  // the UI->download sequence->UI thread hop of
  // GetSaveInfo->CreateDirectoryOnFileThread->ContinueGetSaveInfo.
  if (!page_ || !download_manager_->GetDelegate()) {
    if (callback_) {
      std::move(callback_).Run(false);
    }
    return;
  }

  base::FilePath::StringType default_extension;
  if (can_save_as_complete)
    default_extension = kDefaultHtmlExtension;

  if (save_type_ == content::SAVE_PAGE_TYPE_AS_MHTML) {
    default_extension = FILE_PATH_LITERAL("mhtml");
    suggested_path =
        suggested_path.ReplaceExtension(default_extension);
  }

  download_manager_->GetDelegate()->SafeBrowsingCallback(
      suggested_path,
      base::BindOnce(&SavePackage::OnPathPickedEx,
                     weak_ptr_factory_.GetWeakPtr()));
}

void SavePackage::OnPathPickedEx(
    SavePackagePathPickedParams params,
    SavePackageDownloadCreatedCallback download_created_callback) {
  if (!page_) {
    if (callback_) {
      std::move(callback_).Run(false);
    }
    return;
  }
  saved_main_file_path_ = params.file_path;
  std::string mime_type =
      static_cast<PageImpl*>(page_.get())->GetContentsMimeType();
  net::GenerateSafeFileName(mime_type, false, &saved_main_file_path_);

  saved_main_directory_path_ = saved_main_file_path_.DirName();
  
  if (save_type_ == SAVE_PAGE_TYPE_AS_COMPLETE_HTML) {
    // Make new directory for saving complete file.
    saved_main_directory_path_ = saved_main_directory_path_.Append(
        saved_main_file_path_.RemoveExtension().BaseName().value() +
        FILE_PATH_LITERAL("_files"));
  }
  Init(std::move(download_created_callback));
}
} // namespace content
