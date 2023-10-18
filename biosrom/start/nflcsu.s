	include	"csu.s"

	ALIGN8
bios_start
	incbin	"/video/biosrom/bios/nflbios.bin"
bios_end

	ALIGN8
post_start
	incbin	"/video/biosrom/bios/nfltest.bin"
post_end
