;
; jtable.inc
;
; This is the BIOS system jump table - This table is used to build both the
; BIOS rom and the standard C library.  When used for building the BIOS rom
; the LIBC value is set to 0 and when used for building the standard C library
; the LIBC valus is set to 1.  This file get included by the crt0.s file for
; the BIOS rom and the crt0.s file for the standard C library.
;
; Any changes that occur in this file affect both the BIOS rom and the
; standard C library.
;
; $Revision: $
;
; $Author: $
;
	if	LIBC=0
	inform	0,"Jump table for BIOS rom"
	else
	inform	0,"Jump table for standard C library"
	endif

	if	LIBC=0
JTAB_ENTRY	macro	func_name
	xref	\func_name
	j	\func_name
	nop
	endm
	else
JTAB_ENTRY	macro	func_name
	xdef	\func_name
\func_name
	nop
	nop
	endm
	endif

	if	LIBC=0
UNUSED_JTAB_ENTRY	macro
	j		not_implem
	nop
	endm
	else
UNUSED_JTAB_ENTRY	macro
	nop
	nop
	endm
	endif

	; System Jump Table
	xdef	sys_jtable
	xdef	sys_jt_end

sys_jtable:
	if	~(PHOENIX_SYS=VEGAS_SYSTEM)
	JTAB_ENTRY	tlb_exc
	UNUSED_JTAB_ENTRY	; install_user_vector
	UNUSED_JTAB_ENTRY	; install_debug_hook
	JTAB_ENTRY	_close
	JTAB_ENTRY	_commit
	JTAB_ENTRY	_creat
	JTAB_ENTRY	_creatnew
	JTAB_ENTRY	_findfirst
	JTAB_ENTRY	_findnext
	JTAB_ENTRY	_getdate
	JTAB_ENTRY	_getdiskfree
	JTAB_ENTRY	_getdrive
	JTAB_ENTRY	_getfileattr
	JTAB_ENTRY	_getftime
	JTAB_ENTRY	_gettime
	JTAB_ENTRY	_lock
	JTAB_ENTRY	xtlb_exc
	JTAB_ENTRY	_open
	JTAB_ENTRY	_read
	JTAB_ENTRY	_setdate
	JTAB_ENTRY	_setdrive
	JTAB_ENTRY	_setfileattr
	JTAB_ENTRY	_setftime
	JTAB_ENTRY	_unlock
	JTAB_ENTRY	_write
	JTAB_ENTRY	_ioctl
	JTAB_ENTRY	_get_errno
	JTAB_ENTRY	_exit
	JTAB_ENTRY	isatty
	JTAB_ENTRY	flush_data_cache
	JTAB_ENTRY	pciGetConfigData
	JTAB_ENTRY	pciSetConfigData
	JTAB_ENTRY	cache_exc
	JTAB_ENTRY	pciFindCardMulti
	JTAB_ENTRY	pciMapCardMulti
	JTAB_ENTRY	ide_set_partition
	JTAB_ENTRY	ide_get_partition_size
	JTAB_ENTRY	ide_get_partition_table
	JTAB_ENTRY	ide_identify
	JTAB_ENTRY	ide_get_hdinfo
	JTAB_ENTRY	SecReads		; Takes into account partition
	JTAB_ENTRY	SecWrites		; Takes into account partition
	JTAB_ENTRY	FSFormat		; Partition number must be set
	JTAB_ENTRY	_SecReads		; Raw sector read
	JTAB_ENTRY	_SecWrites		; Raw sector write
	JTAB_ENTRY	get_ide_ident_info	; Get pointer to ide identify info
	JTAB_ENTRY	install_driver		; Install a driver
	JTAB_ENTRY	uninstall_driver	; Uninstall a driver

	; General Exception Handler
	if	LIBC=0
        JTAB_ENTRY      exception
	else
	UNUSED_JTAB_ENTRY
	endif
	JTAB_ENTRY	_settime
	JTAB_ENTRY	_remove
	JTAB_ENTRY	exec
	JTAB_ENTRY	get_platform_id
	JTAB_ENTRY	get_arg_ptr
	JTAB_ENTRY	reinitialize_disk_cache
	JTAB_ENTRY	_rename
	JTAB_ENTRY	_lseek
	JTAB_ENTRY	get_rom_version
	JTAB_ENTRY	is_revision_a
	JTAB_ENTRY	install_disk_callback
	JTAB_ENTRY	install_sys_handler
	JTAB_ENTRY	install_fpu_handler
	JTAB_ENTRY	install_int_div0_handler
	JTAB_ENTRY	install_int_divo_handler
	JTAB_ENTRY	install_interrupt_handler
	JTAB_ENTRY	install_tlb_handler
	JTAB_ENTRY	install_xtlb_handler
	JTAB_ENTRY	install_cache_error_handler
	JTAB_ENTRY	install_disk_info
	JTAB_ENTRY	flush_disk_cache
	JTAB_ENTRY	flush_disk_queue
	JTAB_ENTRY	install_disk_timeout_audit

	else
	; VEGAS System

	if	LIBC=0
	JTAB_ENTRY	TLBExceptionHandler
	else
	UNUSED_JTAB_ENTRY
	endif
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY

	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	if	LIBC=0
	JTAB_ENTRY	XTLBExceptionHandler
	else
	UNUSED_JTAB_ENTRY			; XTLBExceptionHandler
	endif
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	if	LIBC=0
	JTAB_ENTRY	CacheErrorExceptionHandler
	else
	UNUSED_JTAB_ENTRY	; CacheErrorExceptionHandler
	endif
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY
	UNUSED_JTAB_ENTRY

	; General Exception Handler
	if	LIBC=0
	JTAB_ENTRY	exception
	else
	UNUSED_JTAB_ENTRY
	endif
	endif
sys_jt_end:

