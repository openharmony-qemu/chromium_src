/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "libcef/browser/devtools/arkweb/devtools_frontend.h"

#include <cstddef>
#include <iomanip>
#include <utility>

#include "libcef/browser/browser_context.h"
#include "libcef/browser/devtools/devtools_manager_delegate.h"
#include "libcef/browser/net/devtools_scheme_handler.h"
#include "libcef/browser/thread_util.h"
#include "libcef/common/cef_switches.h"
#include "libcef/common/task_runner_manager.h"

#include "base/base64.h"
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/guid.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/json/string_escape.h"
#include "base/memory/ptr_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/pref_names.h"
#include "components/prefs/scoped_user_pref_update.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/file_url_loader.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/shared_cors_origin_access_list.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_client.h"
#include "content/public/common/url_constants.h"
#include "content/public/common/url_utils.h"
#include "ipc/ipc_channel.h"
#include "net/base/completion_once_callback.h"
#include "net/base/io_buffer.h"
#include "net/base/net_errors.h"
#include "net/http/http_response_headers.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/cpp/simple_url_loader_stream_consumer.h"
#include "services/network/public/cpp/wrapper_shared_url_loader_factory.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "storage/browser/file_system/native_file_util.h"

#if BUILDFLAG(IS_WIN)
#include <windows.h>
#elif BUILDFLAG(IS_POSIX)
#include <time.h>
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "cef/ohos_cef_ext/libcef/browser/chrome/extensions/arkweb_chrome_extension_util_ext.h"
#include "chrome/browser/extensions/extension_management.h"
#include "chrome/browser/extensions/extension_service.h"
#include "extensions/common/manifest_url_handlers.h"
#include "extensions/common/manifest_handlers/devtools_page_handler.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_util.h"
#include "extensions/common/mojom/api_permission_id.mojom-shared.h"
#include "extensions/common/permissions/permissions_data.h"
#endif // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#if BUILDFLAG(ARKWEB_DEVTOOLS)
#include "chrome/browser/profiles/profile.h"
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

namespace {

// This constant should be in sync with the constant in
// chrome/browser/devtools/devtools_ui_bindings.cc.
constexpr size_t kMaxMessageChunkSize = IPC::Channel::kMaximumMessageSize / 4;

constexpr int kMaxLogLineLength = 1024;

#if BUILDFLAG(ARKWEB_DEVTOOLS)
static const char kTitleFormat[] = "DevTools - %s";
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

static std::string GetFrontendURL() {
#if BUILDFLAG(ARKWEB_DEVTOOLS)
  return base::StringPrintf("%s://%s/devtools_app.html?can_dock=true&dockSide=undocked",
                            content::kChromeDevToolsScheme,
                            scheme::kChromeDevToolsHost);
#else
  return base::StringPrintf("%s://%s/devtools_app.html",
                            content::kChromeDevToolsScheme,
                            scheme::kChromeDevToolsHost);
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)
}

base::Value::Dict BuildObjectForResponse(const net::HttpResponseHeaders* rh,
                                         bool success,
                                         int net_error) {
  base::Value::Dict response;
  int responseCode = 200;
  if (rh) {
    responseCode = rh->response_code();
  } else if (!success) {
    // In case of no headers, assume file:// URL and failed to load
    responseCode = 404;
  }
  response.Set("statusCode", responseCode);
  response.Set("netError", net_error);
  response.Set("netErrorName", net::ErrorToString(net_error));

  base::Value::Dict headers;
  size_t iterator = 0;
  std::string name;
  std::string value;
  // TODO(caseq): this probably needs to handle duplicate header names
  // correctly by folding them.
  while (rh && rh->EnumerateHeaderLines(&iterator, &name, &value)) {
    headers.Set(name, value);
  }

  response.Set("headers", std::move(headers));
  return response;
}

void WriteTimestamp(std::stringstream& stream) {
#if BUILDFLAG(IS_WIN)
  SYSTEMTIME local_time;
  GetLocalTime(&local_time);
  stream << std::setfill('0') << std::setw(2) << local_time.wMonth
         << std::setw(2) << local_time.wDay << '/' << std::setw(2)
         << local_time.wHour << std::setw(2) << local_time.wMinute
         << std::setw(2) << local_time.wSecond << '.' << std::setw(3)
         << local_time.wMilliseconds;
#elif BUILDFLAG(IS_POSIX)
  timeval tv;
  gettimeofday(&tv, nullptr);
  time_t t = tv.tv_sec;
  struct tm local_time;
  localtime_r(&t, &local_time);
  struct tm* tm_time = &local_time;
  stream << std::setfill('0') << std::setw(2) << 1 + tm_time->tm_mon
         << std::setw(2) << tm_time->tm_mday << '/' << std::setw(2)
         << tm_time->tm_hour << std::setw(2) << tm_time->tm_min << std::setw(2)
         << tm_time->tm_sec << '.' << std::setw(6) << tv.tv_usec;
#else
#error Unsupported platform
#endif
}

