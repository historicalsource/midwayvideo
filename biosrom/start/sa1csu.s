	include	"csu.s"

	ALIGN8
bios_start
	incbin	"/video/biosrom/bios/sa1bios.bin"
bios_end
