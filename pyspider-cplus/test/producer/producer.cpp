#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <msgpack.hpp>

#include "redis_queue.h"

//消息类型中不能存在为0的证书，因为序列化后会为0，导致存入redis后截断；
typedef struct item
{
		char num;
		std::string name;
		//char *data
		std::string  data;
		MSGPACK_DEFINE(num, name, data);
} Item;

RedisQueue<Item> redis_queue("mq", "127.0.0.1", 6379, "test", 10);

void *producer(void *arg)
{
	Item tmp;
	const char *conten = "nba";
	int i = 0;
	while(true)
	{

		tmp.num = '0';
		tmp.name.assign("yr");
		tmp.data = string(conten);

		int ret = redis_queue.put_nowait(tmp);
		if(ret == 1)
		{
			printf("producer %d put succsee\n", i);
		}
		else if(ret == 0)
		{
			printf("producer queue full\n");
		}
		else
		{
			printf("producer redis oprete error\n");
		}
		sleep(1);
	}
	return NULL;
}

int main(void)
{
	redis_queue.init();
	Item tmp;
	const char *conten = "test";

	//存一条序列化消息
	for(int i = 1; i <= 11; i++)
	{
		tmp.num = '0';
		tmp.name.assign("pym");
		tmp.data = string(conten);

		int ret = redis_queue.put_nowait(tmp);
		if(ret == 1)
		{
			printf("%d put succsee\n", i);
		}
		else if(ret == 0)
		{
			printf("queue full\n");
		}
		else
		{
			printf("redis oprete error\n");
		}
	}

	//提取反序列化
	Item data;
	for(int i = 1; i <= 11; i++)
	{
		int ret = redis_queue.get_nowait(data);
		if(ret == 1)
		{
			printf("num: %c ,name: %s, ", data.num, data.name.c_str());
			std::cout << data.data.c_str() << std::endl;
		}
		else if(ret == 0)
		{
			printf("queue empty\n");
		}
		else
		{
			printf("redis oprete error\n");
		}
	}
	//单生产者-单消费者模型测试
	pthread_t pid_p;
	int ret = pthread_create(&pid_p, NULL, producer, NULL);
	if(ret != 0)
	{
		printf("Create Thread Error\n");
		return 0;
	}
	pthread_join(pid_p, NULL);
	return 0;
}
