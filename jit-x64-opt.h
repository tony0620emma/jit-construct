/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.3.0, DynASM x64 version 1.3.0
** DO NOT EDIT! The original file is in "jit-x64-opt.dasc".
*/

#line 1 "jit-x64-opt.dasc"
#include <stdint.h>
#include <stdio.h>
#include "util.h"
#include "bf-lex.h"

//|.arch x64
#if DASM_VERSION != 10300
#error "Version mismatch between DynASM and included encoding engine"
#endif
#line 7 "jit-x64-opt.dasc"
//|.actionlist actions
static const unsigned char actions[59] = {
  83,72,137,252,251,255,198,3,0,255,72,129,195,239,255,128,3,235,255,15,182,
  59,72,184,237,237,252,255,208,255,72,184,237,237,252,255,208,136,3,255,128,
  59,0,15,132,245,249,255,128,59,0,15,133,245,249,255,91,195,255
};

#line 8 "jit-x64-opt.dasc"
//|
//|// Use rbx as our cell pointer.
//|// Since rbx is a callee-save register, it will be preserved
//|// across our calls to getchar and putchar.
//|.define PTR, rbx
//|
//|// Macro for calling a function.
//|// In cases where our target is <=2**32 away we can use
//|//   | call &addr
//|// But since we don't know if it will be, we use this safe
//|// sequence instead.
//|.macro callp, addr
//|  mov64  rax, (uintptr_t)addr
//|  call   rax
//|.endmacro

#define Dst &state
#define MAX_NESTING 256

int main(int argc, char *argv[])
{
	// For lexical analyzer
	extern int yylex();
	extern int yyleng;
	extern FILE *yyin;

	if (argc < 2) err("Usage: jit-x64-opt <inputfile>");
	dasm_State *state;
	initjit(&state, actions);

	unsigned int maxpc = 0;
	int pcstack[MAX_NESTING];
	int *top = pcstack, *limit = pcstack + MAX_NESTING;
	// Function prologue.
	//|  push PTR
	//|  mov  PTR, rdi      // rdi store 1st argument
	dasm_put(Dst, 0);
#line 44 "jit-x64-opt.dasc"
	
	int type;
	for (yyin = fopen(argv[1], "r"), type = yylex(); type != END_OF_FILE; type = yylex()) {
		switch (type) {
		case ZERO:
			//|  mov byte [PTR], 0
			dasm_put(Dst, 6);
#line 50 "jit-x64-opt.dasc"
			break;
		case MOV_R:
			//|  add PTR, yyleng
			dasm_put(Dst, 10, yyleng);
#line 53 "jit-x64-opt.dasc"
			break;
		case MOV_L:
			//|  add  PTR, -(yyleng) 
			dasm_put(Dst, 10, -(yyleng));
#line 56 "jit-x64-opt.dasc"
			break;
		case ADD:
			//|  add byte [PTR], yyleng
			dasm_put(Dst, 15, yyleng);
#line 59 "jit-x64-opt.dasc"
			break;
		case SUB:
			//|  add byte [PTR], -(yyleng)
			dasm_put(Dst, 15, -(yyleng));
#line 62 "jit-x64-opt.dasc"
			break;
		case OUTPUT:
			//|  movzx edi, byte [PTR]
			//|  callp putchar
			dasm_put(Dst, 19, (unsigned int)((uintptr_t)putchar), (unsigned int)(((uintptr_t)putchar)>>32));
#line 66 "jit-x64-opt.dasc"
			break;
		case INPUT:
			//|  callp getchar
			//|  mov   byte [PTR], al
			dasm_put(Dst, 30, (unsigned int)((uintptr_t)getchar), (unsigned int)(((uintptr_t)getchar)>>32));
#line 70 "jit-x64-opt.dasc"
			break;
		case BRACE_L:
			if (top == limit) err("Nesting too deep.");
			// Each loop gets two pclabels: at the beginning and end.
			// We store pclabel offsets in a stack to link the loop
			// begin and end together.
			maxpc += 2;
			*top++ = maxpc;
			dasm_growpc(&state, maxpc);
			//|  cmp  byte [PTR], 0
			//|  je   =>(maxpc-2)
			//|=>(maxpc-1):
			dasm_put(Dst, 40, (maxpc-2), (maxpc-1));
#line 82 "jit-x64-opt.dasc"
			break;
		case BRACE_R:
			if (top == pcstack) err("Unmatched ']'");
			top--;
			//|  cmp  byte [PTR], 0
			//|  jne  =>(*top-1)
			//|=>(*top-2):
			dasm_put(Dst, 48, (*top-1), (*top-2));
#line 89 "jit-x64-opt.dasc"
			break;
		}
	}

	// Function epilogue.
	//|  pop  PTR
	//|  ret
	dasm_put(Dst, 56);
#line 96 "jit-x64-opt.dasc"

	void (*fptr)(char*) = jitcode(&state);
	char *mem = calloc(30000, 1);
	fptr(mem);
	free(mem);
	free_jitcode(fptr);
	return 0;
}

