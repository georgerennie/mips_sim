# vim: ft=mips
main:
	addiu $t0, $zero, 1
	addiu $t1, $zero, 2
	sh    $t0, 10

	lhu    $t1, 10
	beq   $t0, $t1 true

false:
	addiu $a2, $zero, 1
	j exit

true:
	addiu $a3, $zero, 1

exit:
