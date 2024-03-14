/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * Create by lin on 9/3/2024.
 */

#include "oh_web_performance_timing.h"

void OhWebPerformanceTiming::Reset() {
    navigation_id = -1;
    navigation_start = -1;
    redirect_count = 0;
    redirect_start = -1;
    redirect_end = -1;
    fetch_start = -1;
    worker_start = -1;
    domain_lookup_start = -1;
    domain_lookup_end = -1;
    connect_start = -1;
    secure_connect_start = -1;
    connect_end = -1;
    request_start = -1;
    response_start = -1;
    response_end = -1;
    dom_interactive = -1;
    dom_content_loaded_event_start = -1;
    dom_content_loaded_event_end = -1;
    load_event_start = -1;
    load_event_end = -1;
    first_paint = -1;
    first_contentful_paint = -1;
    largest_contentful_paint = -1;
}