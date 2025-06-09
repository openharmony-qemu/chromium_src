/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sandbox/linux/seccomp-bpf-helpers/baseline_policy_ohos.h"

#include <sys/socket.h>

#include "sandbox/linux/bpf_dsl/bpf_dsl_impl.h"
#include "sandbox/linux/bpf_dsl/bpf_dsl.h"
#include "sandbox/linux/seccomp-bpf-helpers/syscall_parameters_restrictions.h"
#include "sandbox/linux/seccomp-bpf-helpers/sigsys_handlers.h"
#include "sandbox/linux/system_headers/linux_syscalls.h"
#include "sandbox/sandbox_export.h"

using sandbox::bpf_dsl::AllOf;
using sandbox::bpf_dsl::Allow;
using sandbox::bpf_dsl::AnyOf;
using sandbox::bpf_dsl::Arg;
using sandbox::bpf_dsl::BoolExpr;
using sandbox::bpf_dsl::If;
using sandbox::bpf_dsl::Error;
using sandbox::bpf_dsl::ResultExpr;

namespace sandbox {

#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC O_CLOEXEC
#endif

#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK O_NONBLOCK
#endif

#define CASES SANDBOX_BPF_DSL_CASES

namespace {

#if defined(__arm__) || defined(__aarch64__)
// Restricts the arguments to sys_socket() to AF_UNIX. Returns a BoolExpr that
// evaluates to true if the syscall should be allowed.
BoolExpr RestrictSocketArguments(const Arg<int>& domain,
                                 const Arg<int>& type,
                                 const Arg<int>& protocol) {
  const int kSockFlags = SOCK_CLOEXEC | SOCK_NONBLOCK;
  return AllOf(domain == AF_UNIX,
               AnyOf((type & ~kSockFlags) == SOCK_DGRAM,
                     (type & ~kSockFlags) == SOCK_STREAM),
               protocol == 0);
}
#endif  // defined(__arm__) || defined(__aarch64__)

}  // namespace

BaselinePolicyOhos::BaselinePolicyOhos(bool allow_sched_affinity)
    : BaselinePolicy(allow_sched_affinity) {}

BaselinePolicyOhos::BaselinePolicyOhos()
  : BaselinePolicy() {}

BaselinePolicyOhos::~BaselinePolicyOhos() = default;

ResultExpr BaselinePolicyOhos::EvaluateSyscall(int sysno) const {
#if DCHECK_IS_ON()
// debug mode, collect stacktrace call _Unwind_Backtrace
// _Unwind_Backtrace call syscall __NR_process_vm_readv(270)
  if (sysno == __NR_process_vm_readv) {
    return Allow();
  }
#endif

  bool override_and_allow = false;
  bool override_and_trap = false;
  switch (sysno) {
#if defined(__arm__) || defined(__aarch64__)
    case __NR_setsockopt:
    case __NR_socket:
    case __NR_bind:
    case __NR_inotify_add_watch:
    case __NR_inotify_rm_watch:
    case __NR_inotify_init1:
    case __NR_getsockopt:
    case __NR_getsockname:
    case __NR_fdatasync:
    case __NR_fsync:
    case __NR_ftruncate:
    case __NR_mremap:
    case __NR_pwrite64:
    case __NR_getpriority:
    case __NR_setpriority:
    case __NR_sysinfo:
    case __NR_uname:
    case __NR_sigaltstack:
    case __NR_brk:
    case __NR_mlock:
    case __NR_munlock:
    case __NR_munmap:
    case __NR_sched_yield:
    case __NR_nanosleep:
    case __NR_epoll_pwait:
    case __NR_epoll_create1:
    case __NR_epoll_ctl:
    case __NR_lseek:
    case __NR_eventfd2:
    case __NR_fstat:
    case __NR_ppoll:
    case __NR_pselect6:
    case __NR_read:
    case __NR_readv:
    case __NR_pread64:
    case __NR_recvfrom:
    case __NR_recvmsg:
    case __NR_sendto:
    case __NR_write:
    case __NR_writev:
    case __NR_pipe2:
    case __NR_gettimeofday:
    case __NR_exit:
    case __NR_exit_group:
    case __NR_wait4:
    case __NR_waitid:
    case __NR_rt_sigaction:
    case __NR_rt_sigprocmask:
    case __NR_rt_sigreturn:
    case __NR_rt_sigtimedwait:
    case __NR_capget:
    case __NR_getegid:
    case __NR_geteuid:
    case __NR_getgid:
    case __NR_getgroups:
    case __NR_getpid:
    case __NR_getppid:
    case __NR_getresgid:
    case __NR_getsid:
    case __NR_gettid:
    case __NR_getuid:
    case __NR_getresuid:
    case __NR_restart_syscall:
    case __NR_close:
    case __NR_dup:
    case __NR_dup3:
    case __NR_shutdown:
    case __NR_mincore:
    case __NR_memfd_create:
    case __NR_faccessat:
    case __NR_prctl:
    case __NR_fcntl:
    case __NR_clone:
    case __NR_capset:
    case __NR_openat:
    case __NR_connect:
    case __NR_readlinkat:
    case __NR_ioctl:
    case __NR_mkdirat:
    case __NR_set_tid_address:
    case __NR_getdents64:
    case __NR_madvise:
    case __NR_getrandom:
    case __NR_prlimit64:
    case __NR_sched_setscheduler:
    case __NR_sched_getparam:
    case __NR_sched_getscheduler:
    case __NR_setsid:
    case __NR_ptrace:
    case __NR_membarrier:
    case __NR_setitimer:
    case __NR_execve:
    case __NR_msync:
    case __NR_statx:
#endif
#if defined(__arm__)
    case __NR_sched_getaffinity:
    case __NR_ftruncate64:
    case __NR_ugetrlimit:
    case __NR_pause:
    case __NR_epoll_create:
    case __NR_epoll_wait:
    case __NR_eventfd:
    case __NR_fstat64:
    case __NR__llseek:
    case __NR_poll:
    case __NR__newselect:
    case __NR_send:
    case __NR_pipe:
    case __NR_getegid32:
    case __NR_geteuid32:
    case __NR_getgid32:
    case __NR_getgroups32:
    case __NR_getresgid32:
    case __NR_getuid32:
    case __NR_getresuid32:
    case __NR_dup2:
    case __NR_open:
    case __NR_readlink:
    case __ARM_NR_cacheflush:
    case __ARM_NR_set_tls:
    case __NR_mkdir:
    case __NR_sigreturn:
    case __NR_fork:
    case __NR_fcntl64:
    case __NR_access:
#endif
#if defined(__aarch64__)
    case __NR_getrlimit:
    case __NR_newfstatat:
    case __NR_fstatfs:
    case __NR_mmap:
#endif
      override_and_allow = true;
      break;
    default:
      break;
  }

  if (override_and_allow)
    return Allow();

#if defined(__arm__) || defined(__aarch64__)
  if (sysno == __NR_socket) {
    const Arg<int> domain(0);
    const Arg<int> type(1);
    const Arg<int> protocol(2);
    return If(RestrictSocketArguments(domain, type, protocol), Allow())
            .Else(Error(EPERM));
  }

  if (sysno == __NR_setsockopt) {
    const Arg<int> level(1);
    const Arg<int> option(2);
    return If(AllOf(level == SOL_SOCKET,
                    AnyOf(option == SO_SNDTIMEO,
                          option == SO_RCVTIMEO,
                          option == SO_SNDBUF,
                          option == SO_REUSEADDR,
                          option == SO_PASSCRED)),
              Allow())
            .Else(BaselinePolicy::EvaluateSyscall(sysno));
  }

  if (sysno == __NR_clock_getres) {
    return RestrictClockID();
  }
#endif

  switch (sysno) {
#if defined(__arm__) || defined(__aarch64__)
    case __NR_setrlimit:
    case __NR_sched_get_priority_max:
    case __NR_sched_get_priority_min:
    case __NR_times:
    case __NR_get_robust_list:
    case __NR_set_robust_list:
    case __NR_setresuid:
    case __NR_flock:
    case __NR_sched_setaffinity:
    case __NR_getrusage:
    case __NR_getsockopt:
    case __NR_pkey_free:
    case __NR_pkey_mprotect:
    case __NR_pkey_alloc:
    case __NR_seteuid:
    case __NR_setegid:
    case __NR_klogctl:
#endif
#if defined(__arm__)
    case __NR_sigaction:
    case __NR_futex_time64:
    case __NR_setresuid32:
    case __NR_recv:
    case __NR_getdents:
    case __NR_unlink:
    case __NR_sigprocmask:
    case __NR_rt_sigtimedwait_time64:
    case __NR_ppoll_time64:
    case __NR_pselect6_time64:
    case __NR_fstatat64:
    case __NR_fstatfs64:
#endif
      override_and_trap = true;
      break;
    default:
      break;
  }

  if (override_and_trap)
    return CrashSIGSYS();

  return BaselinePolicy::EvaluateSyscall(sysno);
}
} // namespace sandbox
