// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_API_SEARCH_SEARCH_API_H_
#define CHROME_BROWSER_EXTENSIONS_API_SEARCH_SEARCH_API_H_

#include "extensions/browser/extension_function.h"
#include "extensions/buildflags/buildflags.h"

static_assert(BUILDFLAG(ENABLE_EXTENSIONS_CORE));

namespace extensions {

class SearchQueryFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("search.query", SEARCH_QUERY)
  SearchQueryFunction() = default;

 private:
  ~SearchQueryFunction() override = default;
  ResponseAction Run() override;
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  static void OnQuery(const base::WeakPtr<SearchQueryFunction>& function,
                      const std::optional<std::string>& error);
  bool call_query_ = false;
  base::WeakPtrFactory<SearchQueryFunction> weak_ptr_factory_{this};
#endif
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_API_SEARCH_SEARCH_API_H_
