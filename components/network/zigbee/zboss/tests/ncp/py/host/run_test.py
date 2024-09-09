#/* ZBOSS Zigbee software protocol stack
# *
# * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
# * www.dsr-zboss.com
# * www.dsr-corporation.com
# * All rights reserved.
# *
# * This is unpublished proprietary source code of DSR Corporation
# * The copyright notice does not evidence any actual or intended
# * publication of such source code.
# *
# * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
# * Corporation
# *
# * Commercial Usage
# * Licensees holding valid DSR Commercial licenses may use
# * this file in accordance with the DSR Commercial License
# * Agreement provided with the Software or, alternatively, in accordance
# * with the terms contained in a written agreement between you and
# * DSR.
#
# PURPOSE: basic host side test running functionality

import os
import sys
import argparse
import logging
from host.test_runner import *

logger = logging.getLogger(__name__)

DEFAULT_TEST_DIR_PREF = ""


def parse_args() -> dict():
    parser = argparse.ArgumentParser(description="Run host side test")
    parser.add_argument("test_name", help="test name")
    parser.add_argument("channel_mask", type=int, help="channel mask")
    parser.add_argument("--test-dir",
                        metavar="somedir",
                        dest="test_dir",
                        default=DEFAULT_TEST_DIR_PREF,
                        help="import path to a folder with test scripts")

    args = parser.parse_args()

    return args


def main():
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)

    args = parse_args()

    script_path = os.path.realpath(__file__)
    sys.path.append(script_path)
    sys.path.append(script_path + '/' + args.test_dir)
    sys.path.append(args.test_dir)

    test_runner = TestRunner()
    logger.info("Running test runner")
    test_runner.run_test(args.test_name, args.channel_mask)


if __name__ == "__main__":
    main()
