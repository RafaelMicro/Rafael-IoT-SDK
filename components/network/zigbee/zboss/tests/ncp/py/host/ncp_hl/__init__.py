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
# PURPOSE: Host-side HL protocol Python module

from .ncp_host_hl import *
# Uncomment se_cert_spec to use certificates from SE specification.
# Uncomment se_cert_dsr to use certificates generated for DSR
# Uncomment se_cert_customer to use certificates generated for customer
# Certificates from the specification allows testing of only 2 devices at a time while some tests requires at least 3 devices..
#from .se_cert_spec import *
#from .se_cert_dsr import *
from .se_cert_customer import *
