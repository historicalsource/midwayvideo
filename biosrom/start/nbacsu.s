	include	"csu.s"

	ALIGN8
bios_start
	incbin	"/video/biosrom/bios/nbabios.bin"
bios_end

	ALIGN8
post_start
	incbin	"/video/biosrom/bios/nbatest.bin"
post_end
