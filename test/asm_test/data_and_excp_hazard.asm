# vim: ft=mips
main:
	addiu $t0, $zero, 5
	sh $t0, 0
	lhu $at, 0
	addiu $t0, $at, 1

