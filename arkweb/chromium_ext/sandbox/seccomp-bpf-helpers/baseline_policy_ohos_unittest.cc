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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cerrno>

#include "baseline_policy_ohos.h"
#include "sandbox/linux/bpf_dsl/bpf_dsl.h"
#include "sandbox/linux/seccomp-bpf-helpers/sigsys_handlers.h"
#include "sandbox/linux/system_headers/linux_syscalls.h"
#include "sandbox/linux/bpf_dsl/bpf_dsl_impl.h"


using namespace sandbox;
using sandbox::bpf_dsl::AllOf;
using sandbox::bpf_dsl::Allow;
using sandbox::bpf_dsl::AnyOf;
using sandbox::bpf_dsl::Arg;
using sandbox::bpf_dsl::BoolExpr;
using sandbox::bpf_dsl::If;
using sandbox::bpf_dsl::Error;
using sandbox::bpf_dsl::ResultExpr;

enum class PolicyKind {
    Allow,
    Error,
    Crash,
    Unknown
};

bool IsAllow(const sandbox::bpf_dsl::ResultExpr& expr) {
    return expr == Allow();
}

bool IsErrorWith(const sandbox::bpf_dsl::ResultExpr& expr, int error_code) {
    return expr == Error(error_code);
}

bool IsCrash(const sandbox::bpf_dsl::ResultExpr& expr) {
    if (expr == CrashSIGSYS() || expr == CrashSIGSYSClone()
        || expr == CrashSIGSYSPrctl() || expr == CrashSIGSYSSockopt()
            || expr == CrashSIGSYSIoctl() ) {
        return true;
    }
    return false;
}

PolicyKind GetPolicyKind(const sandbox::bpf_dsl::ResultExpr& expr) {
    if (IsAllow(expr)) { return PolicyKind::Allow; }
    if (IsErrorWith(expr, EPERM) || IsErrorWith(expr, EINVAL) || IsErrorWith(expr, EACCES)) {
        return PolicyKind::Error;
    }
    if (IsCrash(expr)) { return PolicyKind::Crash; }
    return PolicyKind::Unknown;
}

class BaselinePolicyOhosTest : public testing::Test {
protected:
    BaselinePolicyOhosTest() {
        policy_ = new BaselinePolicyOhos(true);
    }

    ~BaselinePolicyOhosTest() {
        delete policy_;
    }
    BaselinePolicyOhos* policy_;
};

#if defined(__arm__) || defined(__aarch64__)
TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST1) {
    int sysno = __NR_fdatasync;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST2) {
    int sysno = __NR_fsync;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST3) {
    int sysno = __NR_ftruncate;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST4) {
    int sysno = __NR_mremap;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST5) {
    int sysno = __NR_pwrite64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST6) {
    int sysno = __NR_getpriority;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST7) {
    int sysno = __NR_setpriority;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST8) {
    int sysno = __NR_sysinfo;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST9) {
    int sysno = __NR_uname;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST10) {
    int sysno = __NR_sigaltstack;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST11) {
    int sysno = __NR_brk;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST12) {
    int sysno = __NR_mlock;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST13) {
    int sysno = __NR_munlock;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST14) {
    int sysno = __NR_munmap;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST15) {
    int sysno = __NR_sched_yield;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST16) {
    int sysno = __NR_nanosleep;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST17) {
    int sysno = __NR_epoll_pwait;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST18) {
    int sysno = __NR_epoll_create1;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST19) {
    int sysno = __NR_epoll_ctl;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST20) {
    int sysno = __NR_lseek;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST21) {
    int sysno = __NR_eventfd2;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST22) {
    int sysno = __NR_fstat;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST23) {
    int sysno = __NR_ppoll;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST24) {
    int sysno = __NR_pselect6;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST25) {
    int sysno = __NR_read;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST26) {
    int sysno = __NR_readv;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST27) {
    int sysno = __NR_pread64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST28) {
    int sysno = __NR_recvfrom;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST29) {
    int sysno = __NR_recvmsg;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST30) {
    int sysno = __NR_sendto;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST31) {
    int sysno = __NR_write;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST32) {
    int sysno = __NR_writev;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST33) {
    int sysno = __NR_pipe2;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST34) {
    int sysno = __NR_gettimeofday;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST35) {
    int sysno = __NR_exit;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST36) {
    int sysno = __NR_exit_group;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST37) {
    int sysno = __NR_wait4;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST38) {
    int sysno = __NR_waitid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST39) {
    int sysno = __NR_rt_sigaction;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST40) {
    int sysno = __NR_rt_sigprocmask;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST41) {
    int sysno = __NR_rt_sigreturn;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST42) {
    int sysno = __NR_rt_sigtimedwait;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST43) {
    int sysno = __NR_capget;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST44) {
    int sysno = __NR_getegid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST45) {
    int sysno = __NR_geteuid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST46) {
    int sysno = __NR_getgid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST47) {
    int sysno = __NR_getgroups;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST48) {
    int sysno = __NR_getpid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST49) {
    int sysno = __NR_getppid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST50) {
    int sysno = __NR_getresgid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST51) {
    int sysno = __NR_getsid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST52) {
    int sysno = __NR_gettid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST53) {
    int sysno = __NR_getuid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST54) {
    int sysno = __NR_getresuid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST55) {
    int sysno = __NR_restart_syscall;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST56) {
    int sysno = __NR_close;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST57) {
    int sysno = __NR_dup;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST58) {
    int sysno = __NR_dup3;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST59) {
    int sysno = __NR_shutdown;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST60) {
    int sysno = __NR_mincore;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST61) {
    int sysno = __NR_memfd_create;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST62) {
    int sysno = __NR_faccessat;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST63) {
    int sysno = __NR_openat;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST64) {
    int sysno = __NR_connect;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST65) {
    int sysno = __NR_readlinkat;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST66) {
    int sysno = __NR_mkdirat;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST67) {
    int sysno = __NR_set_tid_address;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST68) {
    int sysno = __NR_getdents64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST69) {
    int sysno = __NR_getrandom;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST70) {
    int sysno = __NR_prlimit64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST71) {
    int sysno = __NR_sched_setscheduler;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST72) {
    int sysno = __NR_sched_getscheduler;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST73) {
    int sysno = __NR_membarrier;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST74) {
    int sysno = __NR_setitimer;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST75) {
    int sysno = __NR_msync;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST76) {
    int sysno = __NR_set_robust_list;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST77) {
    int sysno = __NR_sched_getparam;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}
