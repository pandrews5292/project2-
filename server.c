#include "server.h"
#include "grid.h"
#include <pthread.h>
#include <string.h>
#define PATHNAME "server_anchor.txt"
#define PROJ_ID 99


key_t generate_msg_key(){
  int id; 
  id = ftok(PATHNAME, PROJ_ID);
  if (id == -1){
    perror("Failed to genereate message queue id: ");
  }
  return id;
}

int generate_queue(int msg_key, int msg_flg){
  int msq_id = msgget(msg_key, msg_flg);
  if (msq_id == -1){
    perror("Failed to generate message queue: ");
  }
  return msq_id;
}

void message_control(int msq_id, int cmd, struct msqid_ds* buf){
  int control = msgctl(msq_id, cmd, buf);
  if (control == -1){
    perror("Failed to control queue: ");
  }
}

void send_message(int msq_id, const void* msg_p, size_t msg_sz, int msg_flg){
  int sent_message = msgsnd(msq_id, msg_p, msg_sz, msg_flg);
  if (sent_message == -1){
    perror("Failed to send message: ");
  }
}

void receive_message(int msq_id, void* msg_p, size_t msg_sz, long msg_typ, int msg_flg)
{
  ssize_t message_received = msgrcv(msq_id, msg_p, msg_sz, msg_typ, msg_flg);
  if (message_received == -1){
    perror("Messaged received failed: ");
  }
}

int main(int argc,char** argv)
{
  
  /////////////////////////////////////////
  //creates and inititalizes the grid
  int i,j;
  int numPlayers = 1;
  int length = 3;
  int width = 4;

  Grid* myGrid = createGrid(width,length);
  generateAcorns2(myGrid,numPlayers);

  char* myInfo =(char*) malloc(sizeof(char));
  boardInfo(myGrid,myInfo);
  ///////////////////////////////////////////

  int rtrn, num_messages, id, msq_id, run;
  extern int errno;
  struct msg m;
  struct msqid_ds msqid_ds, *buf;
  buf = & msqid_ds;
  m.mtype = 1;
   
  id = generate_msg_key();
  msq_id = generate_queue(id, 0600 | IPC_CREAT);
  run = 1;
  
  
  char* client_info[4];
  client_info[3] = NULL;

  while(run == 1){
    memset(&m.mtext, 0, 100);
    receive_message(msq_id, &m, 100, 1, MSG_NOERROR);
    printf("%s\n", m.mtext);
    
    if(m.mtext[0] == 'J') {
	    char temp[100];
	    strcpy(temp,m.mtext);
	    char* client_id;
	    client_id = strtok(temp,"_");
	    int start = 0;
	      while(client_id!=NULL) {
          client_info[start] = client_id;
	        client_id = strtok(NULL,"_");
	        start++;
	       
	      }

        struct msg s;
        s.mtype = 1;
        strcpy(s.mtext, myInfo);
	    send_message(atoi(client_info[1]), &s, strlen(s.mtext), MSG_NOERROR);
      memset(&m.mtext, 0, 100);

      printf("message to send: %s\n", myInfo);
    }
    
    if(m.mtext[0]=='R' || m.mtext[0]=='L' || m.mtext[0]=='U' ||m.mtext[0]=='D'){

    }
    memset(&m.mtext, 0, 100);

    message_control(msq_id, IPC_STAT, buf);
    if (!strcmp(m.mtext, "exit")){
      run = 0;
    }
    else{
      printf("Message: %s", m.mtext);
    }

  }
  message_control(msq_id, IPC_RMID, buf);
  printf("Goodbye!\n");

}

