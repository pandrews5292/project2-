#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define MAX_STRING 100;

struct msg {
  long mtype;
  char mtext[100];
};

key_t generate_msg_key();

int generate_queue(key_t msg_key, int msg_flg);

void message_control(int msq_id, int cmd, struct msqid_ds* buf);

void receive_message(int msq_id, void* msg_p, size_t msg_sz, long msg_typ, int msg_flg);

void send_message(int msq_id, const void* msg_p, size_t msg_sz, int msg_flg);


struct arguments
{
  int msq_id;
  const void* msg_p;
  size_t msg_sz;
  int msg_flg;
};

struct receive
{
  int msq_id;
  void* msg_p;
  size_t msg_sz;
  long msg_typ;
  int msg_flg;
};
    
