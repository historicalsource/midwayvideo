	include	"csu.s"

	ALIGN8
bios_start
	incbin	"/video/biosrom/bios/b99bios.bin"
bios_end

	ALIGN8
post_start
	incbin	"/video/biosrom/bios/b99test.bin"
post_end
