#include "que.h"
#include <linux/slab.h>
#include <linux/mm.h>

QUE *create_que(void)
{
	QUE *Q = kmalloc(sizeof(QUE), GFP_KERNEL);
	memset(Q, 0, sizeof(QUE));
	Q->tail = -1;

	return Q;
}

int init_que(QUE *Q)
{
	memset(Q, 0, sizeof(QUE));
	Q->tail = -1;
	
	return 0;
}

static int isfull_que(QUE *Q)
{
	return QUE_SIZE == Q->num;
}
int enque(QUE *Q, data_t *data, int size)
{
	int n;
	if(isfull_que(Q))
		return 0;

	size = ( size<(QUE_SIZE-Q->num) )?size:(QUE_SIZE-Q->num);
	
	n = size;
	while(n--)
	{
		Q->tail = (Q->tail+1)%QUE_SIZE;
		Q->data[Q->tail] = *data++;
	}

	Q->num += size;
	return size;
}

int deque(QUE *Q, data_t *data, int size)
{
	int n;
	size = ( size<Q->num )?size:Q->num;

	n =size;
	while(n--)
	{
		*data++ = Q->data[Q->head];
		Q->head = (Q->head+1)%QUE_SIZE;
	}	

	Q->num -= size;
	return size;
}

int get_que_num(QUE *Q)
{
	return Q->num;
}

int destroy_que(QUE *Q)
{
	kfree(Q);
	return 0;
}


