* 6809 assembly program generated by cmoc 0.1.85


	SECTION	code


_dicSize	IMPORT
_dicBitSize	IMPORT
_aheadSize	IMPORT
_aheadBitSize	IMPORT
_dicBitSize	EXPORT
_dicSize	EXPORT
_aheadBitSize	EXPORT
_aheadSize	EXPORT
___va_arg	IMPORT
_abs	IMPORT
_adddww	IMPORT
_atoi	IMPORT
_atol	IMPORT
_atoui	IMPORT
_atoul	IMPORT
_bruteForceSearchOptim	IMPORT
_bsearch	IMPORT
_cmpdww	IMPORT
_delay	IMPORT
_divdwb	IMPORT
_divdww	IMPORT
_divmod16	IMPORT
_divmod8	IMPORT
_dwtoa	IMPORT
_enableCMOCFloatSupport	IMPORT
_exit	IMPORT
_isalnum	IMPORT
_isalpha	IMPORT
_isdigit	IMPORT
_isspace	IMPORT
_itoa10	IMPORT
_labs	IMPORT
_ltoa10	IMPORT
_memchr	IMPORT
_memcmp	IMPORT
_memcpy	IMPORT
_memichr	IMPORT
_memicmp	IMPORT
_memmove	IMPORT
_memset	IMPORT
_memset16	IMPORT
_mulwb	IMPORT
_mulww	IMPORT
_printf	IMPORT
_putchar	IMPORT
_putstr	IMPORT
_qsort	IMPORT
_rand	IMPORT
_readline	IMPORT
_readword	IMPORT
_sbrk	IMPORT
_sbrkmax	IMPORT
_setConsoleOutHook	IMPORT
_set_null_ptr_handler	IMPORT
_set_stack_overflow_handler	IMPORT
_sprintf	IMPORT
_sqrt16	IMPORT
_sqrt32	IMPORT
_srand	IMPORT
_strcat	IMPORT
_strchr	IMPORT
_strcmp	IMPORT
_strcpy	IMPORT
_stricmp	IMPORT
_strlen	IMPORT
_strlwr	IMPORT
_strncmp	IMPORT
_strncpy	IMPORT
_strstr	IMPORT
_strtol	IMPORT
_strtoul	IMPORT
_strupr	IMPORT
_subdww	IMPORT
_tolower	IMPORT
_toupper	IMPORT
_ultoa10	IMPORT
_utoa10	IMPORT
_vprintf	IMPORT
_vsprintf	IMPORT
_zerodw	IMPORT
_initDefaultParameters	EXPORT


*******************************************************************************

* FUNCTION initDefaultParameters(): defined at lz77dec.c:11
_initDefaultParameters	EQU	*
* Calling convention: Default
* Line lz77dec.c:13: assignment: =
	LDD	#$0FFF		decimal 4095 signed
	STD	_dicSize+0,PCR
* Line lz77dec.c:14: assignment: =
	LDB	#$0C
	STB	_dicBitSize+0,PCR	variable dicBitSize
* Line lz77dec.c:15: assignment: =
	CLRA
	LDB	#$3F		decimal 63 signed
	STD	_aheadSize+0,PCR
* Line lz77dec.c:16: assignment: =
	LDB	#$06
	STB	_aheadBitSize+0,PCR	variable aheadBitSize
* Useless label L00078 removed
	RTS
* END FUNCTION initDefaultParameters(): defined at lz77dec.c:11
funcend_initDefaultParameters	EQU *
funcsize_initDefaultParameters	EQU	funcend_initDefaultParameters-_initDefaultParameters
_initParameters	EXPORT


*******************************************************************************

* FUNCTION initParameters(): defined at lz77dec.c:19
_initParameters	EQU	*
* Calling convention: Default
	PSHS	U
	LEAU	,S
* Formal parameter(s):
*      4,U:    2 bytes: dsz: unsigned int
*      7,U:    1 byte : bdsz: unsigned char
*      8,U:    2 bytes: asz: unsigned int
*     11,U:    1 byte : basz: unsigned char
* Line lz77dec.c:21: assignment: =
* optim: stripConsecutiveLoadsToSameReg
	LDD	4,U
	STD	_dicSize+0,PCR
* Line lz77dec.c:22: assignment: =
	LDB	7,U		variable `bdsz', declared at lz77dec.c:19
	STB	_dicBitSize+0,PCR
* Line lz77dec.c:23: assignment: =
* optim: stripConsecutiveLoadsToSameReg
	LDD	8,U
	STD	_aheadSize+0,PCR
* Line lz77dec.c:24: assignment: =
	LDB	11,U		variable `basz', declared at lz77dec.c:19
	STB	_aheadBitSize+0,PCR
