// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef BASE_TUPLE_ext_H_
#define BASE_TUPLE_ext_H_

#include <stddef.h>

#include <tuple>
#include <utility>

#include "arkweb/build/features/features.h"
#include "build/build_config.h"

namespace base {

#if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
template <typename ObjT,
          typename Method,
          typename InTuple,
          typename OutTuple,
          typename P,
          size_t... InNs,
          size_t... OutNs>
inline void DispatchToMethodImpl_Param(const ObjT& obj,
                                       Method method,
                                       InTuple&& in,
                                       OutTuple* out,
                                       P* parameter,
                                       std::index_sequence<InNs...>,
                                       std::index_sequence<OutNs...>)
{
    (obj->*method)(parameter, std::get<InNs>(std::forward<InTuple>(in))...,
                &std::get<OutNs>(*out)...);
}

template <typename ObjT,
          typename Method,
          typename InTuple,
          typename P,
          typename OutTuple>
inline void DispatchToMethod_Param(const ObjT& obj,
                                   Method method,
                                   InTuple&& in,
                                   P* parameter,
                                   OutTuple* out)
{
    constexpr size_t in_size = std::tuple_size<std::decay_t<InTuple>>::value;
    constexpr size_t out_size = std::tuple_size<OutTuple>::value;
    DispatchToMethodImpl_Param(obj, method, std::forward<InTuple>(in), out,
                               parameter, std::make_index_sequence<in_size>(),
                               std::make_index_sequence<out_size>());
}
#endif

} // namespace base
#endif  // BASE_TUPLE_EXT_H_