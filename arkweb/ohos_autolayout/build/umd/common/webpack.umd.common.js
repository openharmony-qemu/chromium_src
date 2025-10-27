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
const webpack = require('webpack');
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
    plugins: [
        // 定义全局变量 __DEV__，用于日志系统
        new webpack.DefinePlugin({
            __DEV__: JSON.stringify(!isProduction),
        }),
    ],
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
                    // 压缩选项
                    compress: {
                        // 删除未使用的代码
                        dead_code: true,
                        // 删除 console.log (保留 console.warn 和 console.error)
                        drop_console: false, // 不删除所有 console，我们通过 pure_funcs 精确控制
                        // 删除 debugger 语句
                        drop_debugger: true,
                        // 优化 if 语句中的条件
                        conditionals: true,
                        // 评估常量表达式
                        evaluate: true,
                        // 删除不可达代码
                        unused: true,
                        // 删除仅在开发环境使用的代码（通过 __DEV__ 标记）
                        global_defs: {
                            __DEV__: false,
                        },
                        // 指定纯函数，可以安全删除（如果未使用返回值）
                        pure_funcs: [
                            // 删除这些 console 方法的调用
                            'console.log',
                            'console.info',
                            'console.debug',
                            'Log.d',
                            'Log.i',
                            'Log.info',
                            'Log.debug',
                            // 不删除: console.warn, console.error
                        ],
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
