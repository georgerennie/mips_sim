# vim: ft=mips

setup:
	addiu $t0, $zero, 0     # $t0 is counter incrementing by 2
	addiu $t1, $zero, 0     # $t1 is square value
	addiu $t2, $zero, 400   # $t2 is branch comparison value (200 * 2 = 400)
	j     store             # $t1 already contains current square so jump to store

loop:
	# Increment square ($t1) by 2n + 1 where n^2 is previous square
	# n = $t0 / 2
	# Therefore increment square ($t1) by $t0 + 1
	addu  $t1, $t1, $t0
	addiu $t1, $t1, 1

	addiu $t0, $t0, 2       # Increment counter by half word size (2)

store:
	sh    $t1, 0($t0)       # Store in array of half words starting at address 0
	bne   $t0, $t2, loop
