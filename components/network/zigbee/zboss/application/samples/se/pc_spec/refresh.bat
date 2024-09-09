rem /* ZBOSS Zigbee software protocol stack
rem  *
rem  * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
rem  * www.dsr-zboss.com
rem  * www.dsr-corporation.com
rem  * All rights reserved.
rem  *
rem  * This is unpublished proprietary source code of DSR Corporation
rem  * The copyright notice does not evidence any actual or intended
rem  * publication of such source code.
rem  *
rem  * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
rem  * Corporation
rem  *
rem  * Commercial Usage
rem  * Licensees holding valid DSR Commercial licenses may use
rem  * this file in accordance with the DSR Commercial License
rem  * Agreement provided with the Software or, alternatively, in accordance
rem  * with the terms contained in a written agreement between you and
rem  * DSR.                                                                                                                                 
rem  PURPOSE:
@echo off
setlocal enabledelayedexpansion
for /R %%f in (*.txt) do (
set txt_name=%%f
set prod_name=!txt_name:~0,-4!.prod
echo !txt_name!
set cmd_generator=..\..\..\devtools\production_config_generator\production_config_generator.exe -i !txt_name! -o !prod_name! --verbose
call !cmd_generator!
)
endlocal
