// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/tracing/common/hitrace_export_ohos.h"

#include <memory>
#include <string_view>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/synchronization/lock.h"
#include "base/trace_event/ohos_hitrace_interceptor.h"
#include "base/trace_event/trace_config_category_filter.h"
#include "base/trace_event/trace_log.h"
#include "perfetto/tracing/backend_type.h"
#include "third_party/perfetto/include/perfetto/tracing/tracing.h"
#include "third_party/perfetto/protos/perfetto/config/interceptor_config.gen.h"

namespace tracing {

namespace {
using base::trace_event::TraceConfigCategoryFilter;

/**
 * inner class for manage the hitrace session, this class is thread safe
 */
class HitraceSession : public perfetto::TrackEventSessionObserver {
 public:
  /** can be in two method to control tracing */
  enum class SessionMode : int32_t {
    /** follow the chrome trace start stop */
    kFollowTraceLog = 0,
    /** enable the long tracing mode */
    kLongTracing = 1,
  };
  /** the default categories that enabled if flag present but string absent */
  static constexpr const char kDefaultCategories[] =
      "benchmark,blink,browser,cc,compositor,drm,gpu,input,input.scrolling,"
      "latency,media,renderer.scheduler,sequence_manager,timeline.frame,"
      "toplevel,toplevel.flow,ui,v8,views,viz";

  /** get the instance */
  static HitraceSession& GetInstance();

  /** get the TrackEventConfig contains tracing categories by input */
  static perfetto::protos::gen::TrackEventConfig CategoriesStrToConfig(
      const TraceConfigCategoryFilter& category_filter);

  /** create the interceptor session config */
  static perfetto::TraceConfig CreateTraceConfig(
      const TraceConfigCategoryFilter& category_filter);

  HitraceSession(const HitraceSession&) = delete;
  ~HitraceSession() override = default;
  /** make registeration and register instance to TraceLog instance */
  void Setup();

  /** called when chrome tracing enabled */
  void OnStart(const perfetto::DataSourceBase::StartArgs&) override;
  /** called when chrome tracing disabled */
  void OnStop(const perfetto::DataSourceBase::StopArgs&) override;

 private:
  HitraceSession();
  /** create tracing session */
  bool EnableInternal(const base::AutoLock&,
                      const TraceConfigCategoryFilter& category_filter);
  /** stop and remove tracing session */
  bool DisableInternal(const base::AutoLock&);

