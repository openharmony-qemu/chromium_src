// Copyright 2019 The Chromium Embedded Framework Authors. Portions copyright
// 2013 The Chromium Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "libcef/browser/devtools/arkweb/devtools_file_manager.h"

#include "libcef/browser/alloy/alloy_browser_host_impl.h"

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/json/json_writer.h"
#include "base/json/values_util.h"
#include "base/lazy_instance.h"
#include "base/path_service.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/values.h"
#include "chrome/common/pref_names.h"
#include "components/prefs/scoped_user_pref_update.h"
#include "content/public/browser/web_contents.h"

#if BUILDFLAG(ARKWEB_DEVTOOLS)
#include "libcef/browser/devtools/arkweb/devtools_message_handler.h"
#include "base/datashare_uri_utils.h"
#include "chrome/grit/generated_resources.h"
#include "content/public/browser/child_process_security_policy.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/common/url_constants.h"
#include "storage/browser/file_system/file_system_url.h"
#include "storage/browser/file_system/isolated_context.h"
#include "storage/common/file_system/file_system_util.h"
#include "ui/base/l10n/l10n_util.h"
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

namespace {

#if BUILDFLAG(ARKWEB_DEVTOOLS)
static const char kRootName[] = "<root>";
static const char kNoHandler[] = "<no handler>";
static const char kPermissionDenied[] = "<permission denied>";
static const char kSelectionCancelled[] = "<selection cancelled>";
static const char kDevToolsFileSystemPaths[] = "devtools.file_system_paths";
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

base::LazyInstance<base::FilePath>::Leaky g_last_save_path =
    LAZY_INSTANCE_INITIALIZER;

void WriteToFile(const base::FilePath& path, const std::string& content) {
  DCHECK(!path.empty());
  //base::WriteFile(path, content.c_str(), content.length());
  base::WriteFile(path, content.c_str());
}

void AppendToFile(const base::FilePath& path, const std::string& content) {
  DCHECK(!path.empty());
  base::AppendToFile(path, std::string_view(content));
}

#if BUILDFLAG(ARKWEB_DEVTOOLS)
using HandleRealPathsCallback =
      base::OnceCallback<void(const std::vector<base::FilePath>&)>;
void TranslateToRealPaths(
    HandleRealPathsCallback callback,
    const std::vector<base::FilePath>& file_paths) {
  if (!callback) {
    return;
  }
  std::vector<base::FilePath> real_paths;
  for (const auto& path: file_paths) {
    if (path.IsDataShareUri()) {
      real_paths.push_back(base::FilePath(base::GetRealPath(path)));
    } else {
      real_paths.push_back(path);
    }
  }
  std::move(callback).Run(real_paths);
}

void RunOnUIThread(base::OnceClosure callback) {
  content::GetUIThreadTaskRunner({})->PostTask(FROM_HERE, std::move(callback));
}

base::Value::Dict CreateFileSystemValue(
    CefDevToolsFileManager::FileSystem file_system) {
  base::Value::Dict file_system_value;
  file_system_value.Set("type", file_system.type);
  file_system_value.Set("fileSystemName", file_system.file_system_name);
  file_system_value.Set("rootURL", file_system.root_url);
  file_system_value.Set("fileSystemPath", file_system.file_system_path);
  return file_system_value;
}

storage::IsolatedContext* isolated_context() {
  CEF_REQUIRE_UIT();
  storage::IsolatedContext* isolated_context =
      storage::IsolatedContext::GetInstance();
  CHECK(isolated_context);
  return isolated_context;
}

std::string RegisterFileSystem(content::WebContents* web_contents,
                               const base::FilePath& path) {
  CEF_REQUIRE_UIT();
  CHECK(web_contents->GetLastCommittedURL().SchemeIs(
      content::kChromeDevToolsScheme));
  auto* rfh = web_contents->GetPrimaryMainFrame();
  if (rfh == nullptr) {
    return "";
  }
  std::string root_name(kRootName);
  storage::IsolatedContext::ScopedFSHandle file_system =
      isolated_context()->RegisterFileSystemForPath(
          storage::kFileSystemTypeLocal, std::string(), path, &root_name);

  content::ChildProcessSecurityPolicy* policy =
      content::ChildProcessSecurityPolicy::GetInstance();
  content::RenderViewHost* render_view_host =
      rfh->GetRenderViewHost();
  int renderer_id = render_view_host->GetProcess()->GetID();
  policy->GrantReadFileSystem(renderer_id, file_system.id());
  policy->GrantWriteFileSystem(renderer_id, file_system.id());
  policy->GrantCreateFileForFileSystem(renderer_id, file_system.id());
  policy->GrantDeleteFromFileSystem(renderer_id, file_system.id());

  // We only need file level access for reading FileEntries. Saving FileEntries
  // just needs the file system to have read/write access, which is granted
  // above if required.
  if (!policy->CanReadFile(renderer_id, path))
    policy->GrantReadFile(renderer_id, path);
  return file_system.id();
}

CefDevToolsFileManager::FileSystem CreateFileSystemStruct(
    content::WebContents* web_contents,
    const std::string& type,
    const std::string& file_system_id,
    const std::string& file_system_path) {
  const GURL origin =
      web_contents->GetLastCommittedURL().DeprecatedGetOriginAsURL();
  std::string file_system_name =
      storage::GetIsolatedFileSystemName(origin, file_system_id);
  std::string root_url = storage::GetIsolatedFileSystemRootURIString(
      origin, file_system_id, kRootName);
  return CefDevToolsFileManager::FileSystem(
      type, file_system_name, root_url, file_system_path);
}


using PathToType = std::map<std::string, std::string>;
PathToType GetAddedFileSystemPaths(PrefService* prefs) {
  if (!prefs) {
    return {};
  }
  const base::Value::Dict& file_systems_paths_value =
      prefs->GetDict(kDevToolsFileSystemPaths);
  PathToType result;
  for (auto pair : file_systems_paths_value) {
    std::string type =
        pair.second.is_string() ? pair.second.GetString() : std::string();
    result[pair.first] = type;
  }
  return result;
}
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

}  // namespace

