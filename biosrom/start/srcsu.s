	include	"csu.s"

	ALIGN8
bios_start
	incbin	"/video/biosrom/bios/srbios.bin"
bios_end

	ALIGN8
post_start
	incbin	"/video/biosrom/bios/srtest.bin"
post_end
