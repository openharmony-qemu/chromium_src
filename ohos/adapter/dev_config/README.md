# Chromium Project: Customizing Browser/Electron/CEF Functionality with dev_config.json

## Introduction to dev_config.json File

The dev_config.json file is a configuration file designed to address the limitation on HarmonyOS platform where startup parameters cannot be used like on other PC platforms. For reference on available command-line switches, see the documentation at:
[https://peter.sh/experiments/chromium-command-line-switches/](https://peter.sh/experiments/chromium-command-line-switches/)

For example, on Windows platform or Mac/Linux systems, you can use the following command to capture trace data for 15 seconds at startup:

```bash
chrome --trace-startup=benchmark,blink,v8,cc,gpu,navigation,toplevel,viz,ui,views --trace-startup-duration=15
```

The two configuration options `trace-startup` and `trace-startup-duration` are processed through the `base::CommandLine` class defined in the file `base/command_line.h`, which serves as the core mechanism for Chromium to read configuration parameters.

These two configuration options are accessed in the `TraceStartupConfig::TraceStartupConfig` constructor of the `services/tracing/public/cpp/trace_startup_config.cc` file. It retrieves the `base::CommandLine` instance and reads the configuration to set member properties of the `tracing::TraceStartupConfig` object.

Since the HarmonyOS platform does not support command-line arguments, custom operations require an additional configuration file that converts these settings into startup parameters during application launch.

The current implementation mechanism uses the `dev_config.json` file, with the specific logic implemented in `ohos/adapter/dev_config/dev_config.h`.

A valid `dev_config.json` file looks like this:

```json
{
  "trace-startup" : true,
  "trace-categories":
    "benchmark,blink,v8,cc,gpu,navigation,toplevel,viz,ui,views",
  "trace-startup-file":
    "/data/storage/el2/base/cache/trace.json",
  "trace-startup-duration": 15,
  "trace-startup-record-mode": "record-until-full",
  "long-tracing": true,
  "remote-debugging": true,
  "remote-debugging-port": 9222,
  "remote-allow-origins": "http://localhost:9222",
  "disable-occlusion-feature": false,
  "disable-partial-swap": false,
  "disable-page-policy": false
}
```

All configuration items are optional, and if not provided, the default values of the options introduced below will be used.

The startup command above uses the `dev_config.json` configuration file as shown below:

Original command:

```bash
chrome --trace-startup=benchmark,blink,v8,cc,gpu,navigation,toplevel,viz,ui,views --trace-startup-duration=15
```

Corresponding JSON file:

```json
{
  "trace-startup" : true,
  "trace-categories":
    "benchmark,blink,v8,cc,gpu,navigation,toplevel,viz,ui,views",
  "trace-startup-duration": 15,
}
```

### trace related

Configure options related to chrome trace.

#### trace-startup

Default value: `false`

Equivalent to the command line parameter `--trace-startup`. If provided, it will enable the startup trace recording feature.

#### trace-categories

Default value:
`benchmark,blink,blink_gc,v8,cc,gpu,navigation,toplevel,viz,ui,views,`
`disk_cache,latency,renderer.scheduler,sequence_manager,timeline.frame,`
`disabled-by-default-v8.gc,disabled-by-default-blink_gc`

Set the categories of traces to be enabled during startup trace and the `long-trace` mode.

For all specific trace categories, refer to the file `base/trace_event/builtin_categories.h`.

To capture all non `disabled-by-default-` categories, you can use `*`.

#### trace-startup-file

Default value: chrometrace.log

The name of the Perfetto trace file saved by Chromium

The file with this name will be saved in the application's temporary folder (i.e., `/data/app/el2/${user_id_usually_100}/base/${package_name}/temp`)

This file can be accessed via this website: [https://ui.perfetto.dev/](https://ui.perfetto.dev/)

#### trace-startup-duration

Default value: 5

Sets the duration for recording the startup trace, similar to `--trace-startup-duration`.

It specifies how many seconds after startup to stop recording the trace and generate the trace file.

If not provided, the default is 5 seconds.

#### trace-startup-record-mode

Default value: record-until-full

The recording mode for the startup trace, with 3 options:
- `"record-until-full"`: Ends recording when full
- `"record-continuously"`: Discards previous content when full
- `"record-as-much-as-possible"`: Uses a very large cache to record, ending when full

#### long-tracing

Default value: false

If false, `hitrace` trace output is only available when Chromium tracing is enabled.

If true, enabling `hitrace` recording will include Chromium's HarmonyOS adaptation for synchronous and asynchronous traces in `hitrace`.

This means a persistent bypass will be opened to connect and write to `hitrace`.

Used for adapting to the performance factory and other automated performance tests.

### Remote Debug Port Related Configuration

These configurations allow the application to export a debug port to support remote debugging.

For remote debugging, see Google's documentation:

[devtools/remote-debugging/local-server](https://developer.chrome.com/docs/devtools/remote-debugging/local-server)

#### remote-debugging

Default value: false

Whether to enable the remote debug port. If false, the following configuration is ignored:

#### remote-allow-origins

Default value: empty

If `remote-debugging` is `true`, this must be specified.
Otherwise, all remote debug port connections will be rejected. See below:

You can use `*` to allow connections from all origins, **but it usually expose a security risk**.

#### remote-debugging-port

Default value: 9222

The currently open port, default is 9222, and needs to be used in conjunction with `remote-allow-origins`.

If it is 9222, then `remote-allow-origins` needs to be `"http://localhost:9222"`.

##### How to Enable Remote Connection on the Development Machine

On a development machine that can connect to the HarmonyOS PC via a USB cable and use the `hdc` command, run the following command:

```bash
hdc fport tcp:9222 tcp:9222
```

This will route the local development machine's port 9222 to the HarmonyOS PC's port 9222.

At this point, open Chrome on the development machine and enter the following URL:

```txt
chrome://inspect/#devices
```

After a short time of wait, you should be able to see the remote machine's tab listed under `Remote Target`.

### Feature Switches

Whether to switch the corresponding feature, set the configuration item to true to disable some feature

#### disable-occlusion-feature

Default value: false

Set to true to disable the occlusion stop vsync capability of HarmonyOS XComponent,
avoiding the white screen issue in cef/electron

#### disable-partial-swap

Default value: false

Set to true to disable the partial refresh capability of gl, always performing a full screen refresh

#### disable-page-policy

Default value: false

Temporarily noop and not a effective switch

## Include dev_config.json by packaging

When packaging the application, you can include the configuration file in the following path,

which is the same path as libadapter.so

- chromium:
  `chromium/libs/arm64-v8a/dev_config.json`
- electron:
  `electron/libs/arm64-v8a/dev_config.json`
- cef:
  `entry/libs/arm64-v8a/dev_config.json`

This allows the configuration in dev_config.json to take effect after packaging.

**Note, the modified dev_config.json will be overridden by the dev_config.json configuration in the user directory below.**

## Write to the dev_config.json file in the user options folder through common events

Provided for developers and testers who can connect to HarmonyOS using hdc on the development machine for customization.

The dev_config.json in the user options folder will override the packaged dev_config.json.

Note that this override means that once there is a user data `dev_config.json`, the `dev_config.json` in the application packaging directory will no longer be read,
and there is no fine-grained ability to override by configuration item.

### Path to User Data Files

The path to user data files is under the sandbox path:

```
/data/storage/el2/base/preferences/dev_config.json
```

Absolute hdc shell physical path:

```
/data/app/el2/${user_id_usually_100}/base/${package_name}/preferences
```

### Push dev_config.json via hdc file send

On the development machine that can connect to hdc, enter the following command:

```bash
hdc file send ${development_machine_physical_path}/dev_config.json /data/app/el2/${user_id_usually_100}/base/${package_name}/preferences/
```

This will push the dev_config.json file to the user data folder.

If the new HarmonyOS version does not have the permission to push to the app sandbox, the following method is needed:

### Trigger a pop-up to select and import dev_config.json by sending a common event

#### Prerequisites for updating dev_config.json with a common event

You must add the following to the `gn` configuration file during compilation:

```gn
enable_cem_update_devconfig = true
```

This enables the ability to add the user directory's dev_config.json through a common event in the compiled browser/cef/electron.
(Also in the el1 directory)

The user must be able to connect to the PC via `hdc shell`,
and the browser application that needs to push the `dev_config.json` file must be running.

To enable the import of dev_config.json, send the following command:

```bash
hdc shell cem publish -e chromium_update_config -d <application package name>:update_config
```

A file selection dialog will pop up, where you need to select a dev_config.json from the user directory.

(You can send it to the user directory using hdc file send or create and modify it using a local text editor)

The file will then be copied to the application sandbox at:

`/data/app/el2/${user id usually 100}/base/${package name}/preferences/dev_config.json`

#### View the pushed dev_config.json file

Users connected to hdc and with permissions can view it directly by

```bash
hdc shell cat /data/app/el2/${user_id_usually_100}/base/${package_name}/preferences/dev_config.json
```

Non-root users can view it through a file browser. The specific steps are:
- Enter file management
- In the storage location sidebar on the left side of the interface, select the computer🖥️ icon
- In the two drive icons of `System` / `Personal`, select `System`
- Double-click the folder `Application Data` > double-click the folder `el2` > double-click the folder `base`
- Double-click into the folder corresponding to the application's Chinese/English package name
- Double-click into the `preferences` folder
- You can see the `dev_config.json` file

This file can be edited using a text editor in file management. After editing, it can be saved or deleted in file management.

## Delete dev_config.json in the User Directory

There are two ways to delete it:

1. Delete via File Management

Refer to the steps above for viewing via file management. After entering `preferences`, you can delete `dev_config.json` via file management.

2. Delete via Common Event

See below.
### Delete dev_config.json in the User Directory via Common Event

While keeping the browser application that needs to delete the `dev_config.json` file open,

enter the following command to send a common event to delete the `dev_config.json` in the corresponding package:

```bash
hdc shell cem publish -e chromium_update_config -d ${package_name}:clear
```

## Overwriting Mechanism of dev_config.json

The overwriting mechanism prioritizes the `dev_config.json` file in the user directory 
`/data/app/el2/${user_id_usually_100}/base/${package_name}/preferences/dev_config.json`.

If this file does not exist,
it uses the file in `libs/arm64-v8a/dev_config.json` within the package.

If neither exists, it uses the default values for each configuration item in `dev_config.json`.

### If you want to remove the impact of dev_config.json in the installed package and restore to default values

Since the content of the user data `/data/app/el2/${user_id_usually_100}/base/${package_name}/preferences/dev_config.json`
will prioritize overwriting the content of the installed package, you only need to push a `dev_config.json` file with empty object:

```json
{}
```

The empty file will use the default values for each configuration item,
which is equivalent to having no `dev_config.json` in any location.