#if BUILDFLAG(ARKWEB_DEVTOOLS)
CefDevToolsFileManager::CefDevToolsFileManager(
    AlloyBrowserHostImpl* browser_impl,
    Delegate* delegate,
    PrefService* prefs)
    : browser_impl_(browser_impl),
      delegate_(delegate),
      prefs_(prefs),
      file_task_runner_(
          base::ThreadPool::CreateSequencedTaskRunner({base::MayBlock()})),
      weak_factory_(this) {
  pref_change_registrar_.Init(prefs_);
  web_contents_ = browser_impl_->web_contents();
}
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

CefDevToolsFileManager::CefDevToolsFileManager(
    AlloyBrowserHostImpl* browser_impl,
    PrefService* prefs)
    : browser_impl_(browser_impl),
      prefs_(prefs),
      file_task_runner_(
          base::ThreadPool::CreateSequencedTaskRunner({base::MayBlock()})),
      weak_factory_(this) {}

void CefDevToolsFileManager::SaveToFile(const std::string& url,
                                        const std::string& content,
                                        bool save_as) {
  Save(url, content, save_as,
       base::BindOnce(&CefDevToolsFileManager::FileSavedAs,
                      weak_factory_.GetWeakPtr(), url),
       base::BindOnce(&CefDevToolsFileManager::CanceledFileSaveAs,
                      weak_factory_.GetWeakPtr(), url));
}

void CefDevToolsFileManager::AppendToFile(const std::string& url,
                                          const std::string& content) {
  Append(url, content,
         base::BindOnce(&CefDevToolsFileManager::AppendedTo,
                        weak_factory_.GetWeakPtr(), url));
}

void CefDevToolsFileManager::Save(const std::string& url,
                                  const std::string& content,
                                  bool save_as,
                                  SaveCallback saveCallback,
                                  CancelCallback cancelCallback) {
  auto it = saved_files_.find(url);
  if (it != saved_files_.end() && !save_as) {
    SaveAsFileSelected(url, content, std::move(saveCallback), it->second);
    return;
  }
  base::FilePath initial_path;
  GURL gurl(url);
  std::string suggested_file_name =
      gurl.is_valid() ? gurl.ExtractFileName() : url;

  if (suggested_file_name.length() > 64) {
    suggested_file_name = suggested_file_name.substr(0, 64);
  }
  initial_path = initial_path.AppendASCII(suggested_file_name);

  blink::mojom::FileChooserParams params;
  params.mode = blink::mojom::FileChooserParams::Mode::kSave;
  if (!initial_path.empty()) {
    params.default_file_name = initial_path;
  }

#if BUILDFLAG(ARKWEB_DEVTOOLS)
  auto fileChooserCallback = base::BindOnce(
      &CefDevToolsFileManager::SaveAsDialogDismissed,
      weak_factory_.GetWeakPtr(), url, content,
      std::move(saveCallback), std::move(cancelCallback));
  if (devtools_message_handler_) {
    fileChooserCallback = devtools_message_handler_->ShowFileChooser(
        params, std::move(fileChooserCallback));
  }
  if (fileChooserCallback) {
    auto translateFilePathCallback = base::BindOnce(
        &TranslateToRealPaths, std::move(fileChooserCallback));
    browser_impl_->RunFileChooserForBrowser(
        params, std::move(translateFilePathCallback));
  }
#else
  browser_impl_->RunFileChooserForBrowser(
      params,
      base::BindOnce(&CefDevToolsFileManager::SaveAsDialogDismissed,
                     weak_factory_.GetWeakPtr(), url, content,
                     std::move(saveCallback), std::move(cancelCallback)));
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)
}

