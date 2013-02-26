#include "client.h"

#define PATHNAME "server_anchor.txt"
#define SERVER_PROJ_ID 99

int CLIENT_MSQID;
long M_TYPE;
int SERVER_MSQID;
int MY_COLLECTED_ACORNS = 0;
char* MY_NAME;


void get_server_msqid(){
  //Get the servers msqid and set SERVER_MSQID
  int id, msqid;
  id = generate_msg_key(SERVER_PROJ_ID);
  
  msqid = generate_queue(id, 0600 | IPC_CREAT);
  SERVER_MSQID = msqid;
}

void generate_my_msqid(){
  //Get this clients msqid and set CLIENT_MSQID
  int client_id, id, msq_id;

  client_id = get_client_id();
  set_anchor_for_new_client(client_id);

  id = generate_msg_key(client_id);
  msq_id = generate_queue(id, 0600 | IPC_CREAT);
  CLIENT_MSQID = msq_id;
  M_TYPE = 1;
}

int get_client_id(){
  //Get this clients id from server_anchor.txt
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
  //increment number in server_anchor.txt by 1 for new clients to use
  FILE *in_file = fopen(PATHNAME, "w");
  int new_id = prev_id+1;
  fprintf(in_file, "%d", new_id);
  fclose(in_file);
}

int get_len_id(int msq_id){
  //Get length of msqid
  if (msq_id < 10){
      return 1;
  }
  else{
    return 1 + (msq_id / 10);
  }
}

key_t generate_msg_key(int proj_id){
  //Generate msg_key for generating queue
  int id; 
  id = ftok(PATHNAME, proj_id);
  if (id == -1){
    perror("Failed to genereate message queue id: ");
  }
  return id;
}

int generate_queue(int msg_key, int msg_flg){
  //Create this clients message queue
  int msq_id = msgget(msg_key, msg_flg);
  if (msq_id == -1){
    perror("Failed to generate message queue: ");
  }
  return msq_id;
}

void message_control(int msq_id, int cmd, struct msqid_ds* buf){
  //Run msgctl on given msqid
  int control = msgctl(msq_id, cmd, buf);
  if (control == -1){
    perror("Failed to control queue: ");
  }
}

void receive_message(int msq_id, void* msg_p, size_t msg_sz, long msg_typ, int msg_flg){
  //Receive message from this clients message queue
  ssize_t message_received = msgrcv(msq_id, msg_p, msg_sz, msg_typ, msg_flg);
  if (message_received == -1){
    perror("Messaged received failed: ");
  }
}

void send_message_to_server(int msq_id, const void* msg_p, size_t msg_sz, int msg_flg){
  //Send a message to the server
  int sent_message = msgsnd(msq_id, msg_p, msg_sz, msg_flg);
  if (sent_message == -1){
    perror("Failed to send message: ");
  }
}

char* send_JOIN(int msq_id, int num_players){
  //Send join message to the server.
  int server_msqid;
  char* id = NULL;
  id = (char*)malloc(sizeof(char)* get_len_id(msq_id));
  struct msg j;
  j.mtype = M_TYPE;
  memset(&j.mtext, 0, 100);
  strcat(j.mtext, "J_");
  sprintf(id, "%d_%d", msq_id, num_players);
  strcat(j.mtext,id );
  strcat(j.mtext,"_");
  strcat(j.mtext, MY_NAME);
  printf("join message: %s\n", j.mtext);
  get_server_msqid();

  send_message_to_server(SERVER_MSQID, &j, strlen(j.mtext), MSG_NOERROR);
  free(id);
}

int send_directional_message(char* direction, int client_msqid){
  //Send specific directional message.
  int message_set = 0;
  char* right = "right\n"; 
  char* left = "left\n"; 
  char* up = "up\n"; 
  char* down = "down\n";

  char* id = NULL;
  id = (char*)malloc(sizeof(char)* get_len_id(client_msqid));
  sprintf(id, "%d", client_msqid);
  direction = strtok(direction, " ");

  struct msg m;
  m.mtype = M_TYPE;
  memset(&m.mtext, 0, 10);

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
    send_message_to_server(SERVER_MSQID, &m, strlen(m.mtext), MSG_NOERROR);
  }
  free(id);
  return message_set;
}