void LogProtocolMessage(const base::FilePath& log_file,
                        ProtocolMessageType type,
                        std::string to_log) {
  // Track if logging has failed, in which case we don't keep trying.
  static bool log_error = false;
  if (log_error) {
    return;
  }

  if (storage::NativeFileUtil::EnsureFileExists(log_file, nullptr) !=
      base::File::FILE_OK) {
    LOG(ERROR) << "Failed to create file " << log_file.value();
    log_error = true;
    return;
  }

  std::string type_label;
  switch (type) {
    case ProtocolMessageType::METHOD:
      type_label = "METHOD";
      break;
    case ProtocolMessageType::RESULT:
      type_label = "RESULT";
      break;
    case ProtocolMessageType::EVENT:
      type_label = "EVENT";
      break;
  }

  std::stringstream stream;
  WriteTimestamp(stream);
  stream << ": " << type_label << ": " << to_log << "\n";
  const std::string& str = stream.str();
  if (!base::AppendToFile(log_file, std::string_view(str))) {
    LOG(ERROR) << "Failed to write file " << log_file.value();
    log_error = true;
  }
}

#if BUILDFLAG(ARKWEB_DEVTOOLS)
base::Value::Dict CreateFileSystemValue(
    CefDevToolsFileManager::FileSystem file_system) {
  base::Value::Dict file_system_value;
  file_system_value.Set("type", file_system.type);
  file_system_value.Set("fileSystemName", file_system.file_system_name);
  file_system_value.Set("rootURL", file_system.root_url);
  file_system_value.Set("fileSystemPath", file_system.file_system_path);
  return file_system_value;
}
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

}  // namespace

class CefDevToolsFrontend::NetworkResourceLoader
    : public network::SimpleURLLoaderStreamConsumer {
 public:
  NetworkResourceLoader(int stream_id,
                        CefDevToolsFrontend* bindings,
                        std::unique_ptr<network::SimpleURLLoader> loader,
                        network::mojom::URLLoaderFactory* url_loader_factory,
                        int request_id)
      : stream_id_(stream_id),
        bindings_(bindings),
        loader_(std::move(loader)),
        request_id_(request_id) {
    loader_->SetOnResponseStartedCallback(base::BindOnce(
        &NetworkResourceLoader::OnResponseStarted, base::Unretained(this)));
    loader_->DownloadAsStream(url_loader_factory, this);
  }

  NetworkResourceLoader(const NetworkResourceLoader&) = delete;
  NetworkResourceLoader& operator=(const NetworkResourceLoader&) = delete;

 private:
  void OnResponseStarted(const GURL& final_url,
                         const network::mojom::URLResponseHead& response_head) {
    response_headers_ = response_head.headers;
  }

  void OnDataReceived(std::string_view chunk,
                      base::OnceClosure resume) override {
    base::Value chunkValue;

    bool encoded = !base::IsStringUTF8(chunk);
    if (encoded) {
      chunkValue = base::Value(base::Base64Encode(chunk));
    } else {
      chunkValue = base::Value(chunk);
    }
    base::Value id(stream_id_);
    base::Value encodedValue(encoded);

    bindings_->CallClientFunction("DevToolsAPI", "streamWrite", std::move(id),
                                  std::move(chunkValue),
                                  std::move(encodedValue));
    std::move(resume).Run();
  }

  void OnComplete(bool success) override {
    auto response = BuildObjectForResponse(response_headers_.get(), success,
                                           loader_->NetError());
    bindings_->SendMessageAck(request_id_, std::move(response));

    bindings_->loaders_.erase(bindings_->loaders_.find(this));
  }

  void OnRetry(base::OnceClosure start_retry) override { DCHECK(false); }

  const int stream_id_;
  CefDevToolsFrontend* const bindings_;
  std::unique_ptr<network::SimpleURLLoader> loader_;
  int request_id_;
  scoped_refptr<net::HttpResponseHeaders> response_headers_;
};