void CefDevToolsFileManager::SaveAsDialogDismissed(
    const std::string& url,
    const std::string& content,
    SaveCallback saveCallback,
    CancelCallback cancelCallback,
    const std::vector<base::FilePath>& file_paths) {
  if (file_paths.size() == 1) {
    SaveAsFileSelected(url, content, std::move(saveCallback), file_paths[0]);
  } else {
    std::move(cancelCallback).Run();
  }
}

void CefDevToolsFileManager::SaveAsFileSelected(const std::string& url,
                                                const std::string& content,
                                                SaveCallback callback,
                                                const base::FilePath& path) {
  *g_last_save_path.Pointer() = path;
  saved_files_[url] = path;

  ScopedDictPrefUpdate update(prefs_, prefs::kDevToolsEditedFiles);
  update->Set(base::MD5String(url), base::FilePathToValue(path));
  std::string file_system_path = path.AsUTF8Unsafe();
  std::move(callback).Run(file_system_path);
  file_task_runner_->PostTask(FROM_HERE,
                              base::BindOnce(&::WriteToFile, path, content));
}

void CefDevToolsFileManager::FileSavedAs(const std::string& url,
                                         const std::string& file_system_path) {
  base::Value url_value(url);
  base::Value file_system_path_value(file_system_path);
  CallClientFunction("DevToolsAPI.savedURL", &url_value,
                     &file_system_path_value, nullptr);
}

void CefDevToolsFileManager::CanceledFileSaveAs(const std::string& url) {
  base::Value url_value(url);
  CallClientFunction("DevToolsAPI.canceledSaveURL", &url_value, nullptr,
                     nullptr);
}

void CefDevToolsFileManager::Append(const std::string& url,
                                    const std::string& content,
                                    AppendCallback callback) {
  auto it = saved_files_.find(url);
  if (it == saved_files_.end()) {
    return;
  }
  std::move(callback).Run();
  file_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&::AppendToFile, it->second, content));
}

void CefDevToolsFileManager::AppendedTo(const std::string& url) {
  base::Value url_value(url);
  CallClientFunction("DevToolsAPI.appendedToURL", &url_value, nullptr, nullptr);
}

void CefDevToolsFileManager::CallClientFunction(
    const std::string& function_name,
    const base::Value* arg1,
    const base::Value* arg2,
    const base::Value* arg3) {
  std::string javascript = function_name + "(";
  if (arg1) {
    std::string json;
    base::JSONWriter::Write(*arg1, &json);
    javascript.append(json);
    if (arg2) {
      base::JSONWriter::Write(*arg2, &json);
      javascript.append(", ").append(json);
      if (arg3) {
        base::JSONWriter::Write(*arg3, &json);
        javascript.append(", ").append(json);
      }
    }
  }
  javascript.append(");");
  auto* rfh = browser_impl_->web_contents()->GetPrimaryMainFrame();
  if (rfh != nullptr) {
    rfh->ExecuteJavaScript(
        base::UTF8ToUTF16(javascript), base::NullCallback());
  }
}

#if BUILDFLAG(ARKWEB_DEVTOOLS)
void CefDevToolsFileManager::CallClientMethod(
    const std::string& object_name,
    const std::string& method_name,
    base::Value arg1,
    base::Value arg2,
    base::Value arg3,
    base::OnceCallback<void(base::Value)> completion_callback) {
  // If the client renderer is gone (e.g., the window was closed with both the
  // inspector and client being destroyed), the message can not be sent.
  auto* rfh = web_contents_->GetPrimaryMainFrame();
  if (rfh == nullptr) {
    return;
  }
  if (!rfh->IsRenderFrameLive())
    return;
  base::Value::List arguments;
  if (!arg1.is_none()) {
    arguments.Append(std::move(arg1));
    if (!arg2.is_none()) {
      arguments.Append(std::move(arg2));
      if (!arg3.is_none()) {
        arguments.Append(std::move(arg3));
      }
    }
  }
  rfh->ExecuteJavaScriptMethod(
      base::ASCIIToUTF16(object_name), base::ASCIIToUTF16(method_name),
      std::move(arguments), std::move(completion_callback));
}