* Useless label L00079 removed
	LEAS	,U
	PULS	U,PC
* END FUNCTION initParameters(): defined at lz77dec.c:19
funcend_initParameters	EQU *
funcsize_initParameters	EQU	funcend_initParameters-_initParameters
_initBitField	EXPORT


*******************************************************************************

* FUNCTION initBitField(): defined at lz77dec.c:27
_initBitField	EQU	*
* Calling convention: Default
	PSHS	U
	LEAU	,S
* Formal parameter(s):
*      4,U:    2 bytes: bf: struct bitFieldStruct *
*      6,U:    2 bytes: buf: unsigned char *
* Line lz77dec.c:29: assignment: =
	CLRA
	LDB	#$07		decimal 7 signed
* optim: stripExtraPushPullB
	LDX	4,U		variable bf
* optim: optimizeLeax
* optim: stripExtraPushPullB
	STB	4,X		optim: optimizeLeax
* Line lz77dec.c:30: assignment: =
* optim: stripExtraClrA_B
	CLRB
* optim: stripUselessPushPull
* optim: removeRepeatedLDX
* optim: optimizeLeax
* optim: stripUselessPushPull
	STD	2,X		optim: optimizeLeax
* Line lz77dec.c:31: assignment: =
	LDD	6,U		variable `buf', declared at lz77dec.c:27
* optim: stripUselessPushPull
* optim: optimizeLdx
* optim: stripUselessPushPull
	STD	[4,U]		optim: optimizeLdx
* Useless label L00080 removed
	LEAS	,U
	PULS	U,PC
* END FUNCTION initBitField(): defined at lz77dec.c:27
funcend_initBitField	EQU *
funcsize_initBitField	EQU	funcend_initBitField-_initBitField
_readbits	EXPORT


*******************************************************************************

* FUNCTION readbits(): defined at lz77dec.c:35
_readbits	EQU	*
* Calling convention: Default
	PSHS	U
	LEAU	,S
	LEAS	-6,S
* Formal parameter(s):
*      4,U:    2 bytes: bf: struct bitFieldStruct *
*      7,U:    1 byte : bitCount: unsigned char
* Local non-static variable(s):
*     -6,U:    1 byte : mask: unsigned char
*     -5,U:    2 bytes: value: unsigned int
*     -3,U:    2 bytes: currentBit: unsigned int
*     -1,U:    1 byte : bitSet: unsigned char
* Line lz77dec.c:38: init of variable mask
	CLR	-6,U		variable mask
* Line lz77dec.c:39: init of variable value
	CLRA
	CLRB
	STD	-5,U		variable value
* Line lz77dec.c:40: init of variable currentBit
* optim: stripExtraClrA_B
* optim: stripExtraClrA_B
	STD	-3,U		variable currentBit
* Line lz77dec.c:41: init of variable bitSet
	CLR	-1,U		variable bitSet
* Line lz77dec.c:42: while
	LBRA	L00084		jump to while condition
L00083	EQU	*		while body
* Line lz77dec.c:43: assignment: =
	CLRA
	LDB	#$01		decimal 1 signed
	PSHS	B,A		left side of shift: used and popped by shiftLeft
	LDX	4,U		variable bf
	LDB	4,X		member bitLeft of bitFieldStruct
* optim: stripExtraClrA_B
	LBSR	shiftLeft
* Cast from `int' to byte: result already in B
	STB	-6,U
* Line lz77dec.c:44: assignment: =
* optim: storeLoad
	PSHS	B
	LDX	4,U		variable bf
	LDD	2,X		member currentIndex of bitFieldStruct
	PSHS	B,A		preserve array index
	LDX	4,U		variable bf
	LDD	,X		member buffer of bitFieldStruct
	ADDD	,S++		add stacked array index to array address in D
	TFR	D,X		put resulting address in X
	LDB	,X		get r-value
	ANDB	,S+
	PSHS	B		left side of shift: used and popped by shiftByteRightUnsigned
	LDX	4,U		variable bf
	LDB	4,X		member bitLeft of bitFieldStruct
	CLRA
	LBSR	shiftByteRightUnsigned
	CLRA
	STD	-3,U