// static
CefDevToolsFrontend* CefDevToolsFrontend::Show(
    AlloyBrowserHostImpl* inspected_browser,
    const CefWindowInfo& windowInfo,
    CefRefPtr<CefClient> client,
    const CefBrowserSettings& settings,
    const CefPoint& inspect_element_at,
    base::OnceClosure frontend_destroyed_callback) {
  CefBrowserSettings new_settings = settings;
  if (!windowInfo.windowless_rendering_enabled &&
      CefColorGetA(new_settings.background_color) != SK_AlphaOPAQUE) {
    // Use white as the default background color for windowed DevTools instead
    // of the CefSettings.background_color value.
    new_settings.background_color = SK_ColorWHITE;
  }

  CefBrowserCreateParams create_params;
  if (inspected_browser->is_views_hosted()) {
    create_params.popup_with_views_hosted_opener = true;
  } else {
    create_params.window_info.reset(new CefWindowInfo(windowInfo));
  }
  create_params.client = client;
  create_params.settings = new_settings;
  create_params.devtools_opener = inspected_browser;
  create_params.request_context = inspected_browser->GetRequestContext();
  create_params.extra_info = inspected_browser->browser_info()->extra_info();

  CefRefPtr<AlloyBrowserHostImpl> frontend_browser =
      AlloyBrowserHostImpl::Create(create_params);

  content::WebContents* inspected_contents = inspected_browser->web_contents();

  // CefDevToolsFrontend will delete itself when the frontend WebContents is
  // destroyed.
  CefDevToolsFrontend* devtools_frontend = new CefDevToolsFrontend(
      static_cast<AlloyBrowserHostImpl*>(frontend_browser.get()),
      inspected_contents, inspect_element_at,
      std::move(frontend_destroyed_callback));

  // Need to load the URL after creating the DevTools objects.
  frontend_browser->GetMainFrame()->LoadURL(GetFrontendURL());

  return devtools_frontend;
}

#if BUILDFLAG(ARKWEB_DEVTOOLS)
// static
CefDevToolsFrontend* CefDevToolsFrontend::ShowWith(
      CefRefPtr<ArkWebBrowserHostExt> frontend_browser,
      CefRefPtr<CefDevToolsMessageHandlerDelegate> devtools_message_handler,
      content::WebContents* inspected_contents,
      const CefPoint& inspect_element_at,
      base::OnceClosure frontend_destroyed_callback) {
  LOG(INFO) << "CefDevToolsFrontend::ShowWith({"
            << inspect_element_at.x << "*" << inspect_element_at.y << "})";
  AlloyBrowserHostImpl* alloy_frontend_browser = static_cast<AlloyBrowserHostImpl*>(frontend_browser.get());
  auto handler = std::make_unique<CefDevToolsMessageHandler>(
      std::move(devtools_message_handler),
      Profile::FromBrowserContext(
          alloy_frontend_browser->web_contents()->GetBrowserContext()));
  // CefDevToolsFrontend will delete itself when the frontend WebContents is
  // destroyed.
  CefDevToolsFrontend* devtools_frontend = new CefDevToolsFrontend(
      alloy_frontend_browser, std::move(handler),
      inspected_contents, inspect_element_at,
      std::move(frontend_destroyed_callback));

  // Need to load the URL after creating the DevTools objects.
  alloy_frontend_browser->GetMainFrame()->LoadURL(GetFrontendURL());

  return devtools_frontend;
}
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

void CefDevToolsFrontend::Activate() {
  frontend_browser_->ActivateContents(web_contents());
}

void CefDevToolsFrontend::Focus() {
  frontend_browser_->SetFocus(true);
}

void CefDevToolsFrontend::InspectElementAt(int x, int y) {
  if (inspect_element_at_.x != x || inspect_element_at_.y != y) {
    inspect_element_at_.Set(x, y);
  }
  if (agent_host_) {
    agent_host_->InspectElement(inspected_contents_->GetFocusedFrame(), x, y);
  }
}

void CefDevToolsFrontend::Close() {
  CEF_POST_TASK(CEF_UIT, base::BindOnce(&AlloyBrowserHostImpl::CloseBrowser,
                                        frontend_browser_.get(), true));
}

CefDevToolsFrontend::CefDevToolsFrontend(
    AlloyBrowserHostImpl* frontend_browser,
    content::WebContents* inspected_contents,
    const CefPoint& inspect_element_at,
    base::OnceClosure frontend_destroyed_callback)
    : content::WebContentsObserver(frontend_browser->web_contents()),
      frontend_browser_(frontend_browser),
      inspected_contents_(inspected_contents),
      inspect_element_at_(inspect_element_at),
      frontend_destroyed_callback_(std::move(frontend_destroyed_callback)),
      file_manager_(frontend_browser, GetPrefs()),
      protocol_log_file_(
          base::CommandLine::ForCurrentProcess()->GetSwitchValuePath(
              switches::kDevToolsProtocolLogFile)),
      weak_factory_(this) {
  DCHECK(!frontend_destroyed_callback_.is_null());
}

