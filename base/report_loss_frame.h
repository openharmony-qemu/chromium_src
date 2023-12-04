
#ifndef BASE_REPORT_LOSS_FRAME_H_
#define BASE_REPORT_LOSS_FRAME_H_

#include <vector>
#include <string>

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