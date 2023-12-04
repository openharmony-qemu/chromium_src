#ifndef BASE_MESSAGE_LOOP_MESSAGE_PUMP_OHOS_H_
#define BASE_MESSAGE_LOOP_MESSAGE_PUMP_OHOS_H_

#include <memory>

#include "base/base_export.h"
#include "base/compiler_specific.h"
#include "base/functional/callback.h"
#include "base/message_loop/message_pump.h"
#include "base/time/time.h"
#include "event_handler_adapter.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace base {

class RunLoop;

class BASE_EXPORT MessagePumpForUI : public MessagePump {
 public:
  MessagePumpForUI();

  MessagePumpForUI(const MessagePumpForUI&) = delete;
  MessagePumpForUI& operator=(const MessagePumpForUI&) = delete;

  ~MessagePumpForUI() override;

  void Run(Delegate* delegate) override;
  void Quit() override;
  void ScheduleWork() override;
  void ScheduleDelayedWork(
      const Delegate::NextWorkInfo& next_work_info) override;

  // Attaches |delegate| to this native MessagePump. |delegate| will from then
  // on be invoked by the native loop to process application tasks.
  void Attach(Delegate* delegate);

  void Abort() { should_abort_ = true; }
  bool IsAborted() { return should_abort_; }
  bool ShouldQuit() const { return should_abort_ || quit_; }

  // Tells the RunLoop to quit when idle, calling the callback when it's safe
  // for the Thread to stop.
  void QuitWhenIdle(base::OnceClosure callback);

  // These functions are only public so that the looper callbacks can call them,
  // and should not be called from outside this class.
  void OnDelayedLooperCallback();
  void OnNonDelayedLooperCallback();

 protected:
  Delegate* SetDelegate(Delegate* delegate);
  bool SetQuit(bool quit);
  void DoDelayedLooperWork();
  void DoNonDelayedLooperWork(bool do_idle_work);

 private:
  void ScheduleWorkInternal(bool do_idle_work);
  void DoIdleWork();

  std::unique_ptr<RunLoop> run_loop_;

  // See Abort().
  bool should_abort_ = false;

  // Whether this message pump is quitting, or has quit.
  bool quit_ = false;

  // The MessageLoop::Delegate for this pump.
  Delegate* delegate_ = nullptr;

  // The time at which we are currently scheduled to wake up and perform a
  // delayed task. This avoids redundantly scheduling |delayed_fd_| with the
  // same timeout when subsequent work phases all go idle on the same pending
  // delayed task; nullopt if no wakeup is currently scheduled.
  absl::optional<TimeTicks> delayed_scheduled_time_;

  // If set, a callback to fire when the message pump is quit.
  base::OnceClosure on_quit_callback_;

  // The file descriptor used to signal that non-delayed work is available.
  int non_delayed_fd_;

  // The file descriptor used to signal that delayed work is available.
  int delayed_fd_;

  std::unique_ptr<OHOS::NWeb::EventHandlerAdapter> ohos_event_handler_adapter_;
  std::shared_ptr<OHOS::NWeb::EventHandlerFDListenerAdapter> ohos_listener;
};

}  // namespace base

#endif  // BASE_MESSAGE_LOOP_MESSAGE_PUMP_OHOS_H_
