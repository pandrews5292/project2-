struct queue_node {
    char* message;
    struct queue_node* next;
    int message_length;
    long message_type;
};

struct queue {
    struct queue_node* head;
    int length;
};

void show(struct queue* q);

char* pop(struct queue* q);

void push(struct queue* q, char* m, int m_type);

int get_length(struct queue* q);

struct queue* create_queue();

struct queue_node* create_queue_node(char* m, int m_type);

int check_first_node_type(struct queue* q);

void destroy_queue(struct queue* q);




