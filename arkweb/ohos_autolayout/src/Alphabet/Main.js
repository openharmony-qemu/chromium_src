/*
  Copyright (c) 2025 Huawei Device Co., Ltd.
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

const AlphabetAdapter = require('./AlphabetAdapter');
const Utils = require('./Utils');

const start = (function() {
    let alphabetAdapter;
    return function(config) {
        if (alphabetAdapter) {
            alphabetAdapter.setConfig(config);
            alphabetAdapter.reset();
            Utils.log('Reset AlphabetAdapter');
        } else {
            alphabetAdapter = new AlphabetAdapter(config);
            Utils.log('New AlphabetAdapter');
        }
    };
})();

module.exports = { start };