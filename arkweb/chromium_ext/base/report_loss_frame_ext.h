// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_REPORT_LOSS_FRAME_H_
#define BASE_REPORT_LOSS_FRAME_H_

#include <string>
#include <vector>

enum ScrollMode { START, STOP };

enum JankRangeType : size_t {
  JANK_FREQ_6_FRAME = 0,
  JANK_FREQ_15_FRAME,
  JANK_FREQ_20_FRAME,
  JANK_FREQ_36_FRAME,
  JANK_FREQ_48_FRAME,
  JANK_FREQ_60_FRAME,
  JANK_FREQ_120_FRAME,
  JANK_FREQ_180_FRAME,
  JANK_FREQ_EXCEED_FRAME
};

// Frame loss reporting interval:
// 1. [1, 6) JANK_FREQ_6_FRAME
const int MAX_OF_FIRST_INTERVAL = 6;
// 2. [6, 15) JANK_FREQ_15_FRAME
const int MAX_OF_SECOND_INTERVAL = 15;
// 3. [15, 20）JANK_FREQ_20_FRAME
const int MAX_OF_THIRD_INTERVAL = 20;
// 4. [20, 36) JANK_FREQ_36_FRAME
const int MAX_OF_FOURTH_INTERVAL = 36;
// 5. [36, 48) JANK_FREQ_48_FRAME
const int MAX_OF_FIFTH_INTERVAL = 48;
// 6. [48, 60) JANK_FREQ_60_FRAME
const int MAX_OF_SIXTH_INTERVAL = 60;
// 7. [60, 120) JANK_FREQ_120_FRAME
const int MAX_OF_SEVENTH_INTERVAL = 120;
// 8. [120, 180）JANK_FREQ_180_FRAME
const int MAX_OF_EIGHTTH_INTERVAL = 180;

class ReportLossFrame {
 public:
  static ReportLossFrame* GetInstance();

  void Record();

  void Report();

  void Reset();

  void SetScrollState(ScrollMode state);

  void SetVsyncPeriod(int64_t vsync_period);

  int64_t GetCurrentTimestampMS();

 private:
  ReportLossFrame();
  ~ReportLossFrame();

  ReportLossFrame(const ReportLossFrame&) = delete;
  ReportLossFrame& operator=(const ReportLossFrame&) = delete;

  ScrollMode scroll_state_ = ScrollMode::STOP;

  int64_t start_time_for_scroll_ = 0;

  int64_t start_time_;

  int64_t vsync_period_;

  bool need_report_ = false;

  // the number 9 comes from type JankRangeType
  std::vector<uint16_t> jank_stats_ = std::vector<uint16_t>(9, 0);

  static ReportLossFrame* instance;
};

#endif  // BASE_REPORT_LOSS_FRAME_H_