* Line lz77dec.c:45: assignment: |=
* optim: storeLoad
	PSHS	B,A		left side of shift: used and popped by shiftLeft
	LDB	-1,U		variable `bitSet', declared at lz77dec.c:41
* optim: stripExtraClrA_B
	LBSR	shiftLeft
	PSHS	B,A
	LEAX	-5,U
	LDD	,S++
	ORA	,X
	ORB	1,X
	STD	,X
* Line lz77dec.c:46: post-decrement
	DEC	7,U
* Line lz77dec.c:47: post-increment
	INC	-1,U
* Line lz77dec.c:48: post-decrement
	LDX	4,U		variable bf
* optim: optimizeLeax
	DEC	4,X		optim: optimizeLeax
* Line lz77dec.c:49: if
	LDX	4,U		variable bf
	LDB	4,X		member bitLeft of bitFieldStruct
	CMPB	#0
	BGE	L00087		 (optim: condBranchOverUncondBranch)
* optim: condBranchOverUncondBranch
* Useless label L00086 removed
* Line lz77dec.c:49
* Line lz77dec.c:50: assignment: =
	CLRA
* LDB #$07 optim: optimizeStackOperations1
* PSHS B optim: optimizeStackOperations1
	LDX	4,U		variable bf
* optim: optimizeLeax
	LDB	#7		optim: optimizeStackOperations1
	STB	4,X		optim: optimizeLeax
* Line lz77dec.c:51: post-increment
* optim: removeRepeatedLDX
	LEAX	2,X		member currentIndex of bitFieldStruct
	LDD	,X
	ADDD	#1
	STD	,X
L00087	EQU	*		else clause of if() started at lz77dec.c:49
* Useless label L00088 removed
L00084	EQU	*		while condition at lz77dec.c:42
	LDB	7,U		variable `bitCount', declared at lz77dec.c:35
* optim: loadCmpZeroBeqOrBne
	LBNE	L00083
* optim: branchToNextLocation
* Useless label L00085 removed
* Line lz77dec.c:54: return with value
	LDD	-5,U		variable `value', declared at lz77dec.c:39
* optim: branchToNextLocation
* Useless label L00081 removed
	LEAS	,U
	PULS	U,PC
* END FUNCTION readbits(): defined at lz77dec.c:35
funcend_readbits	EQU *
funcsize_readbits	EQU	funcend_readbits-_readbits
_uncompress	EXPORT


*******************************************************************************

* FUNCTION uncompress(): defined at lz77dec.c:72
_uncompress	EQU	*
* Calling convention: Default
	PSHS	U
	LEAU	,S
	LEAS	-12,S
* Formal parameter(s):
*      4,U:    2 bytes: input: unsigned char *
*      6,U:    2 bytes: iSize: unsigned int
*      8,U:    2 bytes: output: unsigned char *
*     10,U:    2 bytes: oSize: unsigned int
* Local non-static variable(s):
*    -12,U:    5 bytes: t: struct tupleStruct
*     -7,U:    5 bytes: bf: struct bitFieldStruct
*     -2,U:    2 bytes: outputIdx: int
* Line lz77dec.c:75: function call: initBitField()
	LDD	4,U		variable `input', declared at lz77dec.c:72
	PSHS	B,A		argument 2 of initBitField(): unsigned char *
	LEAX	-7,U		variable `bf', declared at lz77dec.c:74
	PSHS	X		argument 1 of initBitField(): struct bitFieldStruct *
	LBSR	_initBitField
	LEAS	4,S
* Line lz77dec.c:76: init of variable outputIdx
	CLRA
	CLRB
	STD	-2,U		variable outputIdx
* Line lz77dec.c:79: while
	LBRA	L00090		jump to while condition
L00089	EQU	*		while body
* Line lz77dec.c:82: assignment: =
* Line lz77dec.c:82: function call: readbits()
	LDB	_dicBitSize+0,PCR	variable `dicBitSize', declared at lz77dec.c:6
	CLRA			promoting byte argument to word
	PSHS	B,A		argument 2 of readbits(): unsigned char
	LEAX	-7,U		variable `bf', declared at lz77dec.c:74
	PSHS	X		argument 1 of readbits(): struct bitFieldStruct *
	LBSR	_readbits
	LEAS	4,S
* optim: stripUselessPushPull
* optim: optimizeLeax
* optim: stripUselessPushPull
	STD	-12,U		optim: optimizeLeax
* Line lz77dec.c:83: assignment: =
* Line lz77dec.c:83: function call: readbits()
	LDB	_aheadBitSize+0,PCR	variable `aheadBitSize', declared at lz77dec.c:8
	CLRA			promoting byte argument to word
	PSHS	B,A		argument 2 of readbits(): unsigned char
	LEAX	-7,U		variable `bf', declared at lz77dec.c:74
	PSHS	X		argument 1 of readbits(): struct bitFieldStruct *
	LBSR	_readbits
	LEAS	4,S
* optim: stripUselessPushPull
* optim: optimizeLeax
* optim: stripUselessPushPull
	STD	-10,U		optim: optimizeLeax
* Line lz77dec.c:84: assignment: =
* Line lz77dec.c:84: function call: readbits()
	CLRA
	LDB	#$08		decimal 8 signed
	PSHS	B,A		argument 2 of readbits(): int
	LEAX	-7,U		variable `bf', declared at lz77dec.c:74
	PSHS	X		argument 1 of readbits(): struct bitFieldStruct *
	LBSR	_readbits
	LEAS	4,S
