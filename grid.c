#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"

#define MAX_INT 1000

Grid* create_grid(int size)
{
  //initializes the grid and an array that will contain the number of acorns in
  // in that position
  int i,j;

  Grid* myGrid = (Grid*)malloc(sizeof(Grid));
  myGrid->size = size;
 
  myGrid->total_acorns = 0;

  myGrid->allAcorns = (int**)malloc(sizeof(int*)*(myGrid->size));
  
  for(i=0; i<myGrid->size; i++)
  {
    myGrid->allAcorns[i] = (int*) malloc(sizeof(int)*(myGrid->size));
    for(j=0; j<myGrid->size; j++)
	  {
	    myGrid->allAcorns[i][j] = 0;
	  }
  }
  return myGrid;
}

void print_grid(Grid* myGrid)
//print out the current state of the grid
{
  int i,j;
  for(i=0;i<myGrid->size;i++)
    {
      for(j=0;j<myGrid->size;j++)
	     {
	       printf("%d ",myGrid->allAcorns[i][j]);
	 
	     }
       printf("\n");
    }
}



void generate_random_acorns(Grid* myGrid)
{
  //generates random number of acorns
  srand(time(NULL));
  int randNum;
  
  int i,j;
  
  for(i=0;i<myGrid->size;i++)
    for(j=0;j<myGrid->size;j++)
    {
	    int numAcorns=0;
	
	    randNum = rand()%2;
	    while(randNum == 0)
	    {
	      numAcorns++;
	      randNum = rand()%2;
	    }
	    myGrid->allAcorns[i][j] = numAcorns;
	    myGrid->total_acorns += numAcorns;
    }
}

void generate_acorns_per_player(Grid* myGrid,int numPlayers){

  //generates acorn according to the number of players
  srand(time(NULL));
  int i, j, x, y, randNum, scale = numPlayers*10;
  
  myGrid->total_acorns = scale; 
 
  while(scale > 0) {
    randNum = rand() % myGrid->size;
    for (i = 0; i<2; i++){
      if (i == 0){
        x = randNum;
      }
      else{
        y = randNum;
      }
      randNum = rand() % myGrid->size;
    }
    myGrid->allAcorns[x][y] += 1;
    scale -= 1;       
  }
}

void set_board_info(Grid* myGrid, char* myInfo)
//set init message to send to client to inform them of board info
{
  char init[] ="B/n";
  char gridsize[MAX_INT];
  sprintf(gridsize,"%d_%d/n",myGrid->size,myGrid->size);
  strcpy(myInfo,init);
  strcat(myInfo,gridsize);

  int i,j;
  
  for(i=0;i<myGrid->size;i++)
    {
      char coordinates[MAX_INT];
      for(j=0;j<myGrid->size;j++)
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

Squirrel* create_squirrel(Grid* myGrid,int msq_id)
//create a squirrel
{
  
  srand(time(NULL));

  Squirrel* mySquirrel = (Squirrel*) malloc(sizeof(Squirrel));
  mySquirrel->msq_id = msq_id;
  int randNum, greatest, x, y;
  
  greatest =myGrid->size;
  
  randNum = rand() % greatest;
  x = randNum;
  randNum = rand() % greatest;		   
  y = randNum;
  
  while(myGrid->allAcorns[x][y]!=0)
    {
      randNum = rand() % greatest;
      x = randNum;
      randNum = rand() % greatest;		   
      y = randNum;
    }
  mySquirrel->x =x;
  mySquirrel->y =y;
  mySquirrel->numAcorns=0;
  return mySquirrel;
}

int move_squirrel(Grid* myGrid, Squirrel* squirrel, char direction){
  //move the squirrel to a new position on the board if legal
  printf("squirrelname: %s\n",squirrel->name);
  int direction_to_move;
  char* dir;
  if (direction == 'U'){
    dir = "U";
    direction_to_move = squirrel->x - 1;
  }
  else if (direction == 'D'){
    dir = "D";
    direction_to_move = squirrel->x + 1;
  }
  else if (direction == 'L'){
    dir = "L";
    direction_to_move = squirrel->y - 1;
  }
  else if (direction == 'R'){
    dir = "R";
    direction_to_move = squirrel->y + 1;
  }

  if (direction_to_move < 0 || direction_to_move > myGrid->size - 1) {
    //bad move
    return 0;
  }
  else{

      if (!strcmp(dir, "U") || !strcmp(dir, "D")){
        squirrel->x = direction_to_move;
      }
      if (!strcmp(dir, "R") || !strcmp(dir, "L")) {
        squirrel->y = direction_to_move;
      }
      if (myGrid->allAcorns[squirrel->x][squirrel->y] != 0){
          squirrel->numAcorns++;
          myGrid->allAcorns[squirrel->x][squirrel->y] -= 1; 
          myGrid->total_acorns -= 1;
          printf("total_acorns: %d\n", myGrid->total_acorns);
          //game over
          if (myGrid->total_acorns == 0) {return 3;}
          //got an acorn
          return 1;
      }
      else{
          //good move no acorn
          return 2;
      }
  }
}
