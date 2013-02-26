#ifndef GRID_H
#define GRID_H
#define MAX_NAME 1000

typedef struct grid
{
  int size;;
  int total_acorns;
  int** allAcorns;
  
}Grid;

typedef struct squirrel
{
  int x;
  int y;
  char name[MAX_NAME];
  int msq_id;
  int numAcorns;
}Squirrel;


//Grid Functions
Grid* create_grid(int size);
void print_grid(Grid* myGrid);
int get_total_acorns(Grid* myGrid);
void generate_random_acorns(Grid* myGrid);
void generate_acorns_per_player(Grid* myGrid, int numPlayers);
void set_board_info(Grid* myGrid, char* myInfo);
Squirrel* create_squirrel(Grid* myGrid,int msq_id);
int move_squirrel(Grid* myGrid, Squirrel* squirrel, char direction);


#endif
