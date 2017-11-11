#ifndef _GOBANG_H_
#define _GOBANG_H_
#include <stdlib.h>
int table[15][15] = {0}; 
const char tablechar[][3] = {"  ", "()", "><"}; 
int lastx, lasty, count = 0;
void printtable() 
{
	int i, j;
	printf("    A  B  C  D  E  F  G  H  I  J  K  L  M  N  O\n");
	for(i = 0; i < 15; ++i) {
		if(i)printf("   --+--+--+--+--+--+--+--+--+--+--+--+--+--+--\n");
		printf("%2d ", i + 1);
		for(j = 0; j < 15; ++j) {
			if(j)printf("|");
			printf("%s", tablechar[table[i][j]]);
		}
		printf("\n");
	}
}

int set(int x, int y, int c) 
{
	if(x >= 15)return 1;
	if(y >= 15)return 1;
	if(table[y][x])return 1;
	else table[y][x] = c;
	lastx = x;
	lasty = y;
	++count;
	return 0;
}

const int wayx[8] = { -1, +1, -1, +1, -1, +1, +0, +0};
const int wayy[8] = { -1, +1, +1, -1, +0, +0, -1, +1};

int check() 
{
	if(count == 15 * 15)return 3;
	int i, j, x, y, d;
	int c = table[lasty][lastx];
	for(i = 0; i < 8 ; ++i) {
		x = lastx;
		y = lasty;
		for(j = 0; j < 4; ++j) {
			x += wayx[i];
			y += wayy[i];
			if(x < 0 || x >= 15)break;
			if(y < 0 || y >= 15)break;
			if(table[y][x] != c)break;
		}
		if((i&1)==0)d=j;
		else {
			d+=j+1;
			if(d >= 5)return c;
		}
	}
	return 0;
}

void ipttoop(char *ipt, int *x, int *y, int *over) 
{
	char buf[3] = {0};
	int tx, ty;
	tx = ipt[0] - 'A';
	if(tx > ('Z' - 'A'))tx = ipt[0] - 'a';
	if(tx == ('Q' - 'A'))*over = 1;
	if(tx > ('O' - 'A'))tx = -1;
	if(tx < 0)tx = -1;
	buf[0] = ipt[1];
	buf[1] = ipt[2];
	buf[3] = 0;
	ty = atoi(buf) - 1;
	if(ty >= 15)ty = -1;
	*x = tx;
	*y = ty;
}

#endif