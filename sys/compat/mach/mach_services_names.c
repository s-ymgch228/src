/* $NetBSD: mach_services_names.c,v 1.3 2003/11/17 01:52:14 manu Exp $ */

/*
 * Mach services names. This file is not built
 * by the kernel, it is included by kdump sources.
 *
 * created from  $NetBSD: mach_services_names.c,v 1.3 2003/11/17 01:52:14 manu Exp $
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: mach_services_names.c,v 1.3 2003/11/17 01:52:14 manu Exp $");

struct mach_service_name {
	int srv_id;
	const char *srv_name;
};

struct mach_service_name mach_services_names[] = {
	{64, "obsolete notify_first"},
	{65, "notify_port_deleted"},
	{66, "obsolete notify_msg_accepted"},
	{67, "obsolete notify_ownership_rights"},
	{68, "obsolete notify_receive_rights"},
	{69, "notify_port_destroyed"},
	{70, "notify_port_no_senders"},
	{71, "notify_port_send_once"},
	{72, "notify_port_dead_name"},
	{200, "host_info"},
	{201, "unimpl. host_kernel_version"},
	{202, "host_page_size"},
	{203, "unimpl. memory_object_memory_entry"},
	{204, "unimpl. host_processor_info"},
	{205, "host_get_io_master"},
	{206, "host_get_clock_service"},
	{207, "unimpl. kmod_get_info"},
	{208, "unimpl. host_zone_info"},
	{209, "unimpl. host_virtual_physical_table_info"},
	{210, "unimpl. host_ipc_hash_info"},
	{211, "unimpl. enable_bluebox"},
	{212, "unimpl. disable_bluebox"},
	{213, "unimpl. processor_set_default"},
	{214, "unimpl. processor_set_create"},
	{215, "unimpl. memory_object_memory_entry_64"},
	{216, "unimpl. host_statistics"},
	{400, "unimpl. host_get_boot_info"},
	{401, "unimpl. host_reboot"},
	{402, "unimpl. host_priv_statistics"},
	{403, "unimpl. host_default_memory_manager"},
	{404, "bootstrap_look_up"},
	{405, "unimpl. thread_wire"},
	{406, "unimpl. vm_allocate_cpm"},
	{407, "unimpl. host_processors"},
	{408, "unimpl. host_get_clock_control"},
	{409, "unimpl. kmod_create"},
	{410, "unimpl. kmod_destroy"},
	{411, "unimpl. kmod_control"},
	{412, "unimpl. host_get_special_port"},
	{413, "unimpl. host_set_special_port"},
	{414, "unimpl. host_set_exception_ports"},
	{415, "unimpl. host_get_exception_ports"},
	{416, "unimpl. host_swap_exception_ports"},
	{417, "unimpl. host_load_symbol_table"},
	{418, "unimpl. task_swappable"},
	{419, "unimpl. host_processor_sets"},
	{420, "unimpl. host_processor_set_priv"},
	{421, "unimpl. set_dp_control_port"},
	{422, "unimpl. get_dp_control_port"},
	{423, "unimpl. host_set_UNDServer"},
	{424, "unimpl. host_get_UNDServer"},
	{600, "unimpl. host_security_create_task_token"},
	{601, "unimpl. host_security_set_task_token"},
	{1000, "clock_get_time"},
	{1001, "unimpl. clock_get_attributes"},
	{1002, "unimpl. clock_alarm"},
	{1200, "unimpl. clock_set_time"},
	{1201, "unimpl. clock_set_attributes"},
	{2000, "unimpl. memory_object_get_attributes"},
	{2001, "unimpl. memory_object_change_attributes"},
	{2002, "unimpl. memory_object_synchronize_completed"},
	{2003, "unimpl. memory_object_lock_request"},
	{2004, "unimpl. memory_object_destroy"},
	{2005, "unimpl. memory_object_upl_request"},
	{2006, "unimpl. memory_object_super_upl_request"},
	{2007, "unimpl. memory_object_page_op"},
	{2008, "unimpl. memory_object_recover_named"},
	{2009, "unimpl. memory_object_release_name"},
	{2050, "unimpl. upl_abort"},
	{2051, "unimpl. upl_abort_range"},
	{2052, "unimpl. upl_commit"},
	{2053, "unimpl. upl_commit_range"},
	{2200, "unimpl. memory_object_init"},
	{2201, "unimpl. memory_object_terminate"},
	{2202, "unimpl. memory_object_data_request"},
	{2203, "unimpl. memory_object_data_return"},
	{2204, "unimpl. memory_object_data_initialize"},
	{2205, "unimpl. memory_object_data_unlock"},
	{2206, "unimpl. memory_object_synchronize"},
	{2207, "unimpl. memory_object_unmap"},
	{2250, "unimpl. memory_object_create"},
	{2275, "unimpl. default_pager_object_create"},
	{2276, "unimpl. default_pager_info"},
	{2277, "unimpl. default_pager_objects"},
	{2278, "unimpl. default_pager_object_pages"},
	{2279, "unimpl. default_pager_backing_store_create"},
	{2280, "unimpl. default_pager_backing_store_delete"},
	{2281, "unimpl. default_pager_add_segment"},
	{2282, "unimpl. default_pager_backing_store_info"},
	{2283, "unimpl. default_pager_add_file"},
	{2284, "unimpl. default_pager_triggers"},
	{2295, "unimpl. default_pager_space_alert"},
	{2401, "exception_raise"},
	{2402, "exception_raise_state"},
	{2403, "exception_raise_state_identity"},
	{2450, "unimpl. samples"},
	{2451, "unimpl. notices"},
	{2800, "io_object_get_class"},
	{2801, "io_object_conforms_to"},
	{2802, "io_iterator_next"},
	{2803, "io_iterator_reset"},
	{2804, "io_service_get_matching_services"},
	{2805, "io_registry_entry_get_property"},
	{2806, "unimpl. io_registry_create_iterator"},
	{2807, "unimpl. io_registry_iterator_enter_entry"},
	{2808, "unimpl. io_registry_iterator_exit_entry"},
	{2809, "io_registry_entry_from_path"},
	{2810, "unimpl. io_registry_entry_get_name"},
	{2811, "io_registry_entry_get_properties"},
	{2812, "unimpl. io_registry_entry_get_property_bytes"},
	{2813, "io_registry_entry_get_child_iterator"},
	{2814, "io_registry_entry_get_parent_iterator"},
	{2815, "io_service_open"},
	{2816, "io_service_close"},
	{2817, "io_connect_get_service"},
	{2818, "io_connect_set_notification_port"},
	{2819, "io_connect_map_memory"},
	{2820, "io_connect_add_client"},
	{2821, "io_connect_set_properties"},
	{2822, "io_connect_method_scalari_scalaro"},
	{2823, "io_connect_method_scalari_structo"},
	{2824, "io_connect_method_scalari_structi"},
	{2825, "io_connect_method_structi_structo"},
	{2826, "io_registry_entry_get_path"},
	{2827, "io_registry_get_root_entry"},
	{2828, "unimpl. io_registry_entry_set_properties"},
	{2829, "unimpl. io_registry_entry_in_plane"},
	{2830, "unimpl. io_object_get_retain_count"},
	{2831, "unimpl. io_service_get_busy_state"},
	{2832, "unimpl. io_service_wait_quiet"},
	{2833, "io_registry_entry_create_iterator"},
	{2834, "unimpl. io_iterator_is_valid"},
	{2835, "unimpl. io_make_matching"},
	{2836, "unimpl. io_catalog_send_data"},
	{2837, "unimpl. io_catalog_terminate"},
	{2838, "unimpl. io_catalog_get_data"},
	{2839, "unimpl. io_catalog_get_gen_count"},
	{2840, "unimpl. io_catalog_module_loaded"},
	{2841, "unimpl. io_catalog_reset"},
	{2842, "unimpl. io_service_request_probe"},
	{2843, "io_registry_entry_get_name_in_plane"},
	{2844, "unimpl. io_service_match_property_table"},
	{2845, "unimpl. io_async_method_scalari_scalaro"},
	{2846, "unimpl. io_async_method_scalari_structo"},
	{2847, "unimpl. io_async_method_scalari_structi"},
	{2848, "unimpl. io_async_method_structi_structo"},
	{2849, "unimpl. io_service_add_notification"},
	{2850, "io_service_add_interest_notification"},
	{2851, "unimpl. io_service_acknowledge_notification"},
	{2852, "unimpl. io_connect_get_notification_semaphore"},
	{2853, "unimpl. io_connect_unmap_memory"},
	{2854, "io_registry_entry_get_location_in_plane"},
	{2855, "unimpl. io_registry_entry_get_property_recursively"},
	{3000, "unimpl. processor_start"},
	{3001, "unimpl. processor_exit"},
	{3002, "unimpl. processor_info"},
	{3003, "unimpl. processor_control"},
	{3004, "unimpl. processor_assign"},
	{3005, "unimpl. processor_get_assignment"},
	{3200, "unimpl. port_names"},
	{3201, "port_type"},
	{3202, "unimpl. port_rename"},
	{3203, "unimpl. port_allocate_name"},
	{3204, "port_allocate"},
	{3205, "port_destroy"},
	{3206, "port_deallocate"},
	{3207, "unimpl. port_get_refs"},
	{3208, "unimpl. port_mod_refs"},
	{3210, "unimpl. port_set_mscount"},
	{3211, "unimpl. port_get_set_status"},
	{3212, "port_move_member"},
	{3213, "port_request_notification"},
	{3214, "port_insert_right"},
	{3215, "unimpl. port_extract_right"},
	{3216, "unimpl. port_set_seqno"},
	{3217, "port_get_attributes"},
	{3218, "port_set_attributes"},
	{3219, "unimpl. port_allocate_qos"},
	{3220, "unimpl. port_allocate_full"},
	{3221, "unimpl. task_set_port_space"},
	{3222, "unimpl. port_get_srights"},
	{3223, "unimpl. port_space_info"},
	{3224, "unimpl. port_dnrequest_info"},
	{3225, "unimpl. port_kernel_object"},
	{3226, "port_insert_member"},
	{3227, "unimpl. port_extract_member"},
	{3400, "unimpl. task_create"},
	{3401, "unimpl. task_terminate"},
	{3402, "task_threads"},
	{3403, "unimpl. ports_register"},
	{3404, "ports_lookup"},
	{3405, "task_info"},
	{3406, "unimpl. task_set_info"},
	{3407, "task_suspend"},
	{3408, "task_resume"},
	{3409, "task_get_special_port"},
	{3410, "task_set_special_port"},
	{3411, "unimpl. thread_create"},
	{3412, "thread_create_running"},
	{3413, "task_set_exception_ports"},
	{3414, "task_get_exception_ports"},
	{3415, "unimpl. task_swap_exception_ports"},
	{3416, "unimpl. lock_set_create"},
	{3417, "unimpl. lock_set_destroy"},
	{3418, "semaphore_create"},
	{3419, "semaphore_destroy"},
	{3420, "unimpl. task_policy_set"},
	{3421, "unimpl. task_policy_get"},
	{3422, "unimpl. task_sample"},
	{3423, "unimpl. task_policy"},
	{3424, "unimpl. task_set_emulation"},
	{3425, "unimpl. task_get_emulation_vector"},
	{3426, "unimpl. task_set_emulation_vector"},
	{3427, "unimpl. task_set_ras_pc"},
	{3428, "unimpl. kernel_task_create"},
	{3429, "unimpl. task_assign"},
	{3430, "unimpl. task_assign_default"},
	{3431, "unimpl. task_get_assignment"},
	{3432, "unimpl. task_set_policy"},
	{3600, "unimpl. thread_terminate"},
	{3601, "unimpl. act_get_state"},
	{3602, "unimpl. act_set_state"},
	{3603, "thread_get_state"},
	{3604, "thread_set_state"},
	{3605, "unimpl. thread_suspend"},
	{3606, "unimpl. thread_resume"},
	{3607, "unimpl. thread_abort"},
	{3608, "unimpl. thread_abort_safely"},
	{3609, "unimpl. thread_depress_abort"},
	{3610, "unimpl. thread_get_special_port"},
	{3611, "unimpl. thread_set_special_port"},
	{3612, "thread_info"},
	{3613, "unimpl. thread_set_exception_ports"},
	{3614, "unimpl. thread_get_exception_ports"},
	{3615, "unimpl. thread_swap_exception_ports"},
	{3616, "thread_policy"},
	{3617, "unimpl. thread_policy_set"},
	{3618, "unimpl. thread_policy_get"},
	{3619, "unimpl. thread_sample"},
	{3620, "unimpl. etap_trace_thread"},
	{3621, "unimpl. thread_assign"},
	{3622, "unimpl. thread_assign_default"},
	{3623, "unimpl. thread_get_assignment"},
	{3624, "unimpl. thread_set_policy"},
	{3800, "vm_region"},
	{3801, "vm_allocate"},
	{3802, "vm_deallocate"},
	{3803, "vm_protect"},
	{3804, "vm_inherit"},
	{3805, "unimpl. vm_read"},
	{3806, "unimpl. vm_read_list"},
	{3807, "unimpl. vm_write"},
	{3808, "vm_copy"},
	{3809, "unimpl. vm_read_overwrite"},
	{3810, "vm_msync"},
	{3811, "unimpl. vm_behavior_set"},
	{3812, "vm_map"},
	{3813, "unimpl. vm_machine_attribute"},
	{3814, "unimpl. vm_remap"},
	{3815, "unimpl. task_wire"},
	{3816, "unimpl. make_memory_entry"},
	{3817, "unimpl. vm_map_page_query"},
	{3818, "unimpl. vm_region_info"},
	{3819, "unimpl. vm_mapped_pages_info"},
	{3820, "unimpl. vm_region_object_create"},
	{3821, "unimpl. vm_region_recurse"},
	{3822, "unimpl. vm_region_recurse_64"},
	{3823, "unimpl. vm_region_info_64"},
	{3824, "vm_region_64"},
	{3825, "make_memory_entry_64"},
	{3826, "unimpl. vm_map_64"},
	{3827, "unimpl. vm_map_get_upl"},
	{3828, "unimpl. vm_upl_map"},
	{3829, "unimpl. vm_upl_unmap"},
	{4000, "unimpl. processor_set_statistics"},
	{4001, "unimpl. processor_set_destroy"},
	{4002, "unimpl. processor_set_max_priority"},
	{4003, "unimpl. processor_set_policy_enable"},
	{4004, "unimpl. processor_set_policy_disable"},
	{4005, "unimpl. processor_set_tasks"},
	{4006, "unimpl. processor_set_threads"},
	{4007, "unimpl. processor_set_policy_control"},
	{4008, "unimpl. processor_set_stack_usage"},
	{4009, "unimpl. processor_set_info"},
	{5000, "unimpl. ledger_create"},
	{5001, "unimpl. ledger_terminate"},
	{5002, "unimpl. ledger_transfer"},
	{5003, "unimpl. ledger_read"},
	{6000, "unimpl. und_execute_rpc"},
	{6001, "unimpl. und_display_notice_from_bundle_rpc"},
	{6002, "unimpl. und_display_alert_from_bundle_rpc"},
	{6003, "unimpl. und_display_custom_from_bundle_rpc"},
	{6004, "unimpl. und_display_custom_from_dictionary_rpc"},
	{6005, "unimpl. und_cancel_notification_rpc"},
	{6006, "unimpl. und_display_notice_simple_rpc"},
	{6007, "unimpl. und_display_alert_simple_rpc"},
	{6200, "unimpl. und_alert_completed_with_result_rpc"},
	{6201, "unimpl. und_notification_created_rpc("},
	{555001, "unimpl. task_set_child_node"},
	{555002, "unimpl. norma_node_self"},
	{555005, "unimpl. norma_task_clone"},
	{555006, "unimpl. norma_task_create"},
	{555007, "unimpl. norma_get_special_port"},
	{555008, "unimpl. norma_set_special_port"},
	{555009, "unimpl. norma_task_teleport"},
	{555012, "unimpl. norma_port_location_hint"},
	{617000, "unimpl. lock_acquire"},
	{617001, "unimpl. lock_release"},
	{617002, "unimpl. lock_try"},
	{617003, "unimpl. lock_make_stable"},
	{617004, "unimpl. lock_handoff"},
	{617005, "unimpl. lock_handoff_accept"},
	{617005, "unimpl. lock_set_create"},
	{617006, "unimpl. lock_set_destroy"},
	{617007, "unimpl. lock_acquire"},
	{617008, "unimpl. lock_release"},
	{617009, "unimpl. lock_try"},
	{617010, "unimpl. lock_make_stable"},
	{617011, "unimpl. lock_handoff"},
	{617012, "unimpl. lock_handoff_accept"},
	{617200, "unimpl. semaphore_signal"},
	{617201, "unimpl. semaphore_signal_all"},
	{617202, "unimpl. semaphore_wait"},
	{617203, "unimpl. semaphore_signal_thread"},
	{617204, "unimpl. semaphore_timedwait"},
	{617205, "unimpl. semaphore_wait_signal"},
	{617206, "unimpl. semaphore_timedwait_signal"},
	{0, NULL}
};
