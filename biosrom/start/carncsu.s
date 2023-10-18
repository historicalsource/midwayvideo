	include	"csu.s"

	ALIGN8
bios_start
	incbin	"/video/biosrom/bios/carnbios.bin"
bios_end

	ALIGN8
post_start
	incbin	"/video/biosrom/bios/carntest.bin"
post_end
