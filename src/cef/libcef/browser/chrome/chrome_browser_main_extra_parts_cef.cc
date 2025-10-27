// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/libcef/browser/chrome/chrome_browser_main_extra_parts_cef.h"

#include "base/task/thread_pool.h"
#include "cef/libcef/browser/alloy/dialogs/alloy_constrained_window_views_client.h"
#include "cef/libcef/browser/chrome/chrome_context_menu_handler.h"
#include "cef/libcef/browser/chrome/chrome_startup_browser_creator.h"
#include "cef/libcef/browser/context.h"
#include "cef/libcef/browser/file_dialog_runner.h"
#include "cef/libcef/browser/permission_prompt.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/views/chrome_constrained_window_views_client.h"
#include "components/constrained_window/constrained_window_views.h"

#if BUILDFLAG(IS_LINUX)
#include "base/linux_util.h"
#include "cef/libcef/browser/printing/print_dialog_linux.h"
#endif

#if BUILDFLAG(IS_WIN)
#include "chrome/browser/win/app_icon.h"
#endif

#if BUILDFLAG(ARKWEB_ENCRYPT)
#include "base/path_service.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/grit/branded_strings.h"
#include "components/os_crypt/sync/os_crypt.h"
#include "components/password_manager/core/browser/password_manager_switches.h"
#include "ui/base/l10n/l10n_util.h"
#endif

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
#include "base/base_switches.h"
#include "cef/ohos_cef_ext/libcef/browser/predictors/loading_predictor.h"
#include "cef/ohos_cef_ext/libcef/browser/predictors/loading_predictor_factory.h"
#include "cef/ohos_cef_ext/libcef/browser/predictors/predictor_database.h"
#endif

#include "cef/ohos_cef_ext/libcef/browser/arkweb_request_context_impl_ext.h"

#if BUILDFLAG(ARKWEB_COOKIE)
#include "cef/ohos_cef_ext/libcef/browser/chrome/chrome_browser_main_extra_parts_cef_for_include.cc"
#endif

ChromeBrowserMainExtraPartsCef::ChromeBrowserMainExtraPartsCef() = default;

ChromeBrowserMainExtraPartsCef::~ChromeBrowserMainExtraPartsCef() = default;

void ChromeBrowserMainExtraPartsCef::PostProfileInit(Profile* profile,
                                                     bool is_initial_profile) {
  if (!is_initial_profile) {
    return;
  }

  CefRequestContextSettings settings;
  CefContext::Get()->PopulateGlobalRequestContextSettings(&settings);

  // Use the existing path for the initial profile.
  CefString(&settings.cache_path) = profile->GetPath().value();

  // Create the global RequestContext.
  global_request_context_ =
      CefRequestContextImpl::CreateGlobalRequestContext(settings);

#if BUILDFLAG(ARKWEB_COOKIE)
  CefCookieManagerExt::GetGlobalManager(nullptr);
#endif
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
  // Create the Global off the record request context.
  CefRequestContextSettings off_the_record_settings;
  CefContext::Get()->PopulateGlobalOTRRequestContextSettings(
      &off_the_record_settings);
  global_otr_request_context_ =
      ArkWebRequestContextImplExt::CreateGlobalOTRRequestContext(
          off_the_record_settings);
  CefCookieManagerExt::GetGlobalIncognitoManager(nullptr);
#endif

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  if (base::CommandLine::ForCurrentProcess() &&
      !base::CommandLine::ForCurrentProcess()->HasSwitch(
        switches::kDisableAutoPreconnect)) {
    std::set<std::string> urls =
        predictor::PredictorDatabase::GetInstance()->GetRecentVisitedUrl();
    if (CefBrowserContext::GetAll().size() > 0) {
      auto browser_context = CefBrowserContext::GetAll()[0];
      ohos_predictors::LoadingPredictor* predictor =
          ohos_predictors::LoadingPredictorFactory::GetForBrowserContext(
              browser_context->AsBrowserContext());
      if (!predictor) {
        LOG(ERROR) << "loading predictor is null.";
        return;
      }
      for (auto& url : urls) {
        predictor->PrepareForPageLoad(GURL(url),
                                      ohos_predictors::HintOrigin::OMNIBOX, true);
      }

      std::vector<predictor::PreconnectUrlInfo> preconnect_url_infos =
          std::move(predictor::PredictorDatabase::preconnect_url_info_list);
      for (auto& preconnect_url_info : preconnect_url_infos) {
        predictor->PrepareForPageLoad(GURL(preconnect_url_info.url),
                                      ohos_predictors::HintOrigin::OMNIBOX,
                                      preconnect_url_info.is_preconnectable,
                                      preconnect_url_info.num_sockets);
      }
    }
  }
#endif  // ARKWEB_NO_STATE_PREFETCH
}

