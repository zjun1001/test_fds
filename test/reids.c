#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <hiredis.h>
#include "redis_op.h"

int main()
{
	int ret=0;
	redisContext*handle;
	char key_value[100]={0};
	handle=rop_connectdb_nopwd("127.0.0.1", "6379");
	if(handle==NULL)
	{
		ret=-2;
		printf("func rop_connectdb_nopwd err:%d\n",ret);
		return ret;
	}
#if 0
	ret=rop_set_string(handle, "nvqiang", "aixi");
	if(ret==-1)
	{
		printf("func rop_set_string err:%d\n",ret);
		goto END;
	}
#endif
#if 1
	ret=rop_get_string(handle, "hello",key_value);
	//int rop_get_string(redisContext *conn, char *key, char *value)
	if(ret==-1)
	{
		printf("func rop_get_string err:%d\n",ret);
		goto END;
	}
	printf("hello:%s\n",key_value);
#endif
END:
	rop_disconnect(handle);
	return ret;
}