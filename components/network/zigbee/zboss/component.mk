
# Component Makefile

# CPPFLAGS += -DTRACE_PRINT
# CPPFLAGS += -DZB_TRACE_TO_PORT
# CPPFLAGS += -DZB_TRACE_LEVEL=5
 #CPPFLAGS += -DZB_TRACE_MASK=0x38000000
CPPFLAGS += -DZB_TRACE_MASK=0x0
CPPFLAGS += -DZB_LIMIT_VISIBILITY
CPPFLAGS += -DAPS_FRAGENTATION
CPPFLAGS += -DZB_CONFIGURABEL_MEM
CPPFLAGS += -DZB_SOFT_SECURITY
CPPFLAGS += -DZB_MAC_RX_QUEUE_CAP=4
CPPFLAGS += -DZB_LITTLE_ENDIAN
CPPFLAGS += -DZB_NEED_ALIGN
CPPFLAGS += -DZB_AUTO_ACK_TX
CPPFLAGS += -DZB_TEST_PROFILE

## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include platform/include platform/osif/include
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := aps common

## This component's src 
COMPONENT_SRCS := aps/aps_aib.c \
				  aps/aps_bind.c \
				  aps/aps_commands.c \
				  aps/aps_dups.c \
				  aps/aps_interpan.c \
				  aps/aps_main.c \
				  aps/aps_nwk_confirm.c \
				  aps/aps_group_table.c \
				  commissioning/bdb/bdb_finding_binding.c \
				  commissioning/bdb/bdb_touchlink.c \
				  commissioning/bdb/zdo_commissioning_bdb.c \
				  commissioning/bdb/zdo_commissioning_bdb_formation.c \
				  commissioning/legacy/zdo_commissioning_classic.c \
				  common/zb_24bit_math.c \
				  common/zb_48bit_math.c \
				  common/zb_address.c \
				  common/zb_bufpool_mult.c \
				  common/zb_bufpool_mult_storage.c \
				  common/zb_channel_page.c \
				  common/zb_debug.c \
				  common/zb_error_indication.c \
				  common/zb_init_common.c \
				  common/zb_init_default.c \
				  common/zb_nvram.c \
				  common/zb_nvram_custom_handlers.c \
				  common/zb_random.c \
				  common/zb_scheduler.c \
				  common/zb_scheduler_init.c \
				  common/zb_serial_trace_bin.c \
				  common/zb_sleep.c \
				  common/zb_time.c \
				  common/zb_trace_common.c \
				  common/zb_watchdog.c \
				  common/zb_signal_common.c \
				  mac/mac.c \
				  mac/mac_api_trace.c \
				  mac/mac_associate.c \
				  mac/mac_common.c \
				  mac/mac_cr_associate.c \
				  mac/mac_cr_coordinator.c \
				  mac/mac_cr_data.c \
				  mac/mac_data.c \
				  mac/mac_dump.c \
				  mac/mac_fcs.c \
				  mac/mac_optional.c \
				  mac/mac_pib.c \
				  mac/mac_scan.c \
				  mac/mac_visibility.c \
				  mac/mac_zcl_diagnostic.c \
				  mac/mac_zgp.c \
				  nwk/nwk_address_assign.c \
				  nwk/nwk_address_conflict.c \
				  nwk/nwk_broadcasting.c \
				  nwk/nwk_cr_formation.c \
				  nwk/nwk_cr_join.c \
				  nwk/nwk_cr_mesh_routing.c \
				  nwk/nwk_cr_parent.c \
				  nwk/nwk_cr_permit_join.c \
				  nwk/nwk_cr_route_discovery.c \
				  nwk/nwk_cr_tree_routing.c \
				  nwk/nwk_discovery.c \
				  nwk/nwk_formation.c \
				  nwk/nwk_join.c \
				  nwk/nwk_link_status.c \
				  nwk/nwk_main.c \
				  nwk/nwk_mm.c \
				  nwk/nwk_neighbor.c \
				  nwk/nwk_neighbor_utility.c \
				  nwk/nwk_nlme.c \
				  nwk/nwk_panid_conflict.c \
				  nwk/nwk_srouting.c \
				  nwk/zb_nwk_ed_aging.c \
				  secur/aps_secur.c \
				  secur/apsme_secur.c \
				  secur/bdb_secur.c \
				  secur/ic_secur.c \
				  secur/mac_secur.c \
				  secur/nwk_secur.c \
				  secur/secur_ccm.c \
				  secur/secur_formation.c \
				  secur/zb_ecc.c \
				  secur/zdo_secur.c \
				  zcl/ha_sas.c \
				  zcl/zcl_alarms_commands.c \
				  zcl/zcl_attr_value.c \
				  zcl/zcl_basic_commands.c \
				  zcl/zcl_color_control_commands.c \
				  zcl/zcl_common.c \
				  zcl/zcl_continuous_value_change_commands.c \
				  zcl/zcl_custom_cluster_commands.c \
				  zcl/zcl_diagnostics_commands.c \
				  zcl/zcl_fan_control.c \
				  zcl/zcl_general_commands.c \
				  zcl/zcl_groups.c \
				  zcl/zcl_ias_ace_commands.c \
				  zcl/zcl_ias_wd_commands.c \
				  zcl/zcl_ias_zone_commands.c \
				  zcl/zcl_identify_commands.c \
				  zcl/zcl_illuminance_measurement.c \
				  zcl/zcl_ir_blaster.c \
				  zcl/zcl_level_control_commands.c \
				  zcl/zcl_main.c \
				  zcl/zcl_on_off_commands.c \
				  zcl/zcl_on_off_switch_config.c \
				  zcl/zcl_ota_upgrade_commands.c \
				  zcl/zcl_ota_upgrade_common.c \
				  zcl/zcl_ota_upgrade_minimal.c \
				  zcl/zcl_ota_upgrade_srv_commands.c \
				  zcl/zcl_poll_control_client.c \
				  zcl/zcl_poll_control_commands.c \
				  zcl/zcl_power_config_commands.c \
				  zcl/zcl_reporting.c \
				  zcl/zcl_scenes.c \
				  zcl/zcl_shade_config_commands.c \
				  zcl/zcl_temp_measurement.c \
				  zcl/zcl_door_lock.c \
				  zcl/zcl_time.c \
				  zcl/zcl_tunnel.c \
				  zcl/zcl_window_covering.c \
				  zcl/zcl_nvram.c \
				  zcl/zcl_rel_humidity.c \
				  zcl/zcl_s_metering.c \
				  zcl/zcl_el_measurement.c \
				  zdo/af_descriptor.c \
				  zdo/af_interpan.c \
				  zdo/af_rx.c \
				  zdo/test_profile.c \
				  zdo/zb_zboss_api_common.c \
				  zdo/zb_zboss_api_default.c \
				  zdo/zdo_app.c \
				  zdo/zdo_app_common.c \
				  zdo/zdo_app_join.c \
				  zdo/zdo_app_leave.c \
				  zdo/zdo_app_prod_conf.c \
				  zdo/zdo_bind_manage.c \
				  zdo/zdo_channel_manager.c \
				  zdo/zdo_commissioning.c \
				  zdo/zdo_common.c \
				  zdo/zdo_diagnostics.c \
				  zdo/zdo_disc_cli.c \
				  zdo/zdo_disc_srv.c \
				  zdo/zdo_ed_aging.c \
				  zdo/zdo_formation.c \
				  zdo/zdo_nwk_manage_cli.c \
				  zdo/zdo_nwk_manage_srv.c \
				  zdo/zdo_poll.c \
				  zdo/zdo_resp_validate.c \
				  zdo/zdo_rx.c \
				  zgp/zgp_cluster.c \
				  zgp/zgp_cluster_gp.c \
				  zgp/zgp_common.c \
				  zgp/zgp_helper.c \
				  zgp/zgp_proxy_table.c \
				  zgp/zgp_secur.c \
				  zgp/zgp_sink.c \
				  zgp/zgp_stub.c \
				  zgp/zgp_tx_queue.c \
				  zgp/zgp_zgpd.c \
				  zgp/zgp_zgpd_app_table.c \
				  zse/zse_aps_key_est.c \
				  zse/zse_certdb.c \
				  zse/zse_common.c \
				  zse/zse_kec_cluster.c \
				  zse/zse_sas.c \
				  zse/zse_service_discovery.c \
				  zse/zse_steady_state.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := aps common mac nwk secur zcl zdo zgp zse commissioning/bdb commissioning/legacy