void init_join(int msq_id){
  //Initialize the clients join to the server
  char* input = NULL;
  int num_players;
  input = (char*)malloc(sizeof(char)*1000);

  printf("Please enter your squirrel's name... ");
  fgets(input, MAX_STRING, stdin);
  MY_NAME = input;
  printf("Please enter the number of players you would like in the game... ");
  scanf("%d", &num_players);
  printf("Thank you the game will begin shortly with %d player(s)... \n\n", num_players );
  send_JOIN(msq_id, num_players);

  free(input);
}

void check_init_server_response_and_parse(){
  //Check the server join awk message, parse it, and print it to the user.
  char* message;
  struct msg m;
  m.mtype = 1;
  memset(&m.mtext, 0, 1000);
  printf("waiting for a response from server...\n\n");
  receive_message(CLIENT_MSQID, &m, MAX_STRING, M_TYPE, MSG_NOERROR);
  message = strtok(m.mtext, "&");

  char* sep_newline = "/n";
  char* temp_string = NULL;
  int i = 0, j = 0;

  char** game_info = NULL;
  game_info = (char**)malloc(sizeof(char*)*MAX_STRING);
 
  temp_string = strtok(message, sep_newline);
  while(temp_string != NULL){
    game_info[i] = temp_string;
    temp_string = strtok(NULL, sep_newline);
    i++;
  }

  parse_grid_size(game_info[1]);

  int k = 0;
  for (k; k< i-2; k++){
    parse_acorn_locations(game_info[k+2]);
  }
  printf("\n");
  printf("Enter right, left, up, down to move your squirrel\n");
  free(game_info);
}

void parse_grid_size(char* info){
  //Inform the user of the grid size
  int i = 0;
  char* temp_string;
  char* sep_ = "_"; 
  char* grid_size[2];
  temp_string = strtok(info, sep_);
  while(temp_string != NULL){
    grid_size[i] = temp_string;
    temp_string = strtok(NULL, sep_);
    i++;
  }
  printf("The grid size is %s x %s\n",grid_size[0], grid_size[1]);
}

void parse_acorn_locations(char* info){
  //Inform the user of the acorn locations
  int i = 0;
  char* temp_string;
  char* sep_ = "_"; 
  char* acorn_locations[3];
  temp_string = strtok(info, sep_);
  while(temp_string != NULL){
    acorn_locations[i] = temp_string;
    temp_string = strtok(NULL, sep_);
    i++;
  }
  printf("There are %s acorns located at (%s, %s)\n", acorn_locations[2], acorn_locations[1], acorn_locations[0]);
}

void parse_server_reply(){
  //Determine message sent from server based on directional move and print to use
  struct msg m;
  memset(&m.mtext, 0, 100);
  m.mtype = 1;
  receive_message(CLIENT_MSQID, &m, MAX_STRING, M_TYPE, MSG_NOERROR);
  if (!strcmp(m.mtext, "Your move was successful and you got an ACORN!!")){
    //printf("MY_COLLECTED_ACORNS++\n");
    MY_COLLECTED_ACORNS++;
  }
  if (!strcmp(m.mtext, "GAME OVER")){
    printf("You found the most the acorns(%d)...you win!!\n", MY_COLLECTED_ACORNS+1);
    printf("Exiting...\n");
    exit(0);
  }
  else{
    printf("%s\n", m.mtext);
  }

}

int main(){
  int message_set = 0;

  char* input = NULL;
  input = (char*)malloc(sizeof(char)*100);

  int msq_id, run = 1;

  struct msqid_ds msqid_ds, *buf;
  buf = &msqid_ds;

  generate_my_msqid();
  get_server_msqid();

  init_join(CLIENT_MSQID);

  check_init_server_response_and_parse();

  while (run == 1){

    if (strlen(input) > 0){
      printf("Please enter direction to move: ");
    } 

    fgets(input, MAX_STRING, stdin);

    if (strlen(input) > 1){
     
      message_set = send_directional_message(input, CLIENT_MSQID);
      if (message_set == 1){
	      parse_server_reply();
      }
    }  

  }
  message_control(msq_id, IPC_RMID, buf);
}

