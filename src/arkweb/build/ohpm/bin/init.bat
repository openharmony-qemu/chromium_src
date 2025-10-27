@ECHO OFF

SETLOCAL

SET "CUR_DIR=%~dp0"

SET NPM_CMD="%NODE_HOME%\npm.cmd"
IF NOT EXIST %NPM_CMD% (
  SET NPM_CMD=npm
)

cd %CUR_DIR%..

set CONFIG_PATH="%USERPROFILE%\.ohpm\.ohpmrc"

for %%A in ("%CUR_DIR%\..") do set "PARENT_DIR=%%~fA"

set TEMPLATE_CONFIG="%PARENT_DIR%\resources\.ohpmrc"

if not exist "%CONFIG_PATH%" (
  mkdir "%USERPROFILE%\.ohpm" >nul 2>nul
  echo f | xcopy /y "%TEMPLATE_CONFIG%" "%CONFIG_PATH%" >nul 2>nul
)

SET NPM_ARGS=
IF NOT "%REGISTRY%"=="" (
  SET "NPM_ARGS=%NPM_ARGS% --registry %REGISTRY%"
)
IF NOT "%STRICT_SSL%"=="" (
  SET "NPM_ARGS=%NPM_ARGS% --strict-ssl %STRICT_SSL%"
)

%NPM_CMD% install %NPM_ARGS%
