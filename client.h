#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/resource.h>

#define MAX_STRING 100;

struct msg {
  long mtype;
  char mtext[100];
};

int get_client_id();

void set_anchor_for_new_client(int prev_id);

int get_len_id(int msq_id);

char* send_JOIN(int msq_id, int num_players);
 
key_t generate_msg_key();

int generate_queue(key_t msg_key, int msg_flg);

void message_control(int msq_id, int cmd, struct msqid_ds* buf);

void receive_message(int msq_id, void* msg_p, size_t msg_sz, long msg_typ, int msg_flg);

void send_message_to_server(int msq_id, const void* msg_p, size_t msg_sz, int msg_flg);

void send_directional_message(char* direction, int client_msqid);
