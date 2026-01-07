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

module.exports = {
    BASE: {
        MAX_STRING_LENGTH_ON_CHECK_ALPHABET: 1500,
        HEIGHT_WIDTH_MIN_RATIO_DEFAULT: 10,
        HEIGHT_WIDTH_MIN_RATIO_CONFIG_MAX: 30,
        IDENTIFICATION_MIN_SIZE: 18,
        IDENTIFICATION_MIN_SIZE_CONFIG_MAX: 26,
    },
    BUTTON_GROUP: {
        TYPE: 'buttonGroup',
        SIDE_DISTANCE: {
            RIGHT_DEFAULT: 15,
            RIGHT_PLUS: 0,
            BOTTOM_DEFAULT: 15,
            BOTTOM_PLUS: 5,
            TOP_DEFAULT: 15,
            TOP_PLUS: 5,
        },
        BUTTON_SIZE: {
            WIDTH: 24,
            HEIGHT: 24,
        },
        BUTTON_CONTENT_SIZE: 18,
        BUTTON_PADDING: 3,
        BUTTON_TEXT: {
            UP: '󰃌',
            DOWN: '󰢝',
        },
        BUTTON_SWITCH_TYPES: {
            UP: 'up',
            DOWN: 'down',
        },
        OPACITY_VALUES: {
            SHOW: '1.0',
            HIDE: '0',
        },
        BUTTON_DISPLAY_TYPES: {
            SHOW: 'block',
            HIDE: 'none',
        },
        BOX_DISPLAY_TYPES: {
            SHOW: 'block',
            HIDE: 'none',
        },
        LAST_PAGE_BOTTOM_DISTANCE: 20,
        ALPHABET_TRANSITION_STYLE: 'transform 0.5s ease',
        SLIDE_TRANSITION_MICRO_SECONDS: 500,
        HM_SYMBOL_FONT_FAMILY_NAME: 'HM Symbol',
    },
    BUTTON_BOX: {
        TYPE: 'buttonBox',
        SIDE_DISTANCE: {
            RIGHT_DEFAULT: 15,
            RIGHT_PLUS: 0,
            BOTTOM_DEFAULT: 15,
            BOTTOM_PLUS: 5,
        },
        BUTTON_TEXT: {
            UP: '↑',
            DOWN: '↓',
        },
        BUTTON_SWITCH_TYPES: {
            UP: 'up',
            DOWN: 'down',
        },
        LAST_PAGE_BOTTOM_DISTANCE: 20,
        ALPHABET_TRANSITION_STYLE: 'transform 0.5s ease',
    },
    MICRO_DOTS: {
        TYPE: 'microDots',
        MICRO_DISPLAY_TEXT: '.',
        DISTRIBUTE_WEIGHT_TYPES: ['start', 'center', 'end'],
        DISTRIBUTE_WEIGHT_TYPE_DEFAULT: 'center',
        TEXT_NODE_NAME: '#text',
        COMMENT_NODE_NAME: '#comment',
    },
};