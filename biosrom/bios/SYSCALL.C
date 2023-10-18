//
// syscall.c - System call handler for VEGAS
//
// Copyright (c) 1998 by Midway Video Inc.
//
// $Revision: 4 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<ioctl.h>


#ifndef NULL
#define	NULL	0
#endif

//
// External functions
//
extern int _close();
extern int _commit();
extern int _creat();
extern int _creatnew();
extern int _findfirst();
extern int _findnext();
extern int _getdate();
extern int _getdiskfree();
extern int _getdrive();
extern int _getfileattr();
extern int _getftime();
extern int _gettime();
extern int _lock();
extern int _open();
extern int _read();
extern int _setdate();
extern int _setdrive();
extern int _setfileattr();
extern int _setftime();
extern int _unlock();
extern int _write();
extern int _ioctl();
extern int _get_errno();
extern int _exit();
extern int isatty();
extern int flush_data_cache();
extern int pciGetConfigData();
extern int pciSetConfigData();
extern int pciFindCardMulti();
extern int pciMapCardMulti();
extern int ide_set_partition();
extern int ide_get_partition_size();
extern int ide_get_partition_table();
extern int ide_identify();
extern int ide_get_hdinfo();
extern int SecReads();
extern int SecWrites();
extern int FSFormat();
extern int _SecReads();
extern int _SecWrites();
extern int get_ide_ident_info();
extern int install_driver();
extern int uninstall_driver();
extern int _settime();
extern int _remove();
extern int exec();
extern int get_platform_id();
extern int get_arg_ptr();
extern int reinitialize_disk_cache();
extern int _rename();
extern int _lseek();
extern int get_rom_version();
extern int is_revision_a();
extern int get_pci_config_reg();
extern int put_pci_config_reg();
extern int set_handler();
extern int wait_vsync();
extern int get_elapsed_time();
extern int config_cmos();
extern int get_cmos_user_address();
extern int get_cmos_user_size();
extern int check_audit();
extern int check_audits();
extern int clear_audits();
extern int get_audit();
extern int set_audit();
extern int increment_audit();
extern int check_adjustments();
extern int get_adjustment();
extern int set_adjustment();
extern int get_generic_record();
extern int set_generic_record();
extern int crc();
extern int get_timer_val();
extern int get_cmos_dump_record();
extern int init_cmos_dump_record();
extern int snd_reset();
extern int snd_reset_ack();
extern int snd_get_data();
extern int snd_get_debug();
extern int snd_send_data();
extern int snd_send_data_realtime();
extern int snd_dump_port();
extern int snd_send_command();
extern int snd_delay();
extern int snd_clear_latch();
extern int get_snd_status();
extern int get_snd_data();
extern int set_snd_data();
extern int enable_write_merge();
extern int disable_pstall_detect();
extern int install_disk_info();
extern int read_bios_flags();
extern int write_bios_flags();
extern int get_inputs();
extern int disk_queue_empty();



//
// Table of system call functions
//
int	(*syscall_table[])() = {
#ifndef TEST
_close,
_commit,
_creat,
_creatnew,
_findfirst,
_findnext,
_getdate,
_getdiskfree,
_getdrive,
_getfileattr,
_getftime,
_gettime,
_lock,
_open,
_read,
_setdate,
_setdrive,
_setfileattr,
_setftime,
_unlock,
_write,
_ioctl,
_get_errno,
_exit,
isatty,
flush_data_cache,
pciGetConfigData,
pciSetConfigData,
pciFindCardMulti,
pciMapCardMulti,
ide_set_partition,
ide_get_partition_size,
ide_get_partition_table,
ide_identify,
ide_get_hdinfo,
SecReads,
SecWrites,
FSFormat,
_SecReads,
_SecWrites,
get_ide_ident_info,
install_driver,
uninstall_driver,
_settime,
_remove,
exec,
get_platform_id,
get_arg_ptr,
reinitialize_disk_cache,
_rename,
_lseek,
get_rom_version,
is_revision_a,
set_handler,
get_pci_config_reg,
put_pci_config_reg,
wait_vsync,
get_elapsed_time,
config_cmos,
get_cmos_user_address,
get_cmos_user_size,
check_audit,
check_audits,
clear_audits,
get_audit,
set_audit,
increment_audit,
check_adjustments,
get_adjustment,
set_adjustment,
get_generic_record,
set_generic_record,
crc,
get_timer_val,
get_cmos_dump_record,
init_cmos_dump_record,
snd_reset,
snd_reset_ack,
snd_get_data,
snd_get_debug,
snd_send_data,
snd_send_data_realtime,
snd_dump_port,
snd_send_command,
snd_delay,
snd_clear_latch,
get_snd_status,
get_snd_data,
set_snd_data,
enable_write_merge,
disable_pstall_detect,
install_disk_info,
read_bios_flags,
write_bios_flags,
get_inputs,
disk_queue_empty
#else
NULL
#endif
};
