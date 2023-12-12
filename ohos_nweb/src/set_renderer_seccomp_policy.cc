#include "nweb_export.h"
#include "sandbox/linux/seccomp-bpf-helpers/seccomp_starter_ohos.h"
#include "nweb_hilog.h"
#include "sandbox/linux/seccomp-bpf-helpers/baseline_policy_ohos.h"
#include <unistd.h>

extern "C" OHOS_NWEB_EXPORT bool SetRendererSeccompPolicy() {
#if defined(__arm__) || defined(__aarch64__)
    sandbox::SeccompStarterOhos starter;

    starter.set_policy(std::make_unique<sandbox::BaselinePolicyOhos>());

    starter.StartSandbox();

    if (starter.status() == sandbox::SeccompSandboxStatus::ENGAGED) {
        WVLOG_I("Successfully set seccomp policy.");
        return true;
    }
    return false;
#else
    return true;
#endif
}