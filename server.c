#include "server.h"
#include "grid.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "queue.h"
#define PATHNAME "server_anchor.txt"
#define PROJ_ID 99

int MAX_NUMPLAYERS = 1;
struct queue* myQueue;
Grid* grid;

key_t generate_msg_key(){
  //generate the msg_key for creating a message queue
  int id; 
  id = ftok(PATHNAME, PROJ_ID);
  if (id == -1){
    perror("Failed to genereate message queue id: ");
  }
  return id;
}

int generate_queue(int msg_key, int msg_flg){
  //generate a message key based on msg_key
  int msq_id = msgget(msg_key, msg_flg);
  if (msq_id == -1){
    perror("Failed to generate message queue: ");
  }
  return msq_id;
}

void message_control(int msq_id, int cmd, struct msqid_ds* buf){
  //function for using msgctl
  int control = msgctl(msq_id, cmd, buf);
  if (control == -1){
    perror("Failed to control queue: ");
  }
}

void send_message(int msq_id, const void* msg_p, size_t msg_sz, int msg_flg){
  //send a message to a given message queue
  int sent_message = msgsnd(msq_id, msg_p, msg_sz, msg_flg);
  if (sent_message == -1){
    perror("Failed to send message: ");
  }
}

void receive_message(int msq_id, void* msg_p, size_t msg_sz, long msg_typ, int msg_flg)
//receive a message from my message queue
{
  ssize_t message_received = msgrcv(msq_id, msg_p, msg_sz, msg_typ, msg_flg);
  if (message_received == -1){
    perror("Messaged received failed: ");
  }
}

void checkif_Acorn(int check_success,struct msg msg_p,int client_ids)
//depending on status of squirrels move, sending corresponding message to client
{
  if(check_success==0)
    {  
      char failed[] = "Illegal move, squirrel would jump of the board.";
      strcpy(msg_p.mtext,failed);
      send_message(client_ids, &msg_p, strlen(msg_p.mtext), MSG_NOERROR);
      memset(&msg_p.mtext, 0, 100);
    }
  else if(check_success==1)
    {
      char success[] = "Your move was successful and you got an ACORN!!";
      strcpy(msg_p.mtext,success);
      send_message(client_ids, &msg_p, strlen(msg_p.mtext), MSG_NOERROR);
      memset(&msg_p.mtext, 0, 100);
    }
  else if(check_success==2)
    {
      char success[] = "Your move was successful, but no acorn.";
      strcpy(msg_p.mtext,success);
      send_message(client_ids, &msg_p, strlen(msg_p.mtext), MSG_NOERROR);
      memset(&msg_p.mtext, 0, 100);
    }
  else if(check_success==3)
    {
      char over[] = "GAME OVER";
      strcpy(msg_p.mtext,over);
      send_message(client_ids, &msg_p, strlen(msg_p.mtext), MSG_NOERROR);
      memset(&msg_p.mtext, 0, 100);
      exit(0);
    }
}

int get_msqid_from_join(struct msg m){
  //parse msqid from a message from the client
    char* temp = strtok(m.mtext, "_");
    temp = strtok(NULL, "_");
    return atoi(temp);
}


void send_ack(Squirrel* mySquirrel,char* myInfo)
//send acknowledgement to specified client of received messaged
{
  struct msg s;
  s.mtype = 1;// msq_id;
  strcpy(s.mtext, myInfo);
  send_message(mySquirrel->msq_id, &s, strlen(s.mtext), MSG_NOERROR);
}




Grid* init_Board(int numPlayers)
//initialized board based on number of players
{
  Grid* grid = NULL;
  int length = numPlayers+4;
  grid = create_grid(length);
  generate_acorns_per_player(grid,numPlayers);
  return grid;
}

char** get_client_info(struct msg m)
//parse information sent from a client ex. move_msqid_numplayers_name
{
  char** client_info = (char**) malloc(sizeof(char*)*100);
  char* myInfo = (char*) malloc(sizeof(char)* MAX_STRING);
  int j = 0 ;
  for(j;j<10;j++)
    {
      client_info[j] = (char*) malloc(sizeof(char)*100);
    }
  char* temp = strtok(m.mtext,"_");
  strcpy(client_info[0],temp); //j
  temp = strtok(NULL,"_");
  strcpy(client_info[1],temp); //id
  temp = strtok(NULL,"_");
  strcpy(client_info[2],temp); //players
  if(MAX_NUMPLAYERS == 1)
    {
      MAX_NUMPLAYERS = atoi(client_info[2]);
    }
  temp = strtok(NULL,"_");
  strcpy(client_info[3],temp); //name


  return client_info;
}

