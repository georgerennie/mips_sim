# vim: ft=mips
main:
	addiu $t0, $zero, 1
	j exit
	addiu $t1, $zero, 1
	addiu $t2, $zero, 1

exit:
