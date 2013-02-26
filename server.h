#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include "grid.h"
#define MAX_STRING 1000

struct msg {
  long mtype;
  char mtext[MAX_STRING];
};

struct param
{
  Grid* grid;
  Squirrel* squirrel;
};

key_t generate_msg_key();

int generate_queue(key_t msg_key, int msg_flg);

void message_control(int msq_id, int cmd, struct msqid_ds* buf);

void receive_message(int msq_id, void* msg_p, size_t msg_sz, long msg_typ, int msg_flg);

void send_message_to_client(int msq_id, const void* msg_p, size_t msg_sz, int msg_flg);

void checkif_acorn(int check_success,struct msg msg_p,struct msg m,char** client_ids);
		   
		   
void split_acorn(char** client_ids,struct msg m);

Grid* init_Board(int numPlayers);

void split_first_Info(char** client_info,struct msg m);

Squirrel* check_join_and_send_ack(int msq_id,Grid* myGrid, char* myInfo);

int get_msqid_from_join(struct msg m);

Squirrel** init_squirrel_array();

int add_squirrel(Squirrel* mySquirrel,Squirrel** squirrelArray,int numSquirrels);

void run_squirrel(Squirrel* squirrel);

char** get_client_info(struct msg m);
