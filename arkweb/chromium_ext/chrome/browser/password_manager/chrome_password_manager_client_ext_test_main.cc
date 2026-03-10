#include "base/test/launcher/unit_test_launcher.h"
#include "cef/include/cef_app.h"
#include "cef/libcef/common/app_manager.h"
#include "chrome/test/base/chrome_unit_test_suite.h"
#include "content/public/common/content_client.h"
#include "content/public/test/unittest_test_suite.h"

namespace {

class TestCefApp : public CefApp {
 public:
  TestCefApp() = default;
  ~TestCefApp() override = default;

 private:
  IMPLEMENT_REFCOUNTING(TestCefApp);
};

class ScopedTestCefAppManager : public CefAppManager {
 public:
  ScopedTestCefAppManager() : application_(new TestCefApp()) {}
  ~ScopedTestCefAppManager() override = default;

  CefRefPtr<CefApp> GetApplication() override { return application_; }

  content::ContentClient* GetContentClient() override {
    return content::GetContentClient();
  }

  CefRefPtr<CefRequestContext> GetGlobalRequestContext() override {
    return nullptr;
  }

  CefBrowserContext* CreateNewBrowserContext(const CefRequestContextSettings&,
                                             base::OnceClosure) override {
    return nullptr;
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
