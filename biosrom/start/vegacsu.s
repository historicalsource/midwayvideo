	include	"csu.s"

	ALIGN8
bios_start
	incbin	"/video/biosrom/bios/vegabios.bin"
bios_end

	ALIGN8
post_start
	incbin	"/video/biosrom/bios/vegatest.bin"
post_end
