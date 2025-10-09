/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const { merge } = require('webpack-merge');
const common = require('../../common/webpack.common.js');
const path = require('path');
const TerserPlugin = require('terser-webpack-plugin');

const name = 'webview';
const library = 'AutoLayout';

// 判断是否是生产环境
const isProduction = process.env.NODE_ENV === 'production';

module.exports = merge(common, {
    // 2. 根据环境变量设置 mode
    mode: isProduction ? 'production' : 'development',
    entry: {
        [name]: './src/Main.ts',
    },
    output: {
        libraryTarget: 'umd',
        // 3. 为不同环境生成不同文件名，方便区分和使用
        filename: isProduction ? '[name]/autolayout.min.js' : '[name]/autolayout.js',
        path: path.resolve('./dist/sdk'),
        library,
    },
    // 4. 仅在生产环境下启用优化和混淆
    optimization: {
        minimize: isProduction,
        minimizer: [
            new TerserPlugin({
                terserOptions: {
                    // Terser 混淆选项
                    // 参考: https://terser.org/docs/api-reference#mangle-options
                    mangle: {
                        safari10: true, // 解决 Safari 10/11 的 "Cannot declare a let variable twice" bug
                    },
                    output: {
                        // 移除所有注释
                        comments: false,
                    },
                },
                extractComments: false, // 不将注释提取到单独的文件中
            }),
        ],
    },
    // 可选：为了方便调试，生产环境可以生成 source-map
    devtool: isProduction ? 'source-map' : 'eval-source-map',
});
