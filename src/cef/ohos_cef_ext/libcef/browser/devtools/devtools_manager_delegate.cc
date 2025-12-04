// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/devtools/devtools_manager_delegate.h"

#include <stdint.h>

#include <vector>

#include "base/atomicops.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/functional/bind.h"
#include "base/memory/ptr_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "cef/grit/cef_resources.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/devtools_frontend_host.h"
#include "content/public/browser/devtools_socket_factory.h"
#include "content/public/browser/favicon_status.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "net/base/net_errors.h"
#include "net/log/net_log_source.h"
#include "net/socket/tcp_server_socket.h"
#include "ui/base/resource/resource_bundle.h"

#if BUILDFLAG(IS_ARKWEB)
#include <pwd.h>

#include "content/public/browser/android/devtools_auth.h"
#include "net/socket/unix_domain_server_socket_posix.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
namespace content {
bool CanUserConnectToDevTools(
    const net::UnixDomainServerSocket::Credentials& credentials) {
  if (credentials.group_id != credentials.user_id) {
    LOG(WARNING) << "DevTools: credentials.group_id = " << credentials.group_id
                 << " is not equal to credentials.user_id = "
                 << credentials.user_id;
    return false;
  }
  // From processes signed with the same key
  if (credentials.user_id == getuid()) {
    return true;
  }
  struct passwd* creds = getpwuid(credentials.user_id);
  if (!creds || !creds->pw_name) {
    LOG(WARNING) << "DevTools: can't obtain creds for uid "
                 << credentials.user_id;
    return false;
  }
  if (strcmp("root", creds->pw_name) == 0 ||   // For rooted devices
      strcmp("shell", creds->pw_name) == 0) {  // For non-rooted devices
    return true;
  }
  LOG(WARNING) << "DevTools: connection attempt from " << creds->pw_name;
  return false;
}

}  // namespace content
#endif

namespace {

#if BUILDFLAG(IS_ARKWEB)
const char kSocketNameFormat[] = "webview_devtools_remote_%d";
const char kTetheringSocketName[] = "webview_devtools_tethering_%d_%d";
#endif

const int kBackLog = 10;

// Factory for UnixDomainServerSocket.
class UnixDomainServerSocketFactory : public content::DevToolsSocketFactory {
 public:
  explicit UnixDomainServerSocketFactory(const std::string& socket_name)
      : socket_name_(socket_name), last_tethering_socket_(0) {}

  UnixDomainServerSocketFactory(const UnixDomainServerSocketFactory&) = delete;
  UnixDomainServerSocketFactory& operator=(
      const UnixDomainServerSocketFactory&) = delete;

 private:
  // content::DevToolsAgentHost::ServerSocketFactory.
  std::unique_ptr<net::ServerSocket> CreateForHttpServer() override {
    std::unique_ptr<net::UnixDomainServerSocket> socket(
        new net::UnixDomainServerSocket(
            base::BindRepeating(&content::CanUserConnectToDevTools),
            true /* use_abstract_namespace */));
    if (socket->BindAndListen(socket_name_, kBackLog) != net::OK) {
      return nullptr;
    }
    return socket;
  }

  std::unique_ptr<net::ServerSocket> CreateForTethering(
      std::string* name) override {
    *name = base::StringPrintf(kTetheringSocketName, getpid(),
                               ++last_tethering_socket_);
    std::unique_ptr<net::UnixDomainServerSocket> socket(
        new net::UnixDomainServerSocket(
            base::BindRepeating(&content::CanUserConnectToDevTools),
            true /* use_abstract_namespace */));
    if (socket->BindAndListen(*name, kBackLog) != net::OK) {
      return nullptr;
    }

    return socket;
  }

  std::string socket_name_;
  int last_tethering_socket_;
};

#if BUILDFLAG(IS_ARKWEB)
class TCPServerSocketFactory : public content::DevToolsSocketFactory {
 public:
  TCPServerSocketFactory(const std::string& address, uint16_t port)
      : address_(address), port_(port) {}

  TCPServerSocketFactory(const TCPServerSocketFactory&) = delete;
  TCPServerSocketFactory& operator=(const TCPServerSocketFactory&) = delete;

 private:
  // content::DevToolsSocketFactory.
  std::unique_ptr<net::ServerSocket> CreateForHttpServer() override {
    std::unique_ptr<net::ServerSocket> socket(
        new net::TCPServerSocket(nullptr, net::NetLogSource()));
    if (socket->ListenWithAddressAndPort(address_, port_, kBackLog) !=
        net::OK) {
      LOG(INFO) << "TCPServerSocket listen port[" << port_ << "] failed";
      return std::unique_ptr<net::ServerSocket>();
    }
    return socket;
  }

  std::unique_ptr<net::ServerSocket> CreateForTethering(
      std::string* out_name) override {
    return nullptr;
  }

