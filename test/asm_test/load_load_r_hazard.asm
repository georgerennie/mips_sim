# vim: ft=mips
	addiu $t0, $zero, 5
	sh $t0, 2
	addiu $t0, $zero, 9
	sh $t0, 10

# Flush pipeline
	nop
	nop
	nop
	nop
	lhu $fp, 10
	lhu $ra, 2
	or $s6, $fp, $ra