#if BUILDFLAG(ARKWEB_DEVTOOLS)
CefDevToolsFrontend::CefDevToolsFrontend(
    AlloyBrowserHostImpl* frontend_browser,
    std::unique_ptr<CefDevToolsMessageHandler> devtools_message_handler,
    content::WebContents* inspected_contents,
    const CefPoint& inspect_element_at,
    base::OnceClosure frontend_destroyed_callback)
    : content::WebContentsObserver(frontend_browser->web_contents()),
      frontend_browser_(frontend_browser),
      inspected_contents_(inspected_contents),
      inspect_element_at_(inspect_element_at),
      frontend_destroyed_callback_(std::move(frontend_destroyed_callback)),
      file_manager_(frontend_browser, this, GetPrefs()),
      protocol_log_file_(
          base::CommandLine::ForCurrentProcess()->GetSwitchValuePath(
              switches::kDevToolsProtocolLogFile)),
      devtools_message_handler_(std::move(devtools_message_handler)),
      weak_factory_(this) {
  DCHECK(!frontend_destroyed_callback_.is_null());
  file_manager_.SetDevToolsMessageHandler(devtools_message_handler_.get());
}
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

CefDevToolsFrontend::~CefDevToolsFrontend() {}

void CefDevToolsFrontend::ReadyToCommitNavigation(
    content::NavigationHandle* navigation_handle) {
  content::RenderFrameHost* frame = navigation_handle->GetRenderFrameHost();
  if (navigation_handle->IsInMainFrame()) {
#if BUILDFLAG(ARKWEB_DEVTOOLS)
    if (agent_host_.get()) {
      agent_host_->DetachClient(this);
      agent_host_->AttachClient(this);
    }
    if (frontend_host_) {
      return;
    }
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)
    frontend_host_ = content::DevToolsFrontendHost::Create(
        frame, base::BindRepeating(
                   &CefDevToolsFrontend::HandleMessageFromDevToolsFrontend,
                   base::Unretained(this)));
    return;
  }

  std::string origin =
      navigation_handle->GetURL().DeprecatedGetOriginAsURL().spec();
  auto it = extensions_api_.find(origin);
  if (it == extensions_api_.end()) {
    return;
  }
  std::string script = base::StringPrintf("%s(\"%s\")", it->second.c_str(),
                                          base::Uuid::GenerateRandomV4().AsLowercaseString().c_str());
  content::DevToolsFrontendHost::SetupExtensionsAPI(frame, script);
}

void CefDevToolsFrontend::PrimaryMainDocumentElementAvailable() {
  // Don't call AttachClient multiple times for the same DevToolsAgentHost.
  // Otherwise it will call AgentHostClosed which closes the DevTools window.
  // This may happen in cases where the DevTools content fails to load.
  scoped_refptr<content::DevToolsAgentHost> agent_host =
      content::DevToolsAgentHost::GetOrCreateFor(inspected_contents_);
  if (agent_host != agent_host_) {
    if (agent_host_) {
      agent_host_->DetachClient(this);
    }
    agent_host_ = agent_host;
    agent_host_->AttachClient(this);
    if (!inspect_element_at_.IsEmpty()) {
#if BUILDFLAG(ARKWEB_DEVTOOLS)
      content::RenderFrameHost* rfh = inspected_contents_->GetFocusedFrame();
      if (!rfh) {
        rfh = inspected_contents_->GetPrimaryMainFrame();
      }
      if (!rfh) {
        return;
      }
      agent_host_->InspectElement(rfh,
                                  inspect_element_at_.x, inspect_element_at_.y);
#else
      agent_host_->InspectElement(inspected_contents_->GetFocusedFrame(),
                                  inspect_element_at_.x, inspect_element_at_.y);
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)
    }
  }
}

void CefDevToolsFrontend::WebContentsDestroyed() {
  if (agent_host_) {
    agent_host_->DetachClient(this);
    agent_host_ = nullptr;
  }
  std::move(frontend_destroyed_callback_).Run();
  delete this;
}

