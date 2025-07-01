#include "sandbox/linux/seccomp-bpf-helpers/baseline_policy_ohos.h"
#include "sandbox/linux/seccomp-bpf-helpers/syscall_parameters_restrictions.h"
#include <sys/socket.h>
#include "sandbox/linux/bpf_dsl/bpf_dsl.h"
#include "sandbox/linux/system_headers/linux_syscalls.h"
#include "sandbox/linux/seccomp-bpf-helpers/sigsys_handlers.h"
#include <linux/ashmem.h>
#include <linux/android/binder.h>
#include <signal.h>

struct access_token {
    __u64 sender_tokenid;
    __u64 first_tokenid;
    __u64 reserved[2];
};
struct binder_sender_info {
    struct access_token tokens;
    __u64 sender_pid_nr;
};
#define BINDER_ENABLE_ONEWAY_SPAM_DETECTION _IOW('b', 16, __u32)
#define BINDER_FEATURE_SET _IOWR('b', 30, __u64)
#define BINDER_GET_SENDER_INFO  _IOWR('b', 32, struct binder_sender_info)

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

#if !defined(__i386__) || defined(__x86_64__)
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
#endif  // !defined(__i386__)

}  // namespace

BaselinePolicyOhos::BaselinePolicyOhos()
    : BaselinePolicy() {}

BaselinePolicyOhos::~BaselinePolicyOhos() {}

