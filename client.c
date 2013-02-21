#include "client.h"

#define PATHNAME "server_anchor.txt"
#define SERVER_PROJ_ID 99
#define MAX_LEN_INPUT 100
#define MAX_ACK_STRING 1000


int get_server_msqid(){
  int id, msqid;
  id = generate_msg_key(SERVER_PROJ_ID);
  msqid = generate_queue(id, 0600 | IPC_CREAT);
  return msqid;
}

int generate_my_msqid(){
  int client_id, id, msq_id;

  client_id = get_client_id();
  set_anchor_for_new_client(client_id);

  id = generate_msg_key(client_id);
  msq_id = generate_queue(id, 0600 | IPC_CREAT);

  return msq_id;

}

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

void send_message_to_server(int msq_id, const void* msg_p, size_t msg_sz, int msg_flg){
  int sent_message = msgsnd(msq_id, msg_p, msg_sz, msg_flg);
  if (sent_message == -1){
    perror("Failed to send message: ");
  }
}

char* send_JOIN(int msq_id, int num_players){
  int server_msqid, r;
  char* id = NULL;
  id = (char*)malloc(sizeof(char)* get_len_id(msq_id));
  struct msg j;
  j.mtype = 1;
  memset(&j.mtext, '\0', 100);
  strcat(j.mtext, "J_");
  sprintf(id, "%d_%d", msq_id, num_players);
  strcat(j.mtext,id );
  server_msqid = get_server_msqid();

  send_message_to_server(server_msqid, &j, strlen(j.mtext), MSG_NOERROR);
  free(id);
}

void send_directional_message(char* direction, int client_msqid){
  int message_set = 0;
  int server_msqid;
  char* right = "right\n"; 
  char* left = "left\n"; 
  char* up = "up\n"; 
  char* down = "down\n";
  server_msqid = get_server_msqid();

  char* id = NULL;
  id = (char*)malloc(sizeof(char)* get_len_id(client_msqid));
  sprintf(id, "%d", client_msqid);

  struct msg m;
  m.mtype = 1;
  memset(&m.mtext, '\0', 10);

  if (!strcmp(direction, right)){
    strcat(m.mtext, "R_");
    strcat(m.mtext, id);
    message_set = 1;


  }
  if (!strcmp(direction, left)){
    strcat(m.mtext, "L_");
    strcat(m.mtext, id);
    message_set = 1;

  }
  if (!strcmp(direction, up)){
    strcat(m.mtext, "U_");
    strcat(m.mtext, id);
    message_set = 1;

  }
  if (!strcmp(direction, down)){
    strcat(m.mtext, "D_");
    strcat(m.mtext, id);
    message_set = 1;

  }

  if (message_set == 0){
    printf("Please enter valid move\n");
  }else {
    send_message_to_server(server_msqid, &m, strlen(m.mtext), MSG_NOERROR);
  }
}

void init_join(msq_id){
  char* input = NULL;
  char* s_name;
  int num_players;
  input = (char*)malloc(sizeof(char)*100);

  printf("Please enter your squirrel's name... ");
  fgets(input, MAX_LEN_INPUT, stdin);
  s_name = input;
  printf("Please enter the number of players you would like in the game... ");
  scanf("%d", &num_players);
  printf("Thank you the game will begin shortly... %s", s_name);
  send_JOIN(msq_id, num_players);

  free(input);
}

char* check_server_response(){
  int my_msqid;
  my_msqid = generate_my_msqid();

  struct msg* m;
  m = (struct msg*)malloc(sizeof(struct msg));
  
  m->mtype = 1;
  receive_message(my_msqid, &m, MAX_ACK_STRING, 1, MSG_NOERROR);

  char* message = m->mtext;

  return message;


}

void parse_server_ack(char* ack_string){
  char* sep_newline = "\n";
  char* temp_array = NULL;
  int i = 0, j = 0;

  char** game_info = NULL;
  game_info = (char**)malloc(sizeof(char*)*MAX_ACK_STRING);
  for (j; j< MAX_ACK_STRING; j++){
    game_info[i] = (char*)malloc(sizeof(char)* 5);
  }

  if (ack_string[0] != 'B'){
    printf("Bad start string....exiting\n");
    exit(0);
  }
  else{
    
    temp_array = strtok(ack_string, sep_newline);
    while(temp_array != NULL){
      game_info[i] = temp_array;
      temp_array = strtok(NULL, sep_newline);
      i++;
    }
  }
  int len = game_info[0][0];
  int width = game_info[0][2];
  printf("Grid Size is %d x %d\n", len, width);
  
}

int main(){

  char* input = NULL;
  input = (char*)malloc(sizeof(char)*100);

  int msq_id, run = 1;

  struct msqid_ds msqid_ds, *buf;
  buf = &msqid_ds;

  msq_id = generate_my_msqid();

  init_join(msq_id);

  char* response = check_server_response();
  parse_server_ack(response);

  while (run == 1){

    if (strlen(input) > 0){
      printf("Please enter direction to move: ");
    } 

    fgets(input, MAX_LEN_INPUT, stdin);

    if (strlen(input) > 1){
      send_directional_message(input, msq_id);
    }  

  }
  message_control(msq_id, IPC_RMID, buf);

}

