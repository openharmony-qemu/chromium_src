/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * Create by lin on 9/3/2024.
 */

#ifndef OH_WEB_PERFORMANCE_TIMING_H_
#define OH_WEB_PERFORMANCE_TIMING_H_

#include <stdint.h>

struct OhWebPerformanceTiming {
    OhWebPerformanceTiming() = default;
    void Reset();

    int64_t navigation_id = -1;
    int64_t navigation_start = -1;
    uint32_t redirect_count = 0;
    int64_t redirect_start = -1;
    int64_t redirect_end = -1;
    int64_t fetch_start = -1;
    int64_t worker_start = -1;
    int64_t domain_lookup_start = -1;
    int64_t domain_lookup_end = -1;
    int64_t connect_start = -1;
    int64_t secure_connect_start = -1;
    int64_t connect_end = -1;
    int64_t request_start = -1;
    int64_t response_start = -1;
    int64_t response_end = -1;
    int64_t dom_interactive = -1;
    int64_t dom_content_loaded_event_start = -1;
    int64_t dom_content_loaded_event_end = -1;
    int64_t load_event_start = -1;
    int64_t load_event_end = -1;
    int64_t first_paint = -1;
    int64_t first_contentful_paint = -1;
    int64_t largest_contentful_paint = -1;
};

#endif  // OH_WEB_PERFORMANCE_TIMING_H_