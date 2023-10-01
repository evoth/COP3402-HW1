	# $Id: vm_test0.asm,v 1.1 2023/09/18 03:32:18 leavens Exp $
	.text start
start:	STRA
	ADDI $0, $t0, 32768
	ADDI $0, $t1, 4
	MUL $t0, $t0
	MFLO $t0
	MUL $t0, $t1
	EXIT
	.data 1024
	.stack 4096
	.end
	