void CefDevToolsFrontend::HandleMessageFromDevToolsFrontend(
    base::Value::Dict message) {
  const std::string* method = message.FindString("method");
  if (!method) {
    return;
  }

  int request_id = message.FindInt("id").value_or(0);
  base::Value::List* params_value = message.FindList("params");

  // Since we've received message by value, we can take the list.
  base::Value::List params;
  if (params_value) {
    params = std::move(*params_value);
  }

#if BUILDFLAG(ARKWEB_DEVTOOLS)
  if (devtools_message_handler_) {
    auto ret = devtools_message_handler_->HandleMessage(
        request_id, *method, params);
    if (ret.handled) {
      if (request_id) {
        SendMessageAck(request_id, std::move(ret.result));
      }
      return;
    }
  }
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

  if (*method == "dispatchProtocolMessage") {
    if (params.size() < 1) {
      return;
    }
    const std::string* protocol_message = params[0].GetIfString();
    if (!agent_host_ || !protocol_message) {
      return;
    }
    if (ProtocolLoggingEnabled()) {
      LogProtocolMessage(ProtocolMessageType::METHOD, *protocol_message);
    }
    agent_host_->DispatchProtocolMessage(
        this, base::as_bytes(base::span(*protocol_message)));
  } else if (*method == "loadCompleted") {
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
    int tab_id = cef::GetTabIdForWebContents(inspected_contents_);
    CallClientFunction("DevToolsAPI", "setInspectedTabId", base::Value(tab_id));

    AddDevToolsExtensionsToClient();
#endif // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

    auto* rfh = web_contents()->GetPrimaryMainFrame();
    if (rfh == nullptr) {
      return;
    }
    rfh->ExecuteJavaScriptForTests(
        u"DevToolsAPI.setUseSoftMenu(true);", base::NullCallback(), 0);
  } else if (*method == "loadNetworkResource") {
    if (params.size() < 3) {
      return;
    }

    // TODO(pfeldman): handle some of the embedder messages in content.
    const std::string* url = params[0].GetIfString();
    const std::string* headers = params[1].GetIfString();
    std::optional<const int> stream_id = params[2].GetIfInt();
    if (!url || !headers || !stream_id.has_value()) {
      return;
    }

    GURL gurl(*url);
    if (!gurl.is_valid()) {
      base::Value::Dict response;
      response.Set("statusCode", 404);
      response.Set("urlValid", false);
      SendMessageAck(request_id, std::move(response));
      return;
    }

    net::NetworkTrafficAnnotationTag traffic_annotation =
        net::DefineNetworkTrafficAnnotation(
            "devtools_handle_front_end_messages", R"(
            semantics {
              sender: "Developer Tools"
              description:
                "When user opens Developer Tools, the browser may fetch "
                "additional resources from the network to enrich the debugging "
                "experience (e.g. source map resources)."
              trigger: "User opens Developer Tools to debug a web page."
              data: "Any resources requested by Developer Tools."
              destination: OTHER
            }
            policy {
              cookies_allowed: YES
              cookies_store: "user"
              setting:
                "It's not possible to disable this feature from settings."
              chrome_policy {
                DeveloperToolsAvailability {
                  policy_options {mode: MANDATORY}
                  DeveloperToolsAvailability: 2
                }
              }
            })");

    // Based on DevToolsUIBindings::LoadNetworkResource.
    auto resource_request = std::make_unique<network::ResourceRequest>();
    resource_request->url = gurl;
    // TODO(caseq): this preserves behavior of URLFetcher-based
    // implementation. We really need to pass proper first party origin from
    // the front-end.
    resource_request->site_for_cookies = net::SiteForCookies::FromUrl(gurl);
    resource_request->headers.AddHeadersFromString(*headers);

    scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory;
    if (gurl.SchemeIsFile()) {
      mojo::PendingRemote<network::mojom::URLLoaderFactory> pending_remote =
          content::CreateFileURLLoaderFactory(
              base::FilePath() /* profile_path */,
              nullptr /* shared_cors_origin_access_list */);
      url_loader_factory = network::SharedURLLoaderFactory::Create(
          std::make_unique<network::WrapperPendingSharedURLLoaderFactory>(
              std::move(pending_remote)));
    } else if (content::HasWebUIScheme(gurl)) {
      base::Value::Dict response;
      response.Set("statusCode", 403);
      SendMessageAck(request_id, std::move(response));
      return;
    } else {
      auto* rfh = inspected_contents_->GetPrimaryMainFrame();
      if (rfh == nullptr) {
        return;
      }
      auto* partition = rfh->GetStoragePartition();
      url_loader_factory = partition->GetURLLoaderFactoryForBrowserProcess();
    }

    auto simple_url_loader = network::SimpleURLLoader::Create(
        std::move(resource_request), traffic_annotation);
    auto resource_loader = std::make_unique<NetworkResourceLoader>(
        *stream_id, this, std::move(simple_url_loader),
        url_loader_factory.get(), request_id);
    loaders_.insert(std::move(resource_loader));
    return;
  } else if (*method == "getPreferences") {
    SendMessageAck(request_id,
                   GetPrefs()->GetDict(prefs::kDevToolsPreferences).Clone());
    return;
  } else if (*method == "setPreference") {
    if (params.size() < 2) {
      return;
    }
    const std::string* name = params[0].GetIfString();

    // We're just setting params[1] as a value anyways, so just make sure it's
    // the type we want, but don't worry about getting it.
    if (!name || !params[1].is_string()) {
      return;
    }

    ScopedDictPrefUpdate update(GetPrefs(), prefs::kDevToolsPreferences);
    update->Set(*name, std::move(params[1]));
  } else if (*method == "removePreference") {
    const std::string* name = params[0].GetIfString();
    if (!name) {
      return;
    }
    ScopedDictPrefUpdate update(GetPrefs(), prefs::kDevToolsPreferences);
    update->Remove(*name);
  } else if (*method == "requestFileSystems") {
#if BUILDFLAG(ARKWEB_DEVTOOLS)
    RequestFileSystems();
#else
    web_contents()->GetPrimaryMainFrame()->ExecuteJavaScriptForTests(
        u"DevToolsAPI.fileSystemsLoaded([]);", base::NullCallback(), 0);
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)
  } else if (*method == "reattach") {
    if (!agent_host_) {
      return;
    }
    agent_host_->DetachClient(this);
    agent_host_->AttachClient(this);
  } else if (*method == "registerExtensionsAPI") {
    if (params.size() < 2) {
      return;
    }
    const std::string* origin = params[0].GetIfString();
    const std::string* script = params[1].GetIfString();
    if (!origin || !script) {
      return;
    }
    extensions_api_[*origin + "/"] = *script;
  } else if (*method == "save") {
    if (params.size() < 3) {
      return;
    }
    const std::string* url = params[0].GetIfString();
    const std::string* content = params[1].GetIfString();
    std::optional<bool> save_as = params[2].GetIfBool();
    if (!url || !content || !save_as.has_value()) {
      return;
    }
    file_manager_.SaveToFile(*url, *content, *save_as);
  } else if (*method == "append") {
    if (params.size() < 2) {
      return;
    }
    const std::string* url = params[0].GetIfString();
    const std::string* content = params[1].GetIfString();
    if (!url || !content) {
      return;
    }
    file_manager_.AppendToFile(*url, *content);
#if BUILDFLAG(ARKWEB_DEVTOOLS)
  } else if (*method == "openInNewTab") {
    if (params.size() < 1) {
      return;
    }
    const std::string* url = params[0].GetIfString();
    if (url == nullptr) {
      return;
    }
    content::OpenURLParams urlParams(GURL(*url), content::Referrer(),
                                     WindowOpenDisposition::NEW_FOREGROUND_TAB,
                                     ui::PAGE_TRANSITION_LINK, false);
    if (inspected_contents_ != nullptr) {
      inspected_contents_->OpenURL(urlParams, {});
    }
    return;
  } else if (*method == "addFileSystem") {
    if (params.size() < 1) {
      return;
    }
    const std::string* type = params[0].GetIfString();
    if (!type) {
      return;
    }
    file_manager_.AddFileSystem(*type);
  } else if (*method == "removeFileSystem") {
    if (params.size() < 1) {
      return;
    }
    const std::string* file_system_path = params[0].GetIfString();
    if (!file_system_path) {
      return;
    }
    file_manager_.RemoveFileSystem(*file_system_path);
  } else if (*method == "inspectedURLChanged") {
    if (params.size() < 1) {
      return;
    }
    const std::string* url = params[0].GetIfString();
    if (!url) {
      return;
    }
    content::NavigationController& controller = web_contents()->GetController();
    content::NavigationEntry* entry = controller.GetActiveEntry();
    const std::string kHttpPrefix = "http://";
    const std::string kHttpsPrefix = "https://";
    const std::string simplified_url =
        base::StartsWith(*url, kHttpsPrefix, base::CompareCase::SENSITIVE)
            ? url->substr(kHttpsPrefix.length())
            : base::StartsWith(*url, kHttpPrefix, base::CompareCase::SENSITIVE)
                  ? url->substr(kHttpPrefix.length())
                  : *url;
    // DevTools UI is not localized.
    web_contents()->UpdateTitleForEntry(
        entry, base::UTF8ToUTF16(
                   base::StringPrintf(kTitleFormat, simplified_url.c_str())));

    return;
  } else if (*method == "getHostConfig") {
    SendMessageAck(request_id, base::Value::Dict());
    return;
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)
  } else {
    return;
  }

  if (request_id) {
    SendMessageAck(request_id, base::Value::Dict());
  }
}

