/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
 
#ifndef V8_RESTRACE_H_
#define V8_RESTRACE_H_
 
#if (defined OH_ENABLE_RESTRACE) && (defined USING_OHOS_WEB)
 
#define RES_V8_JS_HEAP_MASK (1 << 18)
#define RES_V8_JS_HEAP_TAG ("RES_JS_HEAP_MASK")
 
extern "C" void restrace(unsigned long long mask,
                         void* addr,
                         size_t size,
                         const char* tag,
                         bool is_using) __attribute__((weak));
 
extern "C" void resTraceMove(unsigned long long mask,
                             void* oldAddr,
                             void* newAddr,
                             size_t newSize) __attribute__((weak));
 
extern "C" void resTraceFreeRegion(unsigned long long mask,
                                   void* addr,
                                   size_t size) __attribute__((weak));
 
#define OH_RESTRACE(ADDR, SIZE) \
  restrace(RES_V8_JS_HEAP_MASK, ADDR, SIZE, RES_V8_JS_HEAP_TAG, true)
 
#define OH_RESTRACE_MOVE(OLD_ADDR, NEW_ADDR, NEW_SIZE) \
  resTraceMove(RES_V8_JS_HEAP_MASK, OLD_ADDR, NEW_ADDR, NEW_SIZE)
 
#define OH_RESTRACE_FREE_REGION(ADDR, SIZE) \
  resTraceFreeRegion(RES_V8_JS_HEAP_MASK, ADDR, SIZE)
 
#else
 
#define OH_RESTRACE(ADDR, SIZE)
#define OH_RESTRACE_MOVE(OLD_ADDR, NEW_ADDR, NEW_SIZE)
#define OH_RESTRACE_FREE_REGION(ADDR, SIZE)
 
#endif  // (defined OH_ENABLE_RESTRACE) && (defined USING_OHOS_WEB)
 
#endif  // V8_RESTRACE_H_