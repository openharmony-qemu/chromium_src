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
 
#ifndef COMPONENTS_MEMORY_PRESSURE_SYSTEM_MEMORY_PRESSURE_EVALUATOR_OHOS_H_
#define COMPONENTS_MEMORY_PRESSURE_SYSTEM_MEMORY_PRESSURE_EVALUATOR_OHOS_H_
 
#include "base/memory/memory_pressure_listener.h"
#include "base/process/process_metrics.h"
#include "base/sequence_checker.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "components/memory_pressure/memory_pressure_voter.h"
#include "components/memory_pressure/system_memory_pressure_evaluator.h"
 
namespace memory_pressure {
namespace ohos {
 
// Ohos memory pressure voter. Because there is no OS provided signal this
// polls at a low frequency, and applies internal hysteresis.
class SystemMemoryPressureEvaluator
    : public memory_pressure::SystemMemoryPressureEvaluator {
 public:
  using MemoryPressureLevel = base::MemoryPressureListener::MemoryPressureLevel;
 
  // The memory sampling period, currently 5s.
  static const base::TimeDelta kMemorySamplingPeriod;
 
  // Constants governing the polling and hysteresis behaviour of the observer.
  // The time which should pass between 2 successive moderate memory pressure
  // signals.
  static const base::TimeDelta kModeratePressureCooldown;
 
  // Default minimum free memory thresholds, in percents.
  static const int kDefaultModerateThresholdPc;
  static const int kDefaultCriticalThresholdPc;
 
  // Default constructor. Will choose thresholds automatically based on the
  // actual amount of system memory.
  explicit SystemMemoryPressureEvaluator(
      std::unique_ptr<MemoryPressureVoter> voter);
 
  // Constructor with explicit memory thresholds. These represent the amount of
  // free memory below which the applicable memory pressure state engages.
  SystemMemoryPressureEvaluator(int moderate_threshold_mb,
                                int critical_threshold_mb,
                                std::unique_ptr<MemoryPressureVoter> voter);
 
  ~SystemMemoryPressureEvaluator() override = default;
 
  SystemMemoryPressureEvaluator(const SystemMemoryPressureEvaluator&) = delete;
  SystemMemoryPressureEvaluator& operator=(
      const SystemMemoryPressureEvaluator&) = delete;
 
  // Returns the moderate pressure level free memory threshold, in MB.
  int moderate_threshold_mb() const { return moderate_threshold_mb_; }
 
  // Returns the critical pressure level free memory threshold, in MB.
  int critical_threshold_mb() const { return critical_threshold_mb_; }
 
 protected:
  // Internals are exposed for unittests.
 
  // Starts observing the memory fill level. Calls to StartObserving should
  // always be matched with calls to StopObserving.
  void StartObserving();
 
  // Stop observing the memory fill level. May be safely called if
  // StartObserving has not been called. Must be called from the same thread on
  // which the monitor was instantiated.
  void StopObserving();
 
  // Checks memory pressure, storing the current level, applying any hysteresis
  // and emitting memory pressure level change signals as necessary. This
  // function is called periodically while the monitor is observing memory
  // pressure. Must be called from the same thread on which the monitor was
  // instantiated.
  void CheckMemoryPressure();
 
  // Automatically infers threshold values based on system memory.
  // Returns 'true' if succeeded.
  bool InferThresholds();
 
  // Calculates the current instantaneous memory pressure level. This does not
  // use any hysteresis and simply returns the result at the current moment. Can
  // be called on any thread.
  MemoryPressureLevel CalculateCurrentPressureLevel();
 
  // This is just a wrapper for base:: function;
  // declared as virtual for unit testing
  virtual bool GetSystemMemoryInfo(base::SystemMemoryInfoKB* mem_info);
 
 private:
  // Threshold amounts of available memory that trigger pressure levels
  int moderate_threshold_mb_;
  int critical_threshold_mb_;
 
  // A periodic timer to check for memory pressure changes.
  base::RepeatingTimer timer_;
 
  // To slow down the amount of moderate pressure event calls, this gets used to
  // count the number of events since the last event occurred. This is used by
  // |CheckMemoryPressure| to apply hysteresis on the raw results of
  // |CalculateCurrentPressureLevel|.
  int moderate_pressure_repeat_count_;
 
  // Ensures that this object is used from a single sequence.
  SEQUENCE_CHECKER(sequence_checker_);
};
 
}  // namespace ohos
}  // namespace memory_pressure
 
#endif  // COMPONENTS_MEMORY_PRESSURE_SYSTEM_MEMORY_PRESSURE_EVALUATOR_OHOS_H_
