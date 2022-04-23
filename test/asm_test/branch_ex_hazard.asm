# vim: ft=mips
main:
	addiu $t0, $zero, 1
	addiu $t1, $zero, 1
	beq   $t0, $t1 true

false:
	addiu $a2, $zero, 1
	j exit

true:
	addiu $a3, $zero, 1

exit:
