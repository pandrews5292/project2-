#include "server.h"

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

void send_message_to_client(int msq_id, const void* msg_p, size_t msg_sz, int msg_flg){
  int sent_message = msgsnd(msq_id, msg_p, msg_sz, msg_flg);
  if (sent_message == -1){
    perror("Failed to send message: ");
  }
}

void receive_message(int msq_id, void* msg_p, size_t msg_sz, long msg_typ, int msg_flg){
  ssize_t message_received = msgrcv(msq_id, msg_p, msg_sz, msg_typ, msg_flg);
  if (message_received == -1){
    perror("Messaged received failed: ");
  }

}

int main(){

  int rtrn, num_messages, id, msq_id, run;
  extern int errno;
  struct msg m;
  struct msqid_ds msqid_ds, *buf;
  buf = & msqid_ds;
  m.mtype = 1;
   
  id = generate_msg_key();

  msq_id = generate_queue(id, 0600 | IPC_CREAT);
  printf("MSQID: %d\n", msq_id);
  run = 1;
  while(run == 1){
    memset(&m.mtext, '\0', 100);

    
    receive_message(msq_id, &m, 101, 1, MSG_NOERROR);
    printf("Message: %s\n", m.mtext);
  }

  message_control(msq_id, IPC_RMID, buf);
  printf("Goodbye!\n");

}