void CefDevToolsFrontend::DispatchProtocolMessage(
    content::DevToolsAgentHost* agent_host,
    base::span<const uint8_t> message) {
  if (!frontend_browser_->GetWebContents() ||
      frontend_browser_->GetWebContents()->IsBeingDestroyed()) {
    return;
  }

  std::string_view str_message(reinterpret_cast<const char*>(message.data()),
                                message.size());
  if (ProtocolLoggingEnabled()) {
    // Quick check to avoid parsing the JSON object. Events begin with a
    // "method" value whereas method results begin with an "id" value.
    LogProtocolMessage(base::StartsWith(str_message, "{\"method\":")
                           ? ProtocolMessageType::EVENT
                           : ProtocolMessageType::RESULT,
                       str_message);
  }

  if (str_message.length() < kMaxMessageChunkSize) {
    CallClientFunction("DevToolsAPI", "dispatchMessage",
                       base::Value(std::string(str_message)));
  } else {
    size_t total_size = str_message.length();
    for (size_t pos = 0; pos < str_message.length();
         pos += kMaxMessageChunkSize) {
      std::string_view str_message_chunk =
          str_message.substr(pos, kMaxMessageChunkSize);

      CallClientFunction(
          "DevToolsAPI", "dispatchMessageChunk",
          base::Value(std::string(str_message_chunk)),
          base::Value(base::NumberToString(pos ? 0 : total_size)));
    }
  }
}

