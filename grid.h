#ifndef GRID_H
#define GRID_H


typedef struct Grid
{
  int width;
  int length;
  int TotalAcorns;
  int** allAcorns;
  
}Grid;


//Grid Functions
Grid* createGrid(int width,int length);
int getLength(Grid* myGrid);
int getWidth(Grid* myGrid);
int getTotalAcorns(Grid* myGrid);
int noAcorns(Grid* myGrid);

void generateAcorns(Grid* myGrid);
void generateAcorns2(Grid* myGrid,int numPlayers);

char* boardInfo(Grid* myGrid,char* myInfo);

#endif
