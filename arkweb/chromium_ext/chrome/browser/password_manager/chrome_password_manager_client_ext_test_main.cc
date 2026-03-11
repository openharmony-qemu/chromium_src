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
 
#include "base/test/launcher/unit_test_launcher.h"
#include "cef/include/cef_app.h"
#include "cef/libcef/browser/browser_context.h"
#include "cef/libcef/common/app_manager.h"
#include "chrome/test/base/chrome_unit_test_suite.h"
#include "content/public/common/content_client.h"
#include "content/public/test/unittest_test_suite.h"

#include <utility>

namespace {

class TestCefApp : public CefApp {
 public:
  TestCefApp() = default;
  ~TestCefApp() override = default;

 private:
  IMPLEMENT_REFCOUNTING(TestCefApp);
};

class TestCefBrowserContext : public CefBrowserContext {
 public:
  explicit TestCefBrowserContext(const CefRequestContextSettings& settings)
      : CefBrowserContext(settings) {
    Initialize();
  }

  content::BrowserContext* AsBrowserContext() override { return nullptr; }
  Profile* AsProfile() override { return nullptr; }
  bool IsInitialized() const override { return true; }
  void StoreOrTriggerInitCallback(base::OnceClosure callback) override {
    if (callback) {
      std::move(callback).Run();
    }
  }
  void AddVisitedURLs(const GURL&,
                      const std::vector<GURL>&,
                      ui::PageTransition) override {}
};

class ScopedTestCefAppManager : public CefAppManager {
 public:
  ScopedTestCefAppManager() : application_(new TestCefApp()) {}
  ~ScopedTestCefAppManager() override = default;

  CefRefPtr<CefApp> GetApplication() override { return application_; }

  content::ContentClient* GetContentClient() override {
    content::ContentClient* client = content::GetContentClientForTesting();
    if (client) {
      return client;
    }
    static content::ContentClient fallback_client;
    return &fallback_client;
  }

  CefRefPtr<CefRequestContext> GetGlobalRequestContext() override {
    return nullptr;
  }

  CefBrowserContext* CreateNewBrowserContext(const CefRequestContextSettings& settings,
                                             base::OnceClosure initialized_cb) override {
    if (initialized_cb) {
      std::move(initialized_cb).Run();
    }
    return new TestCefBrowserContext(settings);
  }

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
  CefRefPtr<CefRequestContext> GetGlobalOTRRequestContext() override {
    return nullptr;
  }
#endif

 private:
  CefRefPtr<CefApp> application_;
};

}

int main(int argc, char** argv) {
  ScopedTestCefAppManager scoped_cef_app_manager;
  content::UnitTestTestSuite test_suite(
      new ChromeUnitTestSuite(argc, argv),
      base::BindRepeating(
          &content::UnitTestTestSuite::CreateTestContentClients));
  return base::LaunchUnitTests(
      argc, argv,
      base::BindOnce(&content::UnitTestTestSuite::Run,
                     base::Unretained(&test_suite)));
}
