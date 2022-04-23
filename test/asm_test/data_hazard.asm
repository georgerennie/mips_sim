# vim: ft=mips
main:
	addiu $t0, $zero, 1024
	sh    $t0, 10
	lhu   $t2, 10
	addiu $t2, $t2, 1
	addiu $t2, $t2, 2
	addiu $t2, $t2, 4