ResultExpr BaselinePolicyOhos::EvaluateSyscall(int sysno) const {
    bool override_and_allow = false;
    bool override_and_trap = false;

    switch (sysno) {
#if defined(__arm__) || defined(__aarch64__)
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
    case __NR_openat:
    case __NR_connect:
    case __NR_readlinkat:
    case __NR_mkdirat:
    case __NR_set_tid_address:
    case __NR_getdents64:
    case __NR_prlimit64:
    case __NR_sched_setscheduler:
    case __NR_sched_getscheduler:
    case __NR_membarrier:
    case __NR_setitimer:
    case __NR_msync:
    case __NR_set_robust_list:
    case __NR_sched_getparam:
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
    case __NR_access:
    case __NR_statx:
#endif
#if defined(__aarch64__)
    case __NR_getrlimit:
    case __NR_newfstatat:
    case __NR_fstatfs:
#endif

    override_and_allow = true;
    break;
    }

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

    if (sysno == __NR_ptrace) {
        return RestrictPtrace();
    }

    if (sysno == __NR_madvise) {
        const Arg<int> advice(2);
        const unsigned int MADV_WIPEONFORK = 18;
        return If(AnyOf(advice == -1,
                        advice == MADV_WIPEONFORK),
                Allow())
            .Else(BaselinePolicy::EvaluateSyscall(sysno));
    }

    if (sysno == __NR_ioctl) {

struct QosCtrlData {
    int pid;
    unsigned int type;
    unsigned int level;
    int qos;
    int staticQos;
    int dynamicQos;
    bool tagSchedEnable = false;
};
constexpr unsigned int QOS_CTRL_IPC_MAGIC = 0xCC;
#define QOS_CTRL_BASIC_OPERATION _IOWR(QOS_CTRL_IPC_MAGIC, 1, struct QosCtrlData)

        const Arg<unsigned int> request(1);
        #ifdef BINDER_IPC_32BIT
        const unsigned int kBinderWriteRead32 = BINDER_WRITE_READ;
        const unsigned int kBinderWriteRead64 =
            (BINDER_WRITE_READ & ~IOCSIZE_MASK) |
            ((sizeof(binder_write_read) * 2) << _IOC_SIZESHIFT);
        #else
        const unsigned int kBinderWriteRead64 = BINDER_WRITE_READ;
        const unsigned int kBinderWriteRead32 =
            (BINDER_WRITE_READ & ~IOCSIZE_MASK) |
            ((sizeof(binder_write_read) / 2) << _IOC_SIZESHIFT);
        #endif
        return Switch(request)
            .Cases({
                        ASHMEM_SET_NAME, ASHMEM_GET_NAME, ASHMEM_SET_SIZE,
                        ASHMEM_GET_SIZE, ASHMEM_SET_PROT_MASK, ASHMEM_GET_PROT_MASK,
                        ASHMEM_PIN, ASHMEM_UNPIN, ASHMEM_GET_PIN_STATUS,
                        kBinderWriteRead32, kBinderWriteRead64, BINDER_SET_MAX_THREADS,
                        BINDER_THREAD_EXIT, BINDER_VERSION, BINDER_ENABLE_ONEWAY_SPAM_DETECTION,
                        BINDER_FEATURE_SET, BINDER_GET_SENDER_INFO},
                    Allow())
            .Cases({QOS_CTRL_BASIC_OPERATION, TIOCGWINSZ},
                    Error(EPERM))
            .Default(RestrictIoctl());
    }

    if (sysno == __NR_clone) {
        const Arg<unsigned long> flags(0);
 
        const uint64_t kMuslForkFlags = SIGCHLD;
        const uint64_t kPthreadCreateFlags =
            CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD |
            CLONE_SYSVSEM | CLONE_SETTLS | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID | CLONE_DETACHED;
 
        const BoolExpr is_fork_or_pthread =
            AnyOf(flags==kMuslForkFlags, flags == kPthreadCreateFlags);
        return If(is_fork_or_pthread, Allow()).Else(CrashSIGSYSClone());
    }

    if (sysno == __NR_prctl) {
#define PR_SET_JITFORT_OPTION 0x6a6974
#define JITFORT_CPU_FEATURES 7
#define HM_PR_SILK_BLOCKAWARE_OPS 0x534b4241
#define BLOCKAWARE_SUBOPS_INIT 1
#define BLOCKAWARE_SUBOPS_REG 2
#define BLOCKAWARE_SUBOPS_UNREG 3
#define BLOCKAWARE_SUBOPS_MONITORFD 6
        const Arg<int> option(0), arg(1);

        return Switch(option)
            .Cases({PR_SET_JITFORT_OPTION},
                If(arg == JITFORT_CPU_FEATURES, Allow()).Else(CrashSIGSYSPrctl()))
            .Cases({HM_PR_SILK_BLOCKAWARE_OPS},
                If(AnyOf(arg == BLOCKAWARE_SUBOPS_INIT, arg == BLOCKAWARE_SUBOPS_REG,
                    arg == BLOCKAWARE_SUBOPS_UNREG, arg == BLOCKAWARE_SUBOPS_MONITORFD),
                    Error(EPERM)).Else(CrashSIGSYSPrctl()))
            .Default(BaselinePolicy::EvaluateSyscall(sysno));
    }

    if (sysno == __NR_timerfd_create) {
#define TFD_CLOEXEC  02000000
#define TFD_NONBLOCK 00004000
        const Arg<int> clockid(0), flags(1);

        return Switch(clockid)
            .Cases({CLOCK_MONOTONIC},
                If(flags == (TFD_CLOEXEC | TFD_NONBLOCK), Allow()).Else(CrashSIGSYS()))
            .Default(BaselinePolicy::EvaluateSyscall(sysno));
    }

    if (sysno == __NR_timerfd_settime) {
#define TFD_TIMER_ABSTIME (1 << 0)
        const Arg<int> option(1);

        return Switch(option)
            .Cases({TFD_TIMER_ABSTIME, 0},
                Error(EPERM))
            .Default(BaselinePolicy::EvaluateSyscall(sysno));
    }

    if (sysno == __NR_getsockopt) {
        const Arg<int> level(1), optname(2);

        return Switch(level)
            .Cases({SOL_SOCKET},
                If(optname == SO_SNDBUF, Error(EPERM)).Else(CrashSIGSYSSockopt()))
            .Default(BaselinePolicy::EvaluateSyscall(sysno));
    }
#endif

    switch(sysno) {
#if defined(__arm__) || defined(__aarch64__)
    case __NR_setrlimit:
    case __NR_sched_get_priority_max:
    case __NR_sched_get_priority_min:
    case __NR_times:
    case __NR_get_robust_list:
    case __NR_setresuid:
    case __NR_unlinkat:
    case __NR_flock:
    case __NR_sched_setaffinity:
    case __NR_getrusage:
    case __NR_process_vm_readv:
    case __NR_pkey_free:
    case __NR_pkey_mprotect:
    case __NR_pkey_alloc:
    case __NR_execve:
    case __NR_capset:
    case __NR_setsid:
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
    }

    if (override_and_trap)
        return CrashSIGSYS();

    if (override_and_allow)
        return Allow();

    return BaselinePolicy::EvaluateSyscall(sysno);
}


} // namespace sandbox