  /** the lock ensure all invoke thread safe since invoke can call on any thread
   */
  base::Lock lock_;
  /**
   * default session mode is follow trace log unless extra switch present
   */
  SessionMode sessionMode_ = SessionMode::kFollowTraceLog;
  /**
   * trace session, note chrome now don't support side datasource so has
   * independent startup tracing session
   */
  std::unique_ptr<perfetto::StartupTracingSession> tracing_session_{nullptr};
};

HitraceSession& HitraceSession::GetInstance() {
  static HitraceSession singleton;
  return singleton;
}

perfetto::protos::gen::TrackEventConfig HitraceSession::CategoriesStrToConfig(
    const TraceConfigCategoryFilter& category_filter) {
  perfetto::protos::gen::TrackEventConfig track_event_config;
  track_event_config.clear_enabled_categories();
  // must add disable all as `*` to exclude other unspecified categories, see
  // `third_party/perfetto/protos/perfetto/config/track_event/track_event_config.proto`
  track_event_config.add_disabled_categories("*");
  for (const std::string& elem : category_filter.included_categories()) {
    track_event_config.add_enabled_categories(elem);
  }
  return track_event_config;
}

perfetto::TraceConfig HitraceSession::CreateTraceConfig(
    const TraceConfigCategoryFilter& category_filter) {
  perfetto::TraceConfig config;
  perfetto::protos::gen::TraceConfig_DataSource* data_source =
      config.add_data_sources();
  perfetto::DataSourceConfig* source_config = data_source->mutable_config();
  source_config->set_name("track_event");
  source_config->mutable_interceptor_config()->set_name(
      base::trace_event::ohos::OhosHiTraceInterceptor::kInterceptorName);
  perfetto::protos::gen::TrackEventConfig track_event_config =
      CategoriesStrToConfig(category_filter);
  source_config->set_track_event_config_raw(
      track_event_config.SerializeAsString());
  std::string category_str{category_filter.ToFilterString()};
  LOG(INFO) << "HitraceSession::CreateTraceConfig categories: " << category_str;
  return config;
}

HitraceSession::HitraceSession() = default;

void HitraceSession::Setup() {
  base::AutoLock lock_guard{lock_};
  const base::CommandLine* cmd = base::CommandLine::ForCurrentProcess();
  if (!cmd) {
    LOG(ERROR) << "HitraceSession::Enable: cmd is nullptr";
    return;
  }

  // if flag present, start the never adopted tracing session
  // this is a temporary solution until hitrace start / stop monitor is
  // provided, then the session is controlled by hitrace common event
  if (cmd->HasSwitch(switches::kOhosTrace)) {
    std::string switch_value = cmd->GetSwitchValueASCII(switches::kOhosTrace);
    TraceConfigCategoryFilter category_filter{};
    std::string_view category = switch_value.empty()
                                    ? kDefaultCategories
                                    : std::string_view{switch_value};
    category_filter.InitializeFromString(category);
    sessionMode_ = SessionMode::kLongTracing;
    EnableInternal(lock_guard, category_filter);
    return;
  } else {
    // else in kFollowTraceLog follow the chrome trace start / stop
    sessionMode_ = SessionMode::kFollowTraceLog;
    bool is_tracelog_enabled =
        base::trace_event::TraceLog::GetInstance()->IsEnabled();
    if (is_tracelog_enabled) {
      // if trace is already enabled, just enable it now then add observer to
      // prevent reentry deadlock
      base::trace_event::TraceConfig trace_config =
          base::trace_event::TraceLog::GetInstance()->GetCurrentTraceConfig();
      EnableInternal(lock_guard, trace_config.category_filter());
    }

    base::perfetto_track_event::TrackEvent::AddSessionObserver(this);
  }
}

void HitraceSession::OnStart(const perfetto::DataSourceBase::StartArgs&) {
  base::AutoLock lock_guard{lock_};
  if (sessionMode_ != SessionMode::kFollowTraceLog || tracing_session_) {
    return;
  }
  base::trace_event::TraceConfig trace_config =
      base::trace_event::TraceLog::GetInstance()->GetCurrentTraceConfig();
  EnableInternal(lock_guard, trace_config.category_filter());
}

void HitraceSession::OnStop(const perfetto::DataSourceBase::StopArgs&) {
  base::AutoLock lock_guard{lock_};
  if (sessionMode_ != SessionMode::kFollowTraceLog || !tracing_session_) {
    return;
  }
  DisableInternal(lock_guard);
}

bool HitraceSession::EnableInternal(
    const base::AutoLock&,
    const TraceConfigCategoryFilter& category_filter) {
  if (tracing_session_) {
    // already enabled
    return false;
  }

  perfetto::Tracing::SetupStartupTracingOpts opts;
  opts.timeout_ms = 0;
  opts.backend = perfetto::kCustomBackend;

  perfetto::TraceConfig trace_config = CreateTraceConfig(category_filter);
  tracing_session_ = perfetto::Tracing::SetupStartupTracing(trace_config, opts);
  if (!tracing_session_) {
    LOG(ERROR) << "HitraceSession::EnableInternal has nullptr in "
                  "perfetto::Tracing::SetupStartupTracing";
    return false;
  }
  LOG(INFO) << "HitraceSession Enabled: \"" << category_filter.ToFilterString()
            << '"';
  return true;
}

bool HitraceSession::DisableInternal(const base::AutoLock&) {
  if (!tracing_session_) {
    // already disabled
    return false;
  }
  tracing_session_->Abort();
  tracing_session_ = nullptr;
  LOG(INFO) << "HitraceSession Disabled";
  return true;
}
}  // namespace

void HitraceInterceptorController::EnableHitraceOnSetup() {
  base::trace_event::ohos::OhosHiTraceInterceptor::Register();
  HitraceSession::GetInstance().Setup();
}
}  // namespace tracing
