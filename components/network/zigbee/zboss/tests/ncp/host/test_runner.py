# /***************************************************************************
# *                  ZBOSS Zigbee software protocol stack                    *
# *                                                                          *
# *          Copyright (c) 2012-2019 DSR Corporation Denver CO, USA.         *
# *                       http://www.dsr-zboss.com                           *
# *                       http://www.dsr-corporation.com                     *
# *                                                                          *
# *                            All rights reserved.                          *
# *                                                                          *
# *                                                                          *
# * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR   *
# * Corporation                                                              *
# *                                                                          *
# * Commercial Usage                                                         *
# * Licensees holding valid DSR Commercial licenses may use                  *
# * this file in accordance with the DSR Commercial License                  *
# * Agreement provided with the Software or, alternatively, in accordance    *
# * with the terms contained in a written agreement between you and          *
# * DSR.                                                                     *
# *                                                                          *
# ****************************************************************************
# PURPOSE: basic host side test running functionality
# */

import importlib
import logging
import platform
import traceback
import os

logger = logging.getLogger(__name__)

DEFAULT_TEST_DIR_PREF = ""


class TestRunner:

    def run_test(self, test_name, channel_mask):
        self.clean_logs()
        self.setup_log(test_name)

        try:
            test_module = importlib.import_module(test_name)

            test = test_module.Test(channel_mask)
            logger.info("Running test {} on channel mask {}".format(test_name, channel_mask))
            test.init_host(self.get_ll_so_name())
            test.run()
        except KeyboardInterrupt:
            return
        except Exception as e:
            logger.error(e)
            logger.error(traceback.format_exc())
        finally:
            logging.getLogger().removeHandler(fh)


    def setup_log(self, test_name):
        log_path = test_name + '.log'
        log_number = 0
        while (os.path.exists(log_path)):
            log_number += 1
            log_path = '{}_{}.log'.format(test_name, log_number)

        loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
        loggerFormatter = logging.Formatter(loggerFormat)
        fh = logging.FileHandler(log_path, 'w')
        fh.setLevel(logger.getEffectiveLevel())
        fh.setFormatter(loggerFormatter)
        logging.getLogger().addHandler(fh)

    def clean_logs(self):
        for f in os.listdir('.'):
            if f.endswith('.log'):
                os.remove(f)


    # Is it PC with a simulator build or RPi with NCP connected over SPI?
    def get_ll_so_name(self):
        a = platform.machine()
        if a == "x86_64":
            return "../ncp_ll_nsng.so"
        else:
            return "./zbs_ncp_rpi.so"
