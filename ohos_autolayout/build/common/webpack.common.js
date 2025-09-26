const path = require("path");
const webpack = require("webpack");

module.exports = {
    resolve: {
        extensions: [".ts", ".js"],
    },

    module: {
        rules: [
            {
                test: /\.ts?$/,
                use: "ts-loader",
                exclude: /node-modules/,
            },
        ],
    },
    plugins: [
        new webpack.DefinePlugin({
            _ENABLE_VALUE_: process.env.enable || 1,
        }),
    ],
};
