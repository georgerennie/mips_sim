# vim: ft=mips

setup:
	addiu $t0, $zero, 0     # $t0 is counter incrementing by 2
	addiu $t2, $zero, 400   # $t2 is branch comparison value (200 * 2 = 400)
	j store                 # $t1 already contains current square so jump to store
	addiu $t1, $zero, 0     # $t1 is square value

calc_next:
	# Increment square ($t1) by 2n + 1 where n^2 is previous square
	# n = $t0 / 2
	# Therefore increment square ($t1) by $t0 + 1
	addu $t1, $t1, $t0

	addiu $t0, $t0, 2       # Increment counter by half word size (2)

	# Placed after $t0 operation so that the following bne doesnt have to wait
	# for $t0 to be updated
	addiu $t1, $t1, 1

store:
	bne $t0, $t2, calc_next
	sh $t1, 0($t0)          # Store in array of half words starting at address 0
