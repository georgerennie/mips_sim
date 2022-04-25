setup:
	addiu $t0, $zero, 0     # n
	addiu $t1, $zero, 0     # n^2
	addiu $t2, $zero, 0     # store address
	addiu $t3, $zero, 201   # branch value

loop:
	sh    $t1, 0($t2)       # *store = n^2
	addu  $t1, $t1, $t0     # n^2 += n
	addu  $t1, $t1, $t0     # n^2 += n
	addiu $t1, $t1, 1       # n^2 += 1

	addiu $t0, $t0, 1       # n++
	addiu $t2, $t2, 4       # store += 4
	bne   $t0, $t3, loop    # while n != 201
