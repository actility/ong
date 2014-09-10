
#ifndef _BOARD_H_
#define _BOARD_H_

#define BOARD_MAXLINE 50
#define BOARD_LINELEN 200

typedef struct {
	char str[BOARD_LINELEN+1];
} board_line_t;

typedef struct {
	int w;
	board_line_t	line[BOARD_MAXLINE];
} board_t;

void Board_Put(board_t *board, char *msg, ...);
void Board_Dump(board_t *board, void (*fct)(char *msg));
void Board_Init(board_t *board);

#endif /* _BOARD_H_ */
