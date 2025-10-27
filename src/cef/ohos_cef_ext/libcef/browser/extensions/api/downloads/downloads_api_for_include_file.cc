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

class DownloadsEraseFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.erase", DOWNLOADS_ERASE)
  DownloadsEraseFunction();

  DownloadsEraseFunction(const DownloadsEraseFunction&) = delete;
  DownloadsEraseFunction& operator=(const DownloadsEraseFunction&) = delete;

  ResponseAction Run() override;

  static void EraseCallback(
      const base::WeakPtr<DownloadsEraseFunction>& function,
      std::optional<std::string> error,
      std::vector<int32_t>& erase_id);
  bool call_downloads_erase_ = false;

 protected:
  ~DownloadsEraseFunction() override;
  base::WeakPtrFactory<DownloadsEraseFunction> weak_ptr_factory_{this};
};

class DownloadsOpenFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.open", DOWNLOADS_OPEN)
  DownloadsOpenFunction();

  DownloadsOpenFunction(const DownloadsOpenFunction&) = delete;
  DownloadsOpenFunction& operator=(const DownloadsOpenFunction&) = delete;

  ResponseAction Run() override;

  static void OpenCallback(const base::WeakPtr<DownloadsOpenFunction>& function,
                           std::optional<std::string> error);

  bool call_downloads_open_ = false;

 protected:
  ~DownloadsOpenFunction() override;
  base::WeakPtrFactory<DownloadsOpenFunction> weak_ptr_factory_{this};
};

class DownloadsRemoveFileFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.removeFile", DOWNLOADS_REMOVEFILE)
  DownloadsRemoveFileFunction();

  DownloadsRemoveFileFunction(const DownloadsRemoveFileFunction&) = delete;
  DownloadsRemoveFileFunction& operator=(const DownloadsRemoveFileFunction&) =
      delete;

  ResponseAction Run() override;

  static void RemoveFileCallback(
      const base::WeakPtr<DownloadsRemoveFileFunction>& function,
      std::optional<std::string> error);

  bool call_downloads_remove_file_ = false;

 protected:
  ~DownloadsRemoveFileFunction() override;
  base::WeakPtrFactory<DownloadsRemoveFileFunction> weak_ptr_factory_{this};
};

class DownloadsPauseFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.pause", DOWNLOADS_PAUSE)
  DownloadsPauseFunction();

  DownloadsPauseFunction(const DownloadsPauseFunction&) = delete;
  DownloadsPauseFunction& operator=(const DownloadsPauseFunction&) = delete;

  ResponseAction Run() override;

  static void PauseCallback(
      const base::WeakPtr<DownloadsPauseFunction>& function,
      std::optional<std::string> error);

  bool call_downloads_pause_ = false;

 protected:
  ~DownloadsPauseFunction() override;
  base::WeakPtrFactory<DownloadsPauseFunction> weak_ptr_factory_{this};
};

class DownloadsResumeFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.resume", DOWNLOADS_RESUME)
  DownloadsResumeFunction();

  DownloadsResumeFunction(const DownloadsResumeFunction&) = delete;
  DownloadsResumeFunction& operator=(const DownloadsResumeFunction&) = delete;

  ResponseAction Run() override;

  static void ResumeCallback(
      const base::WeakPtr<DownloadsResumeFunction>& function,
      std::optional<std::string> error);

  bool call_downloads_resume_ = false;

 protected:
  ~DownloadsResumeFunction() override;
  base::WeakPtrFactory<DownloadsResumeFunction> weak_ptr_factory_{this};
};

class DownloadsCancelFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.cancel", DOWNLOADS_CANCEL)
  DownloadsCancelFunction();

  DownloadsCancelFunction(const DownloadsCancelFunction&) = delete;
  DownloadsCancelFunction& operator=(const DownloadsCancelFunction&) = delete;

  ResponseAction Run() override;

  static void CancelCallback(
      const base::WeakPtr<DownloadsCancelFunction>& function,
      std::optional<std::string> error);

  bool call_downloads_cancel_ = false;

 protected:
  ~DownloadsCancelFunction() override;
  base::WeakPtrFactory<DownloadsCancelFunction> weak_ptr_factory_{this};
};

class DownloadsAcceptDangerFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.acceptDanger", DOWNLOADS_ACCEPTDANGER)
  DownloadsAcceptDangerFunction();

  DownloadsAcceptDangerFunction(const DownloadsAcceptDangerFunction&) = delete;
  DownloadsAcceptDangerFunction& operator=(
      const DownloadsAcceptDangerFunction&) = delete;

  ResponseAction Run() override;

  static void AcceptDangerCallback(
      const base::WeakPtr<DownloadsAcceptDangerFunction>& function,
      std::optional<std::string> error);

  bool call_downloads_accept_danger_ = false;

 protected:
  ~DownloadsAcceptDangerFunction() override;
  base::WeakPtrFactory<DownloadsAcceptDangerFunction> weak_ptr_factory_{this};
};

class DownloadsSetUiOptionsFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.setUiOptions", DOWNLOADS_SETUIOPTIONS)
  DownloadsSetUiOptionsFunction();

  DownloadsSetUiOptionsFunction(const DownloadsSetUiOptionsFunction&) = delete;
  DownloadsSetUiOptionsFunction& operator=(
      const DownloadsSetUiOptionsFunction&) = delete;

  ResponseAction Run() override;

  static void SetUiOptionsCallback(
      const base::WeakPtr<DownloadsSetUiOptionsFunction>& function,
      std::optional<std::string> error);

  bool call_downloads_set_ui_options_ = false;

 protected:
  ~DownloadsSetUiOptionsFunction() override;
  base::WeakPtrFactory<DownloadsSetUiOptionsFunction> weak_ptr_factory_{this};
};

class DownloadsSetShelfEnabledFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.setShelfEnabled",
                             DOWNLOADS_SETSHELFENABLED)
  DownloadsSetShelfEnabledFunction();

  DownloadsSetShelfEnabledFunction(const DownloadsSetShelfEnabledFunction&) =
      delete;
  DownloadsSetShelfEnabledFunction& operator=(
      const DownloadsSetShelfEnabledFunction&) = delete;

  ResponseAction Run() override;

  static void SetShelfEnabledCallback(
      const base::WeakPtr<DownloadsSetShelfEnabledFunction>& function,
      std::optional<std::string> error);

  bool call_downloads_set_shelf_enabled_ = false;

 protected:
  ~DownloadsSetShelfEnabledFunction() override;
  base::WeakPtrFactory<DownloadsSetShelfEnabledFunction> weak_ptr_factory_{
      this};
};

class DownloadsShowFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.show", DOWNLOADS_SHOW)
  DownloadsShowFunction();

  DownloadsShowFunction(const DownloadsShowFunction&) = delete;
  DownloadsShowFunction& operator=(const DownloadsShowFunction&) = delete;

  ResponseAction Run() override;
  static void ShowCallback(const base::WeakPtr<DownloadsShowFunction>& function,
                           std::optional<std::string> error);

  bool call_downloads_show_ = false;

 protected:
  ~DownloadsShowFunction() override;
  base::WeakPtrFactory<DownloadsShowFunction> weak_ptr_factory_{this};
};

class DownloadsShowDefaultFolderFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.showDefaultFolder",
                             DOWNLOADS_SHOWDEFAULTFOLDER)
  DownloadsShowDefaultFolderFunction();

  DownloadsShowDefaultFolderFunction(
      const DownloadsShowDefaultFolderFunction&) = delete;
  DownloadsShowDefaultFolderFunction& operator=(
      const DownloadsShowDefaultFolderFunction&) = delete;

  ResponseAction Run() override;

  bool call_downloads_show_default_folder_ = false;

 protected:
  ~DownloadsShowDefaultFolderFunction() override;
  base::WeakPtrFactory<DownloadsShowDefaultFolderFunction> weak_ptr_factory_{
      this};
};

class DownloadsSearchFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.search", DOWNLOADS_SEARCH)
  DownloadsSearchFunction();

  DownloadsSearchFunction(const DownloadsSearchFunction&) = delete;
  DownloadsSearchFunction& operator=(const DownloadsSearchFunction&) = delete;

  ResponseAction Run() override;

  static void SearchCallback(
      const base::WeakPtr<DownloadsSearchFunction>& function,
      std::optional<std::string> error,
      const uint32_t count,
      std::vector<ExDownloadsItem>& download_items);
  bool call_downloads_search_ = false;

 protected:
  ~DownloadsSearchFunction() override;
  base::WeakPtrFactory<DownloadsSearchFunction> weak_ptr_factory_{this};
};

class DownloadsGetFileIconFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("downloads.getFileIcon", DOWNLOADS_GETFILEICON)
  DownloadsGetFileIconFunction();

  DownloadsGetFileIconFunction(const DownloadsGetFileIconFunction&) = delete;
  DownloadsGetFileIconFunction& operator=(const DownloadsGetFileIconFunction&) =
      delete;

  ResponseAction Run() override;
  static void GetFileIconCallback(
      const base::WeakPtr<DownloadsGetFileIconFunction>& function,
      std::optional<std::string> error,
      std::string iconUrl);
  bool call_downloads_getfileicon_ = false;

 protected:
  ~DownloadsGetFileIconFunction() override;
  base::WeakPtrFactory<DownloadsGetFileIconFunction> weak_ptr_factory_{this};
};