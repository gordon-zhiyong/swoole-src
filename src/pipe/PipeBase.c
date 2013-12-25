#include "swoole.h"

static int swPipeBase_read(swPipe *p, void *data, int length);
static int swPipeBase_write(swPipe *p, void *data, int length);
static int swPipeBase_getFd(swPipe *p, int isWriteFd);
static int swPipeBase_close(swPipe *p);

typedef struct _swPipeBase
{
	int pipes[2];
} swPipeBase;

int swPipeBase_create(swPipe *p, int blocking)
{
	int ret;
	swPipeBase *object = sw_malloc(sizeof(swPipeBase));
	if (object == NULL)
	{
		return -1;
	}
	p->blocking = blocking;
	ret = pipe(object->pipes);
	if (ret < 0)
	{
		return -1;
	}
	else
	{
		//Nonblock
		if(blocking == 0)
		{
			swSetNonBlock(object->pipes[0]);
			swSetNonBlock(object->pipes[1]);
		}
		p->object = object;
		p->read = swPipeBase_read;
		p->write = swPipeBase_write;
		p->getFd = swPipeBase_getFd;
		p->close = swPipeBase_close;
	}
	return 0;
}

static int swPipeBase_read(swPipe *p, void *data, int length)
{
	swPipeBase *this = p->object;
	return read(this->pipes[0], data, length);
}

static int swPipeBase_write(swPipe *p, void *data, int length)
{
	swPipeBase *this = p->object;
	return write(this->pipes[1], data, length);
}

static int swPipeBase_getFd(swPipe *p, int isWriteFd)
{
	swPipeBase *this = p->object;
	return (isWriteFd == 0) ?  this->pipes[0] : this->pipes[1];
}

static int swPipeBase_close(swPipe *p)
{
	int ret1, ret2;
	swPipeBase *this = p->object;
	ret1 = close(this->pipes[0]);
	ret2 = close(this->pipes[1]);
	sw_free(this);
	return 0-ret1-ret2;
}
