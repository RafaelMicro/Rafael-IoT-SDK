@echo off
rem /*  Copyright (c)  DSR Corporation, Denver CO, USA.
rem  PURPOSE: Batch file to check MISRA-C 2012 compliance.

set EWB=%ProgramFiles(x86)%\IAR Systems\Embedded Workbench 8.2
set TISDK=C:\ti\simplelink_cc13x2_26x2_sdk_4_20_01_04

set PATH="%EWB%\arm\bin";%PATH%

rem If the script runs from SDK structure then there is '.\ncp' folder.
rem Otherwise, the script is run from repository and already located in 'ncp' folder.
if exist ".\ncp" (set NCP=".\ncp") else (set NCP=".")

set LL_DIR="%NCP%\low_level"
set SRCS=%LL_DIR%\zbncp_debug.c %LL_DIR%\zbncp_ll_impl.c %LL_DIR%\zbncp_mem.c %LL_DIR%\zbncp_node.c %LL_DIR%\zbncp_tr_impl.c %LL_DIR%\zbncp_utils.c %LL_DIR%\zbncp_fragmentation.c %LL_DIR%\zbncp_divider.c %LL_DIR%\zbncp_reassembling.c
set PROJ=zb_ncp_ll
set LIST=STDCHECKS,CERT,SECURITY,MISRAC2012
set CHKS=%PROJ%_checks.ch
set CHDB=%PROJ%_icstat.db
set REPT=%PROJ%_report.html
set CFLG=-c --char_is_signed --strict -DZB_NCP_HOST_RPI=1 -I"%TISDK%\source\ti\posix\iar" -I%NCP%

ichecks --default %LIST% --output %CHKS%
icstat --db %CHDB% clear

for %%f in (%SRCS%) do (
  icstat --db %CHDB% --checks %CHKS% analyze -- iccarm %CFLG% %%f
)

ireport --db %CHDB% --project %PROJ% --full --output %REPT%
