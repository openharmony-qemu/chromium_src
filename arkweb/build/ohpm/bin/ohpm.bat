@ECHO OFF

SETLOCAL

SET "CUR_DIR=%~dp0"

SET NODE_EXE="%NODE_HOME%\node.exe"
IF NOT EXIST %NODE_EXE% (
  SET NODE_EXE=node
)

SET "PM_CLI_JS=%CUR_DIR%pm-cli.js"

SET "INIT_BAT=%CUR_DIR%init.bat"

%NODE_EXE% "%PM_CLI_JS%" -v  >nul 2>nul

IF %errorlevel% NEQ 0 (
  echo ERROR: ohpm has not been initialized yet. Execute the init.bat script to initialize it first.
  exit /b 1
)

%NODE_EXE% "%PM_CLI_JS%" %*





