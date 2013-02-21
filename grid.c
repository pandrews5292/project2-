#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"

#define MAX_INT 1000

Grid* createGrid(int width,int length)
{
  //initializes the grid and an array that will contain the number of acorns in
  // in that position

  int i,j;
  Grid* myGrid = (Grid*) malloc(sizeof(Grid));
  myGrid->width = width;
  myGrid->length = length;
  myGrid->TotalAcorns = 0;

  myGrid->allAcorns = (int**) malloc(sizeof(int*)*(myGrid->width));
  
  for(i=0;i<myGrid->width;i++)
    {
      myGrid->allAcorns[i] = (int*) malloc(sizeof(int)*(myGrid->length));
      for(j=0;j<myGrid->length;j++)
	{
	  myGrid->allAcorns[i][j] = 0;
	}
    }
  
  return myGrid;
}

  
int getLength(Grid* myGrid)
{
  //returns the length of the grid
  return myGrid->length;
}

int getWidth(Grid* myGrid)
{
  //returns the width of the grid
  return myGrid->width;
}

int getTotalAcorns(Grid* myGrid)
{
  //returns the total number of acorns left
  return myGrid->TotalAcorns;
}

int noAcorns(Grid* myGrid)
{
  if(myGrid->TotalAcorns>0) {return 1;}
  else {return 0;}
}

void generateAcorns(Grid* myGrid)
{
  //generates random number of acorns
  srand(time(NULL));
  int randNum;
  
  int i,j;
  
  for(i=0;i<myGrid->width;i++)
    for(j=0;j<myGrid->length;j++)
      {
	int numAcorns=0;
	
	randNum = rand()%2;
	while(randNum == 0)
	  {
	    numAcorns++;
	    randNum = rand()%2;
	  }
	myGrid->allAcorns[i][j] = numAcorns;
	myGrid->TotalAcorns += numAcorns;
      }
}

void generateAcorns2(Grid* myGrid,int numPlayers)
{
  //generates acorn according to the number of players
  srand(time(NULL));
  int randNum;
  int greatest;
  int scale = numPlayers*3;
  int i,j,x,y;
  
  myGrid->TotalAcorns+=scale;
  
  if(myGrid->length > myGrid->width)   {greatest = myGrid->length;} 
  else if(myGrid->width > myGrid->length)   {greatest = myGrid->width;}
  else   {greatest =myGrid->length;}
  
  
  while(scale>0)
    {
      randNum = rand() % greatest;
      for(i=0;i<2;i++)
	{
	  if(i==0){x = randNum;}
	  else if(i==1){y = randNum;}
	  randNum = rand() % greatest;
	}
 
      if(greatest ==  myGrid->width && y!=0)
	{
	myGrid->allAcorns[x][y-1] = myGrid->allAcorns[x][y-1] + 1;
	}
      else if(greatest ==  myGrid->length && x!=0 )
	{
	myGrid->allAcorns[x-1][y] = myGrid->allAcorns[x-1][y] + 1;
	}
      else
	{
	  myGrid->allAcorns[x][y] = myGrid->allAcorns[x][y] + 1;
	}
      scale-=1;	  
    }
}

char* boardInfo(Grid* myGrid,char* myInfo)
{
  //char* myInfo =(char*) malloc(sizeof(char));
  char init[] ="B/n";
  char gridsize[25];
  sprintf(gridsize,"%d_%d/n",myGrid->width,myGrid->length);
  strcpy(myInfo,init);
  strcat(myInfo,gridsize);

  int i,j;
  
  for(i=0;i<myGrid->width;i++)
    {
      char coordinates[25];
      for(j=0;j<myGrid->length;j++)
	{
	  if(myGrid->allAcorns[i][j] != 0)
	    {
	      sprintf(coordinates,"%d_%d_%d/n",i,j,myGrid->allAcorns[i][j]);
	      strcat(myInfo,coordinates);
	    }
	}
    }
  strcat(myInfo,"&");
}

