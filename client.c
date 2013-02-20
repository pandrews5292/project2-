#include "server_client.h"

#define PATHNAME "server_anchor.txt"
#define MAX_LEN_INPUT 100


int get_client_id(){
  char* line = NULL;
  int id;
  struct stat file_stat;

  if (stat(PATHNAME, &file_stat) == 0){
    FILE *in_file = fopen(PATHNAME, "r");
    fscanf(in_file, "%d", &id);
    fclose(in_file);   
    return id;
  }
  else{
    printf("server_anchor.txt does not exist\n");
    exit(0);
  }
}

void set_anchor_for_new_client(int prev_id){
  FILE *in_file = fopen(PATHNAME, "w");
  int new_id = prev_id+1;
  fprintf(in_file, "%d", new_id);
  fclose(in_file);
}

int get_len_id(int msq_id){
  if (msq_id < 10){
      return 1;
  }
  else{
    return 1 + (msq_id / 10);
  }
}

key_t generate_msg_key(int proj_id){
  int id; 
  id = ftok(PATHNAME, proj_id);
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

char* send_JOIN(int msq_id, int num_players){
  char* id = NULL;
  id = (char*)malloc(sizeof(char)* get_len_id(msq_id));
  struct msg j;
  j.mtype = 1;
  memset(&j.mtext, '\0', 100);
  strcat(j.mtext, "J_");
  sprintf(id, "%d_%d", msq_id, num_players);
  strcat(j.mtext,id );
 
  printf("%s\n", j.mtext);
  free(id);

}
 
int main(){

  int rtrn, num_messages, id, msq_id, run, client_id;

  struct msqid_ds msqid_ds, *buf;
  buf = &msqid_ds;

  client_id = get_client_id();
  set_anchor_for_new_client(client_id);

  id = generate_msg_key(client_id);
  msq_id = generate_queue(id, 0600 | IPC_CREAT);

  char* input = NULL;
  input = (char*)malloc(sizeof(char)*100);

  printf("Please enter your squirrel's name...");
  fgets(input, MAX_LEN_INPUT, stdin);
  char* s_name;
  s_name = input;
  printf("Please enter the number of players you would like in the game... ");
  int num_players;
  scanf("%d", &num_players);
  printf("Thank you the game will begin shortly... %s", s_name);
  send_JOIN(msq_id, num_players);
  
  message_control(msq_id, IPC_RMID, buf);

}