#endif

#if defined(__arm__)
TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST78) {
    int sysno = __NR_sched_getaffinity;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST79) {
    int sysno = __NR_ftruncate64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST80) {
    int sysno = __NR_ugetrlimit;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST81) {
    int sysno = __NR_pause;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST82) {
    int sysno = __NR_epoll_create;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST83) {
    int sysno = __NR_epoll_wait;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST84) {
    int sysno = __NR_eventfd;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST85) {
    int sysno = __NR_fstat64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST86) {
    int sysno = __NR__llseek;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST87) {
    int sysno = __NR_poll;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST88) {
    int sysno = __NR__newselect;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST89) {
    int sysno = __NR_send;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST90) {
    int sysno = __NR_pipe;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST91) {
    int sysno = __NR_getegid32;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST92) {
    int sysno = __NR_geteuid32;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST93) {
    int sysno = __NR_getgid32;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST94) {
    int sysno = __NR_getgroups32;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST95) {
    int sysno = __NR_getresgid32;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST96) {
    int sysno = __NR_getuid32;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST97) {
    int sysno = __NR_getresuid32;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST98) {
    int sysno = __NR_dup2;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST99) {
    int sysno = __NR_open;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST100) {
    int sysno = __NR_readlink;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST101) {
    int sysno = __ARM_NR_cacheflush;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST102) {
    int sysno = __ARM_NR_set_tls;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST103) {
    int sysno = __NR_mkdir;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST104) {
    int sysno = __NR_sigreturn;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST105) {
    int sysno = __NR_fork;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST106) {
    int sysno = __NR_access;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST107) {
    int sysno = __NR_statx;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}
#endif

#if defined(__aarch64__)
TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST108) {
    int sysno = __NR_getrlimit;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST109) {
    int sysno = __NR_newfstatat;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST110) {
    int sysno = __NR_fstatfs;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST111) {
    int sysno = __NR_mmap;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}
#endif

#if defined(__arm__) || defined(__aarch64__)
TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST112) {
    int sysno = __NR_socket;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST113) {
    int sysno = __NR_setsockopt;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST114) {
    int sysno = __NR_clock_getres;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST115) {
    int sysno = __NR_ptrace;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST116) {
    int sysno = __NR_madvise;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST117) {
    int sysno = __NR_ioctl;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}
#endif

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST118) {
    int sysno = __NR_clone;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST119) {
    int sysno = __NR_prctl;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST120) {
    int sysno = __NR_timerfd_create;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST121) {
    int sysno = __NR_timerfd_settime;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

#if defined(__arm__) || defined(__aarch64__)
TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST122) {
    int sysno = __NR_setrlimit;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST123) {
    int sysno = __NR_sched_get_priority_max;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST124) {
    int sysno = __NR_sched_get_priority_min;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST125) {
    int sysno = __NR_times;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST126) {
    int sysno = __NR_get_robust_list;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST127) {
    int sysno = __NR_setresuid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST128) {
    int sysno = __NR_unlinkat;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST129) {
    int sysno = __NR_flock;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST130) {
    int sysno = __NR_sched_setaffinity;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST131) {
    int sysno = __NR_getrusage;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST132) {
    int sysno = __NR_process_vm_readv;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST133) {
    int sysno = __NR_pkey_free;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST134) {
    int sysno = __NR_pkey_mprotect;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST135) {
    int sysno = __NR_pkey_alloc;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST136) {
    int sysno = __NR_execve;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST137) {
    int sysno = __NR_capset;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST138) {
    int sysno = __NR_setsid;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}
#endif

#if defined(__arm__)
TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST139) {
    int sysno = __NR_sigaction;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST140) {
    int sysno = __NR_futex_time64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST141) {
    int sysno = __NR_setresuid32;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST142) {
    int sysno = __NR_recv;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST143) {
    int sysno = __NR_getdents;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST144) {
    int sysno = __NR_unlink;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST145) {
    int sysno = __NR_sigprocmask;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST146) {
    int sysno = __NR_rt_sigtimedwait_time64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST147) {
    int sysno = __NR_ppoll_time64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST148) {
    int sysno = __NR_pselect6_time64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST149) {
    int sysno = __NR_fstatat64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}

TEST_F(BaselinePolicyOhosTest, EvaluateSyscall_TEST150) {
    int sysno = __NR_fstatfs64;
    EXPECT_EQ(GetPolicyKind(policy_->EvaluateSyscall(sysno)), PolicyKind::Unknown);
}
#endif

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}