char** move(struct msg m)
//parse a clients move into client_info
{
  char** client_info = (char**) malloc(sizeof(char*)*100);
  int j = 0 ;
  for(j;j<2;j++)
    {
      client_info[j] = (char*) malloc(sizeof(char)*100);
    }
  char* temp = strtok(m.mtext,"_");
  strcpy(client_info[0],temp); //j
  temp = strtok(NULL,"_");
  strcpy(client_info[1],temp); //id
  
  return client_info;
}


Squirrel** init_squirrel_array()
//initialize the array that will hold all the squirrels
{
  Squirrel** squirrel_players =(Squirrel**) malloc(sizeof(Squirrel*)*MAX_STRING);
  return squirrel_players;
}

int add_squirrel(Squirrel* mySquirrel,Squirrel** squirrelArray,int numSquirrels)
//add a squirrel to the squirrel array
{  
  squirrelArray[numSquirrels] = mySquirrel;
  numSquirrels++;
  return numSquirrels;
}
  
void run_squirrel(Squirrel* squirrel)
//function passed to thread to run the logic of a squirrel
{
  show(myQueue);
  
  while(grid->total_acorns!=0)
    {
      if(check_first_node_type(myQueue) == squirrel->msq_id)
	{
	  char* myMessage = pop(myQueue);
	  int check_success = move_squirrel(grid,squirrel,myMessage[0]);
	  struct msg r;
	  r.mtype = 1;
	  checkif_Acorn(check_success,r,squirrel->msq_id);
	  printf("return message\n");
	  print_grid(grid);
	}
    }
}

int main()
{
  
  //#########################################################################
  //declarations

  int i,j;

  int rtrn, num_messages, id, msq_id, run;

  extern int errno;
  struct msg m;
  struct msqid_ds msqid_ds, *buf;

  char** client_info =NULL;

  Squirrel* mySquirrel;
  
  
  char* myInfo=(char*) malloc(sizeof(char)*MAX_STRING);
  char name[MAX_STRING];
 
  Squirrel** squirrel_players;
  pthread_t thread[MAX_STRING];
  
  int numSquirrels=0;

  int currNumPlayers = 0;

  myQueue = create_queue();

  buf = & msqid_ds;
  m.mtype = 1;
   
  id = generate_msg_key();
  msq_id = generate_queue(id, 0600 | IPC_CREAT);
  run = 1;
  
  //running loop for server to watch over game
  while(run == 1)
    {
      while(currNumPlayers < MAX_NUMPLAYERS)
	{
	  
	  memset(&m.mtext, 0, 1000);
	  receive_message(msq_id, &m, 1000, 1, MSG_NOERROR);
	  currNumPlayers++;

	  client_info = get_client_info(m);
	  strcpy(name,client_info[3]);

	  if(currNumPlayers == 1)
	    {
	      grid = init_Board(atoi(client_info[2]));
	      set_board_info(grid,myInfo);
	      squirrel_players = init_squirrel_array();
	    }
	  
	  mySquirrel = create_squirrel(grid, atoi(client_info[1]));
	  printf("%d\n", mySquirrel->msq_id);
	  strcpy(mySquirrel->name,name);
	  numSquirrels = add_squirrel(mySquirrel,squirrel_players,numSquirrels);
	  
	}
      
      for(i=0;i<numSquirrels;i++)
	{
	  printf("%s joining...\n", squirrel_players[i]->name);
	  send_ack(squirrel_players[i],myInfo);
	  printf("id: %d\n", squirrel_players[i]->msq_id);
	  pthread_create(&thread[i],NULL,(void*) run_squirrel,(void*)squirrel_players[i]);
	}
      
      printf("\n");
      print_grid(grid);
      
      while(grid->total_acorns!=0)
	{
	  memset(&m.mtext,0,1000);
	  receive_message(msq_id, &m, 1000, 1, MSG_NOERROR);
	  char** client_id = move(m);
	  push(myQueue,client_id[0],atol(client_id[1]));
	  printf("type: %d\n",atoi(client_id[1]));
	  show(myQueue);
	  
	}
      
    }

  if (!strcmp(m.mtext, "exit"))
    {
      run = 0;
    }
  else
    {
      printf("Message: %s", m.mtext);    
    }
  message_control(msq_id, IPC_RMID, buf);
  printf("Goodbye!\n");

}