  std::string address_;
  uint16_t port_;
};

std::unique_ptr<content::DevToolsSocketFactory> CreateSocketFactory() {
#if BUILDFLAG(IS_ARKWEB)
  LOG(INFO) << "Domain Socket Entry.";
  return std::make_unique<UnixDomainServerSocketFactory>(
      base::StringPrintf(kSocketNameFormat, getpid()));
#else
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  // See if the user specified a port on the command line. Specifying 0 would
  // result in the selection of an ephemeral port but that doesn't make sense
  // for CEF where the URL is otherwise undiscoverable. Also, don't allow
  // binding of ports between 0 and 1024 exclusive because they're normally
  // restricted to root on Posix-based systems.
  uint16_t port = 0;
  if (command_line.HasSwitch(switches::kRemoteDebuggingPort)) {
    int temp_port;
    std::string port_str =
        command_line.GetSwitchValueASCII(switches::kRemoteDebuggingPort);
    if (base::StringToInt(port_str, &temp_port) && temp_port >= 1024 &&
        temp_port < 65535) {
      port = static_cast<uint16_t>(temp_port);
    } else {
      DLOG(WARNING) << "Invalid http debugger port number " << temp_port;
    }
  }
  if (port == 0) {
    return nullptr;
  }
  return std::unique_ptr<content::DevToolsSocketFactory>(
      new TCPServerSocketFactory("127.0.0.1", port));
#endif
}

#if BUILDFLAG(ARKWEB_DEVTOOLS)
std::unique_ptr<content::DevToolsSocketFactory> CreateSocketFactoryWithPort(
    int32_t port)
{
  uint16_t valid_port = 0;
  const int32_t min_valid_port_value = 1024;
  const int32_t max_valid_port_value = 65535;
  if (port >= min_valid_port_value && port <= max_valid_port_value) {
    valid_port = static_cast<uint16_t>(port);
  }
  if (valid_port == 0) {
    LOG(ERROR) << "Invalid http debugger port : " << port;
    return nullptr;
  }
  return std::unique_ptr<content::DevToolsSocketFactory>(
      new TCPServerSocketFactory("0.0.0.0", valid_port));
}
#endif // ARKWEB_DEVTOOLS
#endif
}  //  namespace

// CefDevToolsManagerDelegate ----------------------------------------------

// static
void CefDevToolsManagerDelegate::StartHttpHandler(
    content::BrowserContext* browser_context) {
  std::unique_ptr<content::DevToolsSocketFactory> socket_factory =
      CreateSocketFactory();
  if (!socket_factory) {
    return;
  }

#if BUILDFLAG(IS_ARKWEB)
  LOG(INFO) << "start remote debugging server";
  if (browser_context == nullptr) {
    content::DevToolsAgentHost::StartRemoteDebuggingServer(
        std::move(socket_factory), base::FilePath(), base::FilePath());
  } else {
    content::DevToolsAgentHost::StartRemoteDebuggingServer(
        std::move(socket_factory), browser_context->GetPath(),
        base::FilePath());
  }
#else
  content::DevToolsAgentHost::StartRemoteDebuggingServer(
      std::move(socket_factory), browser_context->GetPath(), base::FilePath());
#endif

  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kRemoteDebuggingPipe)) {
    content::DevToolsAgentHost::StartRemoteDebuggingPipeHandler(
        base::OnceClosure());
  }
}

#if BUILDFLAG(ARKWEB_DEVTOOLS)
// static
void CefDevToolsManagerDelegate::StartHttpHandlerWithPort(
    content::BrowserContext* browser_context, int32_t port)
{
  std::unique_ptr<content::DevToolsSocketFactory> socket_factory =
      CreateSocketFactoryWithPort(port);
  if (!socket_factory) {
    return;
  }

  LOG(INFO) << "start remote debugging server with port";
  if (browser_context == nullptr) {
    content::DevToolsAgentHost::StartRemoteDebuggingServer(
        std::move(socket_factory), base::FilePath(), base::FilePath());
  } else {
    content::DevToolsAgentHost::StartRemoteDebuggingServer(
        std::move(socket_factory), browser_context->GetPath(),
        base::FilePath());
  }

  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kRemoteDebuggingPipe)) {
    content::DevToolsAgentHost::StartRemoteDebuggingPipeHandler(
        base::OnceClosure());
  }
}
#endif // ARKWEB_DEVTOOLS

// static
void CefDevToolsManagerDelegate::StopHttpHandler() {
  // This is a no-op if the server was never started.
  LOG(INFO) << "stop remote debugging server";
  content::DevToolsAgentHost::StopRemoteDebuggingServer();
}

CefDevToolsManagerDelegate::CefDevToolsManagerDelegate() {}

CefDevToolsManagerDelegate::~CefDevToolsManagerDelegate() {}

scoped_refptr<content::DevToolsAgentHost>
CefDevToolsManagerDelegate::CreateNewTarget(const GURL& url,
                                            TargetType target_type,
                                            bool new_window) {
  // This is reached when the user selects "Open link in new tab" from the
  // DevTools interface.
  // TODO(cef): Consider exposing new API to support this.
  return nullptr;
}

std::string CefDevToolsManagerDelegate::GetDiscoveryPageHTML() {
  const char html[] =
      "<html>"
      "<head><title>WebView remote debugging</title></head>"
      "<body>Please use <a href=\'chrome://inspect\'>chrome://inspect</a>"
      "</body>"
      "</html>";
  return html;
}

bool CefDevToolsManagerDelegate::HasBundledFrontendResources() {
  return true;
}