* Cast from `unsigned int' to byte: result already in B
* optim: stripExtraPushPullB
* optim: optimizeLeax
* optim: stripExtraPushPullB
	STB	-8,U		optim: optimizeLeax
* Line lz77dec.c:86: if
	LDD	-12,U		member d of tupleStruct, via variable t
* optim: loadCmpZeroBeqOrBne
	BEQ	L00093		 (optim: condBranchOverUncondBranch)
* optim: condBranchOverUncondBranch
* Useless label L00092 removed
* Line lz77dec.c:86
* Line lz77dec.c:87: function call: memcpy()
	LDD	-10,U		member l of tupleStruct, via variable t
	PSHS	B,A		argument 3 of memcpy(): unsigned int
* optim: optimizeStackOperations4
* optim: optimizeStackOperations4
* optim: optimizeStackOperations4
* optim: optimizeStackOperations4
	LDD	8,U		variable `output', declared at lz77dec.c:72
	ADDD	-2,U		optim: optimizeStackOperations4
	SUBD	-12,U		optim: optimizeStackOperations4
	PSHS	B,A		argument 2 of memcpy(): unsigned char *
* optim: optimizeStackOperations4
* optim: optimizeStackOperations4
	LDD	8,U		variable `output', declared at lz77dec.c:72
	ADDD	-2,U		optim: optimizeStackOperations4
	PSHS	B,A		argument 1 of memcpy(): unsigned char *
	LBSR	_memcpy
	LEAS	6,S
* Line lz77dec.c:88: assignment: +=
* optim: optimizeStackOperations4
* optim: optimizeStackOperations4
	LDD	-2,U
	ADDD	-10,U		optim: optimizeStackOperations4
	STD	-2,U
L00093	EQU	*		else clause of if() started at lz77dec.c:86
* Useless label L00094 removed
* Line lz77dec.c:91: assignment: =
	LDB	-8,U		member c of tupleStruct, via variable t
	PSHS	B
	LDD	-2,U		variable `outputIdx', declared at lz77dec.c:91
	ADDD	#1
	STD	-2,U
	SUBD	#1		post increment yields initial value
	LDX	8,U		pointer output
	LEAX	D,X		add offset
	LDB	,S+
	STB	,X
L00090	EQU	*		while condition at lz77dec.c:79
* optim: optimizeStackOperations4
* optim: optimizeStackOperations4
	LDD	-5,U		member currentIndex of bitFieldStruct, via variable bf
	CMPD	6,U		optim: optimizeStackOperations4
	LBLO	L00089
* optim: branchToNextLocation
* Useless label L00091 removed
* Line lz77dec.c:93: return with value
	LDD	-2,U		variable `outputIdx', declared at lz77dec.c:76
* optim: branchToNextLocation
* Useless label L00082 removed
	LEAS	,U
	PULS	U,PC
* END FUNCTION uncompress(): defined at lz77dec.c:72
funcend_uncompress	EQU *
funcsize_uncompress	EQU	funcend_uncompress-_uncompress


	ENDSECTION




	SECTION	initgl




*******************************************************************************

* Initialize global variables.


	ENDSECTION




	SECTION	rodata


string_literals_start	EQU	*
string_literals_end	EQU	*


*******************************************************************************

* READ-ONLY GLOBAL VARIABLES


	ENDSECTION




	SECTION	rwdata


* Statically-initialized global variables
* Statically-initialized local static variables


	ENDSECTION




	SECTION	bss


bss_start	EQU	*
* Uninitialized global variables
_dicBitSize	EQU	*
	RMB	1		dicBitSize
_dicSize	EQU	*
	RMB	2		dicSize
_aheadBitSize	EQU	*
	RMB	1		aheadBitSize
_aheadSize	EQU	*
	RMB	2		aheadSize
* Uninitialized local static variables
bss_end	EQU	*


	ENDSECTION




*******************************************************************************

* Importing 3 utility routine(s).
_memcpy	IMPORT
shiftByteRightUnsigned	IMPORT
shiftLeft	IMPORT


*******************************************************************************

	END
