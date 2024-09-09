#!/usr/bin/python
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
# PURPOSE: ZBOSS ECC Library test - check how function works on test vectors
#


import argparse
import os
# import re

TEXT_LIM = 20
# CA_PUB_KEY = re.compile("CA Pub Key\:\n([a-zA-Z0-9]+)")
# DEVICE_IMPLICIT_CERT = re.compile("CA Pub Key\:\n([a-zA-Z0-9]+)")

SUITE1 = 22
SUITE2 = 37


def parse_files(data_dir):

    def _reverse_and_modify(line):
        line = line.strip()
        string = ["0x%s" % line[i:i+2] for i in xrange(len(line)) if i % 2 == 0]

        return ",".join(string[::-1])

    def _modify(line):
        line = line.strip()
        string = ["0x%s" % line[i:i+2] for i in xrange(len(line)) if i % 2 == 0]

        return ",".join(string)

    result = {
        "Text Limit": 0,
        "Mask Limit": 0,
        "Test Name": [],
        "Test Mask": [],
        "Suite Type": [],
        "device 1": {
            "Test Mask": [],
            "CA Pub Key": [],
            "Device Implicit Cert": [],
            "Private Key Reconstruction Data": [],
            "Device Public Key": []
        },
        "device 2": {
            "Test Mask": [],
            "CA Pub Key": [],
            "Device Implicit Cert": [],
            "Private Key Reconstruction Data": [],
            "Device Public Key": []
        },
    }

    # not go recursive to subdiractories of data_dir
    for root, dirs, files in os.walk(data_dir):
        files = sorted(files)
        for f in files:
            test_name = ".".join(f.split(".")[:-1])
            result['Test Name'].append(test_name)
            if len(test_name) > result["Text Limit"]:
                result["Text Limit"] = len(test_name)

            with open(os.path.join(root, f), 'r') as fh:
                lines = fh.readlines()

            mask = lines[1].strip()
            result["Test Mask"].append(mask)
            if len(mask) > result["Mask Limit"]:
                result["Mask Limit"] = len(mask)

            result["device 1"]["CA Pub Key"].append(_modify(lines[3]))
            result["device 1"]["Device Implicit Cert"].append(_modify(lines[5]))
            result["device 1"]["Private Key Reconstruction Data"].append(_modify(lines[7]))
            result["device 1"]["Device Public Key"].append(_modify(lines[9]))

            result["device 2"]["CA Pub Key"].append(_modify(lines[11]))
            result["device 2"]["Device Implicit Cert"].append(_modify(lines[13]))
            result["device 2"]["Private Key Reconstruction Data"].append(_modify(lines[15]))
            result["device 2"]["Device Public Key"].append(_modify(lines[17]))

            result["Suite Type"].append(1 if len(lines[3].strip()) / 2 == SUITE1 else 2)

    return result


def generate(data_dir, header):
    with open(header, 'w') as fh:
        res = parse_files(data_dir)
        # fh.write("#include zb_ecc.h\n")
        fh.write("#define TEXT_LIM %d\n" % (int(res['Text Limit']) + 1))  # for null terminate symbol
        fh.write("#define MASK_LIM %d\n" % (int(res['Mask Limit']) + 1))  # for null terminate symbol
        fh.write("#define TEST_COUNT %s\n" % len(res["device 1"]["CA Pub Key"]))

        fh.write("zb_uint8_t test_names[TEST_COUNT][TEXT_LIM] = {\n")
        fh.write(",\n".join(['"%s"' % key for key in res["Test Name"]]))
        fh.write("};\n\n")

        # FIXME: need to compute max length of test_masks strings.
        fh.write("zb_uint8_t test_masks[TEST_COUNT][MASK_LIM] = {\n")
        fh.write(",\n".join(['"%s"' % key for key in res['Test Mask']]))
        fh.write("};\n\n")

        fh.write("zb_uint8_t suite_type[TEST_COUNT] = {\n")
        fh.write(",\n".join(["%s" % key for key in res["Suite Type"]]))
        fh.write("};\n\n")

        fh.write("zb_uint8_t CA_public_key_dev1[TEST_COUNT][ZB_ECC_MAX_PUB_KEY_LEN_IN_BYTES] = {\n")
        fh.write(",\n".join(["{%s}" % key for key in res["device 1"]["CA Pub Key"]]))
        fh.write("};\n\n")

        fh.write("zb_uint8_t CA_public_key_dev2[TEST_COUNT][ZB_ECC_MAX_PUB_KEY_LEN_IN_BYTES] = {\n")
        fh.write(",\n".join(["{%s}" % key for key in res["device 2"]["CA Pub Key"]]))
        fh.write("};\n\n")

        fh.write("zb_uint8_t cert_dev1[TEST_COUNT][ZB_ECC_SECT283_CERT_LEN] = {\n")
        fh.write(",\n".join(["{%s}" % key for key in res["device 1"]["Device Implicit Cert"]]))
        fh.write("};\n\n")

        fh.write("zb_uint8_t cert_dev2[TEST_COUNT][ZB_ECC_SECT283_CERT_LEN] = {\n")
        fh.write(",\n".join(["{%s}" % key for key in res["device 2"]["Device Implicit Cert"]]))
        fh.write("};\n\n")

        fh.write("zb_uint8_t private_key_dev1[TEST_COUNT][ZB_ECC_SECT163_CERT_LEN] = {\n")
        fh.write(",\n".join(["{%s}" % key for key in res["device 1"]["Private Key Reconstruction Data"]]))
        fh.write("};\n\n")

        fh.write("zb_uint8_t private_key_dev2[TEST_COUNT][ZB_ECC_SECT283_CERT_LEN] = {\n")
        fh.write(",\n".join(["{%s}" % key for key in res["device 2"]["Private Key Reconstruction Data"]]))
        fh.write("};\n\n")

        fh.write("zb_uint8_t public_key_dev1[TEST_COUNT][ZB_ECC_SECT163_CERT_LEN] = {\n")
        fh.write(",\n".join(["{%s}" % key for key in res["device 1"]["Device Public Key"]]))
        fh.write("};\n\n")

        fh.write("zb_uint8_t public_key_dev2[TEST_COUNT][ZB_ECC_SECT283_CERT_LEN] = {\n")
        fh.write(",\n".join(["{%s}" % key for key in res["device 2"]["Device Public Key"]]))
        fh.write("};\n\n")


def main():
    parser = argparse.ArgumentParser(
            usage="%(prog)s [options]",
            description="Tool for collecting necessary system information.")

    parser.add_argument("-d", "--dir", type=str, default=None, required=True,
                        help="Path to directory with test data")

    parser.add_argument("--header-name", type=str, default="data.h",
                        help="Path to result header for saving test data")

    # parser.add_argument("--log-level", type=str, default="WARNING",
    #                     help=("Define logging level: DEBUG, INFO, WARNING, "
    #                           "ERROR, CRITICAL"))

    args = parser.parse_args()

    # for root, dirs, files in os.walk(args.dir):
    #     print root, dirs, files
    generate(args.dir, args.header_name)

if __name__ == "__main__":
    main()
