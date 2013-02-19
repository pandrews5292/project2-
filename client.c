#include "client.h"

#define PATHNAME "server_anchor.txt"
#define PROJ_ID 77


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

void receive_message(int msq_id, void* msg_p, size_t msg_sz, long msg_typ, int msg_flg){
  ssize_t message_received = msgrcv(msq_id, msg_p, msg_sz, msg_typ, msg_flg);
  if (message_received == -1){
    perror("Messaged received failed: ");
  }

}

void send_message(int msq_id, const void* msg_p, size_t msg_sz, int msg_flg){
  int sent_message = msgsnd(msq_id, msg_p, msg_sz, msg_flg);
  if (sent_message == -1){
    perror("Failed to send message: ");
  }
}

 
int main(){

  int rtrn, num_messages, id, msq_id, run;
  extern int errno;
  struct msg m;
  m.mtype = 1;
  struct msqid_ds msqid_ds, *buf;
  buf = & msqid_ds;
  
  id = generate_msg_key();
  msq_id = generate_queue(id, 0600 | IPC_CREAT);
  run = 1;
  char* input = NULL;
  input = (char*)malloc(sizeof(char)*100);
  while(run == 1){

    memset(input, '\0', 100);
    memset(&m.mtext, '\0', 100);

    printf("Enter a string: ");
    fgets(input, 100, stdin);

    if (feof(stdin)){
      printf("ctrl-d\n");
      printf("Goodbye!\n");
      strcpy(m.mtext, "exit");
      send_message(msq_id, &m, strlen(m.mtext), MSG_NOERROR);
      exit(0);
    }
    strcpy(m.mtext, input);
    send_message(msq_id, &m, strlen(m.mtext), MSG_NOERROR);
  }
    message_control(msq_id, IPC_RMID, buf);

}

