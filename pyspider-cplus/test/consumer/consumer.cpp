#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <msgpack.hpp>

#include "redis_queue.h"

//��Ϣ�����в��ܴ���Ϊ0��֤�飬��Ϊ���л����Ϊ0�����´���redis��ضϣ�
typedef struct item
{
		char num;
		std::string name;
		std::string  data;
		MSGPACK_DEFINE(num, name, data);
} Item;

RedisQueue<Item> redis_queue("mq", "127.0.0.1", 6379, "test", 10);

void *consumer(void *arg)
{
	Item data;
	while(true)
	{
		int ret = redis_queue.get_nowait(data);
		if(ret == 1)
		{
			printf("num: %c ,name: %s, ", data.num, data.name.c_str());
			std::cout << data.data.c_str() << std::endl;
		}
		else if(ret == 0)
		{
			printf("consumer queue empty\n");
		}
		else
		{
			printf("consumer redis oprete error\n");
		}
		sleep(1);
	}
	return NULL;
}

int main(void)
{
	redis_queue.init();

	//��������-��������ģ�Ͳ���
	pthread_t pid_c;
	int ret = pthread_create(&pid_c, NULL, consumer, NULL);
	if(ret != 0)
	{
		printf("Create Thread Error\n");
		return 0;
	}
	pthread_join(pid_c, NULL);

	return 0;
}