void CefDevToolsFileManager::SetDevToolsMessageHandler(
    CefDevToolsMessageHandler* devtools_message_Handler) {
  devtools_message_handler_ = devtools_message_Handler;
}

CefDevToolsFileManager::FileSystem::FileSystem() = default;

CefDevToolsFileManager::FileSystem::~FileSystem() = default;

CefDevToolsFileManager::FileSystem::FileSystem(
    const FileSystem& other) = default;

CefDevToolsFileManager::FileSystem::FileSystem(
    const std::string& type,
    const std::string& file_system_name,
    const std::string& root_url,
    const std::string& file_system_path)
    : type(type),
      file_system_name(file_system_name),
      root_url(root_url),
      file_system_path(file_system_path) {}

std::vector<CefDevToolsFileManager::FileSystem> CefDevToolsFileManager::GetFileSystems() {
  file_system_paths_ = GetAddedFileSystemPaths(prefs_);
  std::vector<FileSystem> file_systems;
  if (!file_watcher_) {
    file_watcher_.reset(new DevToolsFileWatcher(
        base::BindRepeating(&CefDevToolsFileManager::FilePathsChanged,
                            weak_factory_.GetWeakPtr()),
        base::SequencedTaskRunner::GetCurrentDefault()));
    auto change_handler_on_ui = base::BindRepeating(
        &CefDevToolsFileManager::FileSystemPathsSettingChangedOnUI,
        weak_factory_.GetWeakPtr());
    pref_change_registrar_.Add(
        kDevToolsFileSystemPaths,
        base::BindRepeating(RunOnUIThread, change_handler_on_ui));
  }
  for (auto file_system_path : file_system_paths_) {
    base::FilePath path =
        base::FilePath::FromUTF8Unsafe(file_system_path.first);
    std::string file_system_id = RegisterFileSystem(web_contents_, path);
    FileSystem filesystem =
        CreateFileSystemStruct(web_contents_, file_system_path.second,
                               file_system_id, file_system_path.first);
    file_systems.push_back(filesystem);
    file_watcher_->AddWatch(std::move(path));
  }
  return file_systems;
}

void CefDevToolsFileManager::AddFileSystem(const std::string& type) {
  if (!devtools_message_handler_) {
    FileSystemAdded(kNoHandler, nullptr);
    return;
  }

  blink::mojom::FileChooserParams params;
  params.mode = blink::mojom::FileChooserParams::Mode::kUploadFolder;

  auto selectedCallback = base::BindOnce(
      &CefDevToolsFileManager::ShowDevToolsInfoBar,
      weak_factory_.GetWeakPtr(), type);
  auto cancelCallback = base::BindOnce(
      &CefDevToolsFileManager::FileSystemAdded,
      weak_factory_.GetWeakPtr(),
      std::string(kSelectionCancelled), nullptr);

  auto callback = devtools_message_handler_->ShowFileChooser(
      params,
      base::BindOnce(
          &CefDevToolsFileManager::FileSystemSelectionDialogDismissed,
          weak_factory_.GetWeakPtr(),
          std::move(selectedCallback), std::move(cancelCallback)));
  if (callback) {
    std::move(callback).Run({});
  }
}

void CefDevToolsFileManager::RemoveFileSystem(
    const std::string& file_system_path) {
  CEF_REQUIRE_UIT();
  base::FilePath path = base::FilePath::FromUTF8Unsafe(file_system_path);
  isolated_context()->RevokeFileSystemByPath(path);

  ScopedDictPrefUpdate update(prefs_, kDevToolsFileSystemPaths);
  base::Value::Dict& file_systems_paths_value = update.Get();
  file_systems_paths_value.Remove(file_system_path);
}

bool CefDevToolsFileManager::IsFileSystemAdded(
    const std::string& file_system_path) {
  CEF_REQUIRE_UIT();
  const base::Value::Dict& file_systems_paths_value =
      prefs_->GetDict(kDevToolsFileSystemPaths);
  return file_systems_paths_value.Find(file_system_path);
}

