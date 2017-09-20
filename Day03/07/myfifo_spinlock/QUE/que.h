#ifndef _QUE_
#define _QUE_

#define QUE_SIZE 10240

typedef unsigned char data_t;

typedef struct que{
	data_t data[QUE_SIZE];
	int head;
	int tail;
	int num;
}QUE;

QUE *create_que(void);
int init_que(QUE *Q);
int enque(QUE *Q, data_t *data, int size);
int deque(QUE *Q, data_t *data, int size);
int get_que_num(QUE *Q);
int destroy_que(QUE *Q);

#endif
