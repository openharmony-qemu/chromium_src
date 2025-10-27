// Copyright 2019 The Chromium Embedded Framework Authors. Portions copyright
// 2013 The Chromium Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_DEVTOOLS_ARTKWEB_DEVTOOLS_FILE_MANAGER_H_
#define CEF_LIBCEF_BROWSER_DEVTOOLS_ARTKWEB_DEVTOOLS_FILE_MANAGER_H_

#include "base/functional/callback_forward.h"
#include "base/memory/weak_ptr.h"
#include "arkweb/build/features/features.h"

#include <map>
#include <string>

#if BUILDFLAG(ARKWEB_DEVTOOLS)
#include "base/functional/callback_helpers.h"
#include "base/values.h"
#include "base/memory/raw_ptr.h"
#include "chrome/browser/devtools/devtools_file_watcher.h"
#include "components/prefs/pref_change_registrar.h"
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

namespace base {
class FilePath;
class SequencedTaskRunner;
class Value;
}  // namespace base

class AlloyBrowserHostImpl;
class PrefService;

#if BUILDFLAG(ARKWEB_DEVTOOLS)
namespace content {
class WebContents;
} // namespace content

class CefDevToolsMessageHandler;
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

// File management helper for DevTools.
// Based on chrome/browser/devtools/devtools_ui_bindings.cc and
// chrome/browser/devtools/devtools_file_helper.cc.
class CefDevToolsFileManager {
#if BUILDFLAG(ARKWEB_DEVTOOLS)
 public:
  struct FileSystem {
    FileSystem();
    ~FileSystem();
    FileSystem(const FileSystem& other);
    FileSystem(const std::string& type,
               const std::string& file_system_name,
               const std::string& root_url,
               const std::string& file_system_path);

    std::string type;
    std::string file_system_name;
    std::string root_url;
    std::string file_system_path;
  };

  class Delegate {
   public:
    virtual ~Delegate() {}
    virtual void FileSystemAdded(const std::string& error,
                                 const FileSystem* file_system) = 0;
    virtual void FileSystemRemoved(const std::string& file_system_path) = 0;
    virtual void FilePathsChanged(
        const std::vector<std::string>& changed_paths,
        const std::vector<std::string>& added_paths,
        const std::vector<std::string>& removed_paths) = 0;
  };
  CefDevToolsFileManager(AlloyBrowserHostImpl* browser_impl,
                         Delegate* delegate,
                         PrefService* prefs);
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)
  CefDevToolsFileManager(AlloyBrowserHostImpl* browser_impl,
                         PrefService* prefs);

  CefDevToolsFileManager(const CefDevToolsFileManager&) = delete;
  CefDevToolsFileManager& operator=(const CefDevToolsFileManager&) = delete;

  void SaveToFile(const std::string& url,
                  const std::string& content,
                  bool save_as);
  void AppendToFile(const std::string& url, const std::string& content);

#if BUILDFLAG(ARKWEB_DEVTOOLS)
  void SetDevToolsMessageHandler(
      CefDevToolsMessageHandler* devtools_message_Handler);
  void RequestFileSystems();
  std::vector<FileSystem> GetFileSystems();
  void AddFileSystem(const std::string& type);
  void RemoveFileSystem(const std::string& file_system_path);
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

 private:
  // SaveToFile implementation:
  using SaveCallback = base::OnceCallback<void(const std::string&)>;
  using CancelCallback = base::OnceCallback<void()>;
  void Save(const std::string& url,
            const std::string& content,
            bool save_as,
            SaveCallback saveCallback,
            CancelCallback cancelCallback);
  void SaveAsDialogDismissed(const std::string& url,
                             const std::string& content,
                             SaveCallback saveCallback,
                             CancelCallback cancelCallback,
                             const std::vector<base::FilePath>& file_paths);
  void SaveAsFileSelected(const std::string& url,
                          const std::string& content,
                          SaveCallback callback,
                          const base::FilePath& path);
  void FileSavedAs(const std::string& url, const std::string& file_system_path);
  void CanceledFileSaveAs(const std::string& url);

  // AppendToFile implementation:
  using AppendCallback = base::OnceCallback<void(void)>;
  void Append(const std::string& url,
              const std::string& content,
              AppendCallback callback);
  void AppendedTo(const std::string& url);

  void CallClientFunction(const std::string& function_name,
                          const base::Value* arg1,
                          const base::Value* arg2,
                          const base::Value* arg3);

#if BUILDFLAG(ARKWEB_DEVTOOLS)
  void CallClientMethod(
      const std::string& object_name,
      const std::string& method_name,
      const base::Value arg1 = {},
      const base::Value arg2 = {},
      const base::Value arg3 = {},
      base::OnceCallback<void(base::Value)> cb = base::NullCallback());

  bool IsFileSystemAdded(const std::string& file_system_path);
  void FileSystemAdded(
      const std::string& error,
      const FileSystem* file_system);
  void InnerAddFileSystem(
      const std::string& type,
      const base::FilePath& path,
      bool allowed);

  using FileSystemSelectedCallback =
      base::OnceCallback<void(const base::FilePath&)>;
  using FileSystemSelectionCancelCallback = base::OnceCallback<void()>;
  void FileSystemSelectionDialogDismissed(
      FileSystemSelectedCallback selectedCallback,
      FileSystemSelectionCancelCallback cancelCallback,
      const std::vector<base::FilePath>& file_paths);

  void FileSystemPathsSettingChangedOnUI();
  void FilePathsChanged(
      const std::vector<std::string>& changed_paths,
      const std::vector<std::string>& added_paths,
      const std::vector<std::string>& removed_paths);

  void ShowDevToolsInfoBar(
      const std::string& type,
      const base::FilePath& path);
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

  // Guaranteed to outlive this object.
#if BUILDFLAG(ARKWEB_DEVTOOLS)
  raw_ptr<AlloyBrowserHostImpl> browser_impl_;
#else
  AlloyBrowserHostImpl* browser_impl_;
#endif
#if BUILDFLAG(ARKWEB_DEVTOOLS)
  raw_ptr<Delegate> delegate_;
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)
#if BUILDFLAG(ARKWEB_DEVTOOLS)
  raw_ptr<PrefService> prefs_;
#else
  PrefService* prefs_;
#endif

  using PathsMap = std::map<std::string, base::FilePath>;
  PathsMap saved_files_;
  scoped_refptr<base::SequencedTaskRunner> file_task_runner_;

#if BUILDFLAG(ARKWEB_DEVTOOLS)
  raw_ptr<CefDevToolsMessageHandler> devtools_message_handler_ = nullptr;
  raw_ptr<content::WebContents> web_contents_;
  PrefChangeRegistrar pref_change_registrar_;
  using PathToType = std::map<std::string, std::string>;
  PathToType file_system_paths_;
  std::unique_ptr<DevToolsFileWatcher, DevToolsFileWatcher::Deleter>
      file_watcher_;
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

  base::WeakPtrFactory<CefDevToolsFileManager> weak_factory_;
};

#endif  // CEF_LIBCEF_BROWSER_DEVTOOLS_ARTKWEB_DEVTOOLS_FILE_MANAGER_H_