void CefDevToolsFileManager::FileSystemAdded(
    const std::string& error,
    const FileSystem* file_system) {
  if (file_system) {
    CallClientMethod("DevToolsAPI", "fileSystemAdded", base::Value(error),
        base::Value(CreateFileSystemValue(*file_system)));
  } else {
    CallClientMethod("DevToolsAPI", "fileSystemAdded", base::Value(error));
  }
}

void CefDevToolsFileManager::RequestFileSystems() {
  base::Value::List file_systems_value;
  for (auto const& file_system : GetFileSystems()) {
    file_systems_value.Append(CreateFileSystemValue(file_system));
  }
  CallClientMethod("DevToolsAPI", "fileSystemsLoaded",
      base::Value(std::move(file_systems_value)));
}

void CefDevToolsFileManager::InnerAddFileSystem(
    const std::string& type,
    const base::FilePath& path,
    bool allowed) {
  LOG(INFO) << "InnerAddFileSystem(" << type
            << ", " << path << ", " << allowed << ")";
  if (!allowed) {
    FileSystemAdded(kPermissionDenied, nullptr);
    return;
  }
  std::string file_system_id = RegisterFileSystem(web_contents_, path);
  std::string file_system_path = path.AsUTF8Unsafe();

  ScopedDictPrefUpdate update(prefs_,
                              kDevToolsFileSystemPaths);
  base::Value::Dict& file_systems_paths_value = update.Get();
  file_systems_paths_value.Set(file_system_path, type);
}

void CefDevToolsFileManager::FileSystemSelectionDialogDismissed(
    FileSystemSelectedCallback selected_cb,
    FileSystemSelectionCancelCallback cancel_cb,
    const std::vector<base::FilePath>& file_paths) {
  if (file_paths.empty()) {
    std::move(cancel_cb).Run();
    return;
  }
  std::move(selected_cb).Run(file_paths[0]);
}

void CefDevToolsFileManager::FileSystemPathsSettingChangedOnUI() {
  CEF_REQUIRE_UIT();
  PathToType remaining;
  remaining.swap(file_system_paths_);
  DCHECK(file_watcher_.get());

  for (auto file_system : GetAddedFileSystemPaths(prefs_)) {
    if (remaining.find(file_system.first) == remaining.end()) {
      base::FilePath path = base::FilePath::FromUTF8Unsafe(file_system.first);
      std::string file_system_id = RegisterFileSystem(web_contents_, path);
      FileSystem filesystem = CreateFileSystemStruct(
          web_contents_, file_system.second, file_system_id, file_system.first);
      delegate_->FileSystemAdded(std::string(), &filesystem);
      file_watcher_->AddWatch(std::move(path));
    } else {
      remaining.erase(file_system.first);
    }
    file_system_paths_[file_system.first] = file_system.second;
  }

  for (auto file_system : remaining) {
    delegate_->FileSystemRemoved(file_system.first);
    base::FilePath path = base::FilePath::FromUTF8Unsafe(file_system.first);
    file_watcher_->RemoveWatch(std::move(path));
  }
}
void CefDevToolsFileManager::FilePathsChanged(
    const std::vector<std::string>& changed_paths,
    const std::vector<std::string>& added_paths,
    const std::vector<std::string>& removed_paths) {
  delegate_->FilePathsChanged(changed_paths, added_paths, removed_paths);
}

void CefDevToolsFileManager::ShowDevToolsInfoBar(
    const std::string& type,
    const base::FilePath& path) {
  std::string file_system_path = path.AsUTF8Unsafe();
  if (IsFileSystemAdded(file_system_path)) {
    RemoveFileSystem(file_system_path);
  }

  std::string path_display_name = path.AsEndingWithSeparator().AsUTF8Unsafe();
  std::string message = l10n_util::GetStringFUTF8(
      IDS_DEV_TOOLS_CONFIRM_ADD_FILE_SYSTEM_MESSAGE,
      base::UTF8ToUTF16(path_display_name));
  auto info_bar_cb = base::BindOnce(
      &CefDevToolsFileManager::InnerAddFileSystem,
      weak_factory_.GetWeakPtr(), type, path);
  if (!devtools_message_handler_) {
    // default is allow to add file system.
    std::move(info_bar_cb).Run(true);
    return;
  }
  devtools_message_handler_->ShowInfoBar(
      message, path_display_name, std::move(info_bar_cb));
}
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)
