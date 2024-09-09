#!/usr/bin/python3

import argparse
import re
import os
import shutil
import subprocess
from pathlib import Path


class TestDescription:
    def __init__(self, role, name, sources):
        self.role = role
        self.name = name
        self.sources = sources


class TestsListGenerator:
    TESTS_TABLE_COPYRIGHT = '''/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/* PURPOSE:
 Auto-generated file! Do not edit!
*/'''

    SRCS_LIST_COPYRIGHT = '''#/* ZBOSS Zigbee software protocol stack
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
# PURPOSE:
# Auto-generated file! Do not edit!
#'''

    def __init__(self):
        self.testbin_dir = Path(os.getcwd())
        assert 'testbin' in self.testbin_dir.as_posix()

        self.zboss_dir = self.testbin_dir.parents[2]

    def generate_tests_list(self):
        os.chdir(self.zboss_dir.as_posix())

        shutil.move('Options', 'tmp_Options')
        shutil.copy('tests/multitest_common/Options-gen-tests-list', 'Options')

        os.chdir(self.testbin_dir.parent)

        tests_dirs_list = [dir for dir in os.listdir('.') if os.path.isdir(dir) and dir.startswith('RTP_')]
        tests_dirs_list.sort()

        tests_sources_list = []

        for test_dir in tests_dirs_list:
            os.chdir(test_dir)

            make_result = subprocess.run(['make'], capture_output=True, encoding='ASCII').stdout.split('\n', )
            tests_sources_list += filter(None, make_result)

            os.chdir('..')

        os.chdir(self.testbin_dir.as_posix())

        self.generate_sources_list(tests_sources_list)

        os.chdir(self.zboss_dir.as_posix())
        shutil.move('tmp_Options', 'Options')
        os.chdir(self.testbin_dir.as_posix())

    def generate_sources_list(self, tests_sources_list):
        zc_zr_tests = []
        zed_tests = []

        for test_sources in tests_sources_list:
            test_role, test_name, *sources = test_sources.split()

            if test_role == 'ZC' or test_role == 'ZR':
                zc_zr_tests.append(TestDescription(test_role, test_name, sources))
            elif test_role == 'ED':
                zed_tests.append(TestDescription(test_role, test_name, sources))

        srcs_list_file_content = []
        srcs_list_file_content.append(self.SRCS_LIST_COPYRIGHT + '\n\n')
        srcs_list_file_content.append('include ./tags_config\n')
        srcs_list_file_content.append('\n')

        srcs_list_file_content += self.generate_tests_sources_declaration('SRCS_ZCZR', zc_zr_tests)
        srcs_list_file_content += self.generate_tests_sources_declaration('SRCS_ZED', zed_tests)

        with open('srcs_list', 'w') as srcs_list_file:
            srcs_list_file.writelines(srcs_list_file_content)

        zc_tests_table_content = self.generate_tests_table('ZC', zc_zr_tests)

        with open('zc_tests_table.h', 'w') as tests_table_file:
            tests_table_file.writelines(zc_tests_table_content)

        zed_tests_table_content = self.generate_tests_table('ZED', zed_tests)

        with open('zed_tests_table.h', 'w') as tests_table_file:
            tests_table_file.writelines(zed_tests_table_content)

    def generate_tests_sources_declaration(self, list_name, tests_descriptions):
        sources_declaration = []
        sources_declaration.append('{}=\n'.format(list_name))

        for test_description in tests_descriptions:
            tags = self.get_test_tags_list(test_description)

            if len(tags) > 0:
                check_disabled_tags = ','.join(map(lambda tag_name: '$(ZB_TEST_DISABLE_TAG_{})'.format(tag_name), tags))

                sources_declaration.append('ifneq ($(or {}), 1)\n'.format(check_disabled_tags))
                sources_declaration.append('{}_ANY_OF_TAGS_NOT_DISABLED=1\n'.format(test_description.name))
                sources_declaration.append('endif\n')

                sources_declaration.append('ifneq ($(or $(and $(ZB_TEST_ENABLE_ALL_TAGS),$({}_ANY_OF_TAGS_NOT_DISABLED)),$(and {})),)\n'.format(
                    test_description.name,
                    ','.join(map(lambda tag_name: '$(ZB_TEST_ENABLE_TAG_{})'.format(tag_name), tags))))

            sources_declaration.append('{}+={}\n'.format(list_name, ' '.join(test_description.sources)))

            if len(tags) > 0:
                sources_declaration.append('endif\n')

        sources_declaration.append('\n')

        return sources_declaration

    def get_test_tags_list(self, test_description):
        tags_list = list(filter(None, [re.findall(r'^#define\sZB_TEST_TAG_(\w+)\s(.*)$', line)
            for line in open(test_description.sources[0])]))

        return [tag[0][1] for tag in tags_list]

    def format_test_name(self, raw_test_name):
        return raw_test_name.upper().replace('-', '_')

    def generate_tests_table(self, table_name, tests_descriptions):
        tests_table_content = []
        tests_table_content.append(self.TESTS_TABLE_COPYRIGHT + '\n\n')

        tests_table_header_name = "{}_TESTS_TABLE_H".format(table_name.upper())
        tests_table_content.append("#ifndef {}\n".format(tests_table_header_name))
        tests_table_content.append("#define {}\n".format(tests_table_header_name))
        tests_table_content.append("\n")

        for test_description in tests_descriptions:
            test_name = self.format_test_name(test_description.name)

            tests_table_content.append("void {}_main();\n".format(test_name))
            tests_table_content.append("void {}_zb_zdo_startup_complete(zb_uint8_t param);\n".format(test_name))

        tests_table_content.append("void NVRAM_ERASE_main();\n")
        tests_table_content.append("\n")

        tests_table_content.append("static const zb_test_table_t s_tests_table[] = {\n")

        current_test_group = None

        for test_description in tests_descriptions:
            test_name = self.format_test_name(test_description.name)
            test_group = test_name.split('_')[1]

            if current_test_group != test_group:
                if current_test_group is not None:
                    tests_table_content.append("#endif /* ZB_REG_TEST_GROUP_{} */\n".format(current_test_group))

                tests_table_content.append("#if defined ZB_REG_TEST_GROUP_{}\n".format(test_group))

                current_test_group = test_group

            tags = self.get_test_tags_list(test_description)

            if len(tags) > 0:
                tests_table_content.append("#if (defined(ZB_TEST_ENABLE_ALL_TAGS) && ({})) || ({})\n".format(
                    ' && '.join(map(lambda tag_name: '!defined(ZB_TEST_DISABLE_TAG_{})'.format(tag_name), tags)),
                    ' && '.join(map(lambda tag_name: 'defined(ZB_TEST_ENABLE_TAG_{})'.format(tag_name), tags)),
                    ))

            tests_table_content.append("{{ \"{}\", {}_main, {}_zb_zdo_startup_complete }},\n".format(
                test_name, test_name, test_name))

            if len(tags) > 0:
                tests_table_content.append('#endif\n')

        tests_table_content.append("#endif /* ZB_REG_TEST_GROUP_{} */\n".format(current_test_group))
        tests_table_content.append("{ \"NVRAM_ERASE\", NVRAM_ERASE_main, NULL},\n")
        tests_table_content.append("};\n")

        tests_table_content.append("\n")
        tests_table_content.append("#endif /* {} */".format(tests_table_header_name))

        return tests_table_content

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='ZBOSS tests list generator')
    # parser.add_argument('--tags_enabled', type=str, nargs='*', help='List of enabled tags', default=[])
    # parser.add_argument('--tags_disabled', type=str, nargs='*', help='List of disabled tags', default=[])
    # parser.add_argument('--tags_enable_all', nargs=1, type=int, default=1)

    # args = parser.parse_args()

    tests_list_generator = TestsListGenerator()
    tests_list_generator.generate_tests_list()