void CefDevToolsFrontend::CallClientFunction(
    const std::string& object_name,
    const std::string& method_name,
    base::Value arg1,
    base::Value arg2,
    base::Value arg3,
    base::OnceCallback<void(base::Value)> cb) {
  std::string javascript;
  auto* rfh = web_contents()->GetPrimaryMainFrame();
  if (rfh == nullptr) {
    return;
  }

  rfh->AllowInjectingJavaScript();

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
      std::move(arguments), std::move(cb));
}

void CefDevToolsFrontend::SendMessageAck(int request_id,
                                         base::Value::Dict arg) {
  CallClientFunction("DevToolsAPI", "embedderMessageAck",
                     base::Value(request_id), base::Value(std::move(arg)));
}

bool CefDevToolsFrontend::ProtocolLoggingEnabled() const {
  return !protocol_log_file_.empty();
}

void CefDevToolsFrontend::LogProtocolMessage(ProtocolMessageType type,
                                             const std::string_view& message) {
  DCHECK(ProtocolLoggingEnabled());

  std::string to_log(message.substr(0, kMaxLogLineLength));

  // Execute in an ordered context that allows blocking.
  auto task_runner = CefTaskRunnerManager::Get()->GetBackgroundTaskRunner();
  task_runner->PostTask(
      FROM_HERE, base::BindOnce(::LogProtocolMessage, protocol_log_file_, type,
                                std::move(to_log)));
}

void CefDevToolsFrontend::AgentHostClosed(
    content::DevToolsAgentHost* agent_host) {
  DCHECK(agent_host == agent_host_.get());
  agent_host_ = nullptr;
  Close();
}

PrefService* CefDevToolsFrontend::GetPrefs() const {
  auto* web_contents = frontend_browser_->web_contents();
  if (web_contents == nullptr) {
    return nullptr;
  }
  auto* browser_context = CefBrowserContext::FromBrowserContext(web_contents->GetBrowserContext());
  if (browser_context == nullptr) {
    return nullptr;
  }
  return browser_context->AsProfile()->GetPrefs();
}

#if BUILDFLAG(ARKWEB_DEVTOOLS)
void CefDevToolsFrontend::FileSystemAdded(
    const std::string& error,
    const CefDevToolsFileManager::FileSystem* file_system) {
  if (file_system) {
    CallClientFunction("DevToolsAPI", "fileSystemAdded", base::Value(error),
        base::Value(CreateFileSystemValue(*file_system)));
  } else {
    CallClientFunction("DevToolsAPI", "fileSystemAdded", base::Value(error));
  }
}

void CefDevToolsFrontend::FileSystemRemoved(const std::string& file_system_path) {
  CallClientFunction("DevToolsAPI", "fileSystemRemoved",
      base::Value(file_system_path));
}

void CefDevToolsFrontend::FilePathsChanged(
    const std::vector<std::string>& changed_paths,
    const std::vector<std::string>& added_paths,
    const std::vector<std::string>& removed_paths) {
  const int kMaxPathsPerMessage = 1000;
  size_t changed_index = 0;
  size_t added_index = 0;
  size_t removed_index = 0;
  // Dispatch limited amount of file paths in a time to avoid
  // IPC max message size limit. See https://crbug.com/797817.
  while (changed_index < changed_paths.size() ||
         added_index < added_paths.size() ||
         removed_index < removed_paths.size()) {
    int budget = kMaxPathsPerMessage;
    base::Value::List changed;
    base::Value::List added;
    base::Value::List removed;
    while (budget > 0 && changed_index < changed_paths.size()) {
      changed.Append(changed_paths[changed_index++]);
      --budget;
    }
    while (budget > 0 && added_index < added_paths.size()) {
      added.Append(added_paths[added_index++]);
      --budget;
    }
    while (budget > 0 && removed_index < removed_paths.size()) {
      removed.Append(removed_paths[removed_index++]);
      --budget;
    }
    CallClientFunction("DevToolsAPI", "fileSystemFilesChangedAddedRemoved",
                     base::Value(std::move(changed)),
                     base::Value(std::move(added)),
                     base::Value(std::move(removed)));
  }
}