void ChromeBrowserMainExtraPartsCef::PostBrowserStart() {
  // Register the callback before ChromeBrowserMainParts::PostBrowserStart
  // allows ProcessSingleton to begin processing messages.
  startup_browser_creator::RegisterProcessCommandLineCallback();

#if BUILDFLAG(IS_LINUX)
  // This may be called indirectly via StartupBrowserCreator::LaunchBrowser.
  // Call it here before blocking is disallowed to avoid assertions.
  base::GetLinuxDistro();
#endif
}

void ChromeBrowserMainExtraPartsCef::PreMainMessageLoopRun() {
  background_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
      {base::TaskPriority::BEST_EFFORT,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});
  user_visible_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
      {base::TaskPriority::USER_VISIBLE,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});
  user_blocking_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
      {base::TaskPriority::USER_BLOCKING,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});

  context_menu::RegisterCallbacks();
  file_dialog_runner::RegisterFactory();
  permission_prompt::RegisterCreateCallback();

#if BUILDFLAG(IS_WIN)
  const auto& settings = CefContext::Get()->settings();
  if (settings.chrome_app_icon_id > 0) {
    SetExeAppIconResourceId(settings.chrome_app_icon_id);
  }
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
  if (g_browser_process) {
    g_browser_process->subresource_filter_ruleset_service();
  }
#endif
}

#if BUILDFLAG(ARKWEB_ENCRYPT)
void ChromeBrowserMainExtraPartsCef::PostCreateMainMessageLoop() {
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();

  // Set up crypt config. This needs to be done before anything starts the
  // network service, as the raw encryption key needs to be shared with the
  // network service for encrypted cookie storage.
  // Based on ChromeBrowserMainPartsLinux::PostCreateMainMessageLoop.
  std::unique_ptr<os_crypt::Config> config =
      std::make_unique<os_crypt::Config>();
  // Forward to os_crypt the flag to use a specific password store.
  config->store =
      command_line->GetSwitchValueASCII(password_manager::kPasswordStore);
  // Forward the product name (defaults to "Chromium").
  config->product_name = l10n_util::GetStringUTF8(IDS_PRODUCT_NAME);
  // OSCrypt may target keyring, which requires calls from the main thread.
  // config->main_thread_runner = content::GetUIThreadTaskRunner({});
  // OSCrypt can be disabled in a special settings file.
  config->should_use_preference =
      command_line->HasSwitch(password_manager::kEnableEncryptionSelection);
  base::PathService::Get(chrome::DIR_USER_DATA, &config->user_data_path);
  DCHECK(!config->user_data_path.empty());
  OSCrypt::SetConfig(std::move(config));
}
#endif

void ChromeBrowserMainExtraPartsCef::ToolkitInitialized() {
  // Override the default Chrome client.
  SetConstrainedWindowViewsClient(CreateAlloyConstrainedWindowViewsClient(
      CreateChromeConstrainedWindowViewsClient()));

#if BUILDFLAG(IS_LINUX)
  auto printing_delegate = new CefPrintingContextLinuxDelegate();
  auto default_delegate =
      ui::PrintingContextLinuxDelegate::SetInstance(printing_delegate);
  printing_delegate->SetDefaultDelegate(default_delegate);
#endif  // BUILDFLAG(IS_LINUX)
}
