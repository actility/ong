
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "board.h"


void Board_Put(board_t *board, char *msg, ...) {
	va_list listArg;
	va_start(listArg,msg);
	vsnprintf (board->line[board->w].str, BOARD_LINELEN, msg, listArg);
	va_end(listArg);

	board->line[board->w].str[BOARD_LINELEN] = 0;
	board->w ++;
	if	(board->w == BOARD_MAXLINE)
		board->w = 0;
}

void Board_Dump(board_t *board, void (*fct)(char *msg)) {
	int i;
	for	(i=board->w; i<BOARD_MAXLINE; i++) {
		if	(*board->line[i].str)
			(*fct)(board->line[i].str);
	}
	for	(i=0; i<board->w; i++) {
		if	(*board->line[i].str)
			(*fct)(board->line[i].str);
	}
}

void Board_Init(board_t *board) {
	memset (board, 0, sizeof(*board));
}