void CefDevToolsFrontend::SendMessageAck(int request_id,
                                         base::Value arg) {
  CallClientFunction("DevToolsAPI", "embedderMessageAck",
                     base::Value(request_id), std::move(arg));
}

void CefDevToolsFrontend::RequestFileSystems() {
  base::Value::List file_systems_value;
  for (auto const& file_system : file_manager_.GetFileSystems()) {
    file_systems_value.Append(CreateFileSystemValue(file_system));
  }
  CallClientFunction("DevToolsAPI", "fileSystemsLoaded",
      base::Value(std::move(file_systems_value)));
}
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
base::Value::Dict CefDevToolsFrontend::BuildExtensionInfo(
    const extensions::Extension* extension) {
  GURL url =
      extensions::chrome_manifest_urls::GetDevToolsPage(extension);
  const bool is_extension_url =
      (url.SchemeIs(extensions::kExtensionScheme)
       || url.SchemeIs(extensions::kArkwebExtensionScheme)) &&
      url.host_piece() == extension->id();
  DCHECK(is_extension_url || url.SchemeIsHTTPOrHTTPS());

  // Each devtools extension will need to be able to run in the devtools
  // process. Grant the devtools process the ability to request URLs from the
  // extension.
  content::ChildProcessSecurityPolicy::GetInstance()->GrantRequestOrigin(
      web_contents()->GetPrimaryMainFrame()->GetProcess()->GetID().GetUnsafeValue(),
      url::Origin::Create(extension->url()));

  extensions::ExtensionManagement* management =
      extensions::ExtensionManagementFactory::GetForBrowserContext(
          web_contents()->GetBrowserContext());
  base::Value::List runtime_allowed_hosts;
  std::vector<std::string> allowed_hosts =
      management->GetPolicyAllowedHosts(extension).ToStringVector();
  for (auto& host : allowed_hosts) {
    runtime_allowed_hosts.Append(std::move(host));
  }
  base::Value::List runtime_blocked_hosts;
  std::vector<std::string> blocked_hosts =
      management->GetPolicyBlockedHosts(extension).ToStringVector();
  for (auto& host : blocked_hosts) {
    runtime_blocked_hosts.Append(std::move(host));
  }

  base::Value::Dict extension_info;
  extension_info.Set("startPage", url.spec());
  extension_info.Set("name", extension->name());
  extension_info.Set("exposeExperimentalAPIs",
                     extension->permissions_data()->HasAPIPermission(
                         extensions::mojom::APIPermissionID::kExperimental));
  extension_info.Set("allowFileAccess", extensions::util::AllowFileAccess(
                                            extension->id(),
                                            web_contents()->GetBrowserContext()));
  extension_info.Set(
      "hostsPolicy",
      base::Value::Dict()
          .Set("runtimeAllowedHosts", std::move(runtime_allowed_hosts))
          .Set("runtimeBlockedHosts", std::move(runtime_blocked_hosts)));
  return extension_info;
}

void CefDevToolsFrontend::AddDevToolsExtensionsToClient() {
  // See DevToolsUIBindings::AddDevToolsExtensionsToClient.
  const extensions::ExtensionRegistry* registry =
      extensions::ExtensionRegistry::Get(web_contents()->GetBrowserContext());
  if (!registry) {
    return;
  }

  base::Value::List results;
  base::Value::List forbidden_origins;
  for (const scoped_refptr<const extensions::Extension>& extension :
       registry->enabled_extensions()) {
    if (extensions::Manifest::IsComponentLocation(extension->location())) {
      forbidden_origins.Append(extension->origin().Serialize());
    }
    if (extensions::chrome_manifest_urls::GetDevToolsPage(extension.get())
            .is_empty()) {
      continue;
    }
    results.Append(BuildExtensionInfo(extension.get()));
  }


  CallClientFunction("DevToolsAPI", "setOriginsForbiddenForExtensions",
                     base::Value(std::move(forbidden_origins)));
  CallClientFunction("DevToolsAPI", "addExtensions",
                     base::Value(std::move(results)));
}
#endif // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
