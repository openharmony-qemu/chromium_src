/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

// Applies DomDistillerJs to the content of the page and returns a
// DomDistillerResults (as a javascript object/dict).
(function(options, stringify_output, extract_catalog_info) {
try {
  function initialize() {
  $$DOMDISTILLERJS
  }
  window.setTimeout = function() {};
  window.clearTimeout = function() {};
  if (!window.org?.chromium?.distiller?.DomDistiller) {
  initialize();
  }

  // The OPTIONS placeholder will be replaced with the DomDistillerOptions at
  // runtime.
  const distiller = window.org.chromium.distiller.DomDistiller;
  res = null;
  if (extract_catalog_info) {
  res = distiller.applyCatalogWithOptions(options);
  } else {
  res = distiller.applyWithOptions(options);
  }

  if (stringify_output) {
  return JSON.stringify(res);
  }
  return res;
} catch (e) {
  window.console.error('Error during distillation: ' + e);
  if (e.stack !== undefined) {
  window.console.error(e.stack);
  }
}
return undefined;
})($$OPTIONS, $$STRINGIFY, $$CATALOGINFO);