#ifndef REDIS_QUEUE_H_
#define REDIS_QUEUE_H_

#include <iostream>
#include <string>
#include <sstream>
#include <msgpack.hpp>
#include <time.h>
#include <unistd.h>

#include "hiredis.h"

using namespace std;

#define CMD_MAX_LEN 64

template<class T>
class RedisQueue
{
	public:
		RedisQueue(const std::string name, const std::string host, const int port,
				const std::string pwd, const unsigned long max_size);
		~RedisQueue();
		bool init();
		int put_nowait(const T obj);//通用对象
		int put(bool block,  const T &obj);
		int get_nowait(T &obj);
		int get(bool block, T &obj);

	private:
		long long qsize();
		bool empty();
		bool full();
		void pack_data(const T &t, string &str); //序列化
		void unpack_data(string &str, T &t);  //反序列化
		void print(string const& buf);

	private:
		std::string name; //redis queue name
		std::string host; // ip
		int port; // port
		std::string pwd; // password
		long long max_size; // max size of queue
		redisContext *redis; // redis handle
		redisReply *reply;
		long long last_qsize; // queue
		bool lazy_limit;
		int time_out;   //超时时间
};

template<class T>
RedisQueue<T>::RedisQueue(const std::string name, const std::string host, const int port,
		const std::string pwd, const unsigned long max_size)
{
	this->name = name;
	this->host = host;
	this->port = 6379;
	this->pwd = pwd;
	this->max_size = max_size;
	this->redis = NULL;
	this->last_qsize = 0;
	this->lazy_limit = true;
	this->time_out = 0.3;
}

template<class T>
RedisQueue<T>::~RedisQueue()
{
	if (redis)
	{
		redisFree(redis);
		redis = NULL;
	}
	if(reply)
	{
		freeReplyObject(reply);
		reply = NULL;
	}
}

template<class T>
bool RedisQueue<T>::init()
{
	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	redis = redisConnectWithTimeout(host.c_str(), port, timeout);
	if (redis == NULL || redis->err)
	{
		if (redis)
		{
			printf("Connection error: %s\n", redis->errstr);
		}
		else
		{
			printf("Connection error: can't allocate redis context\n");
		}
		return false;
	}
	reply = NULL;
	return true;
}

template<class T>
long long RedisQueue<T>::qsize()
{
	char tmp[] = "llen %s";
	char cmd[CMD_MAX_LEN] = { 0 };
	snprintf(cmd, CMD_MAX_LEN, tmp, name.c_str());
	reply = (redisReply*)redisCommand(redis, cmd);
	if(reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	else if(reply->type == REDIS_REPLY_INTEGER)
	{
		last_qsize = reply->integer;
		printf("size %lld\n", last_qsize);
	}
	return last_qsize;
}

template<class T>
bool RedisQueue<T>::full()
{
	return qsize() >= max_size ? true : false;
}

template<class T>
bool RedisQueue<T>::empty()
{
	return qsize() == 0 ? true : false;
}

/*
 * 功能： 非等待入队，类似非阻塞入队
 * 返回值: 0 队列满   1 入队成功 -1 redis执行指令
 * */
template<class T>
int RedisQueue<T>::put_nowait(const T obj)
{
	std::string buffer;

	if(this->lazy_limit && (this->last_qsize < this->max_size))
	{
		//序列化对象到字符串对象
		pack_data(obj , buffer);
		reply = (redisReply *)redisCommand(redis, "rpush %s %s", name.c_str(), buffer.data());
		if(reply->type == REDIS_REPLY_ERROR)
		{
			return -1;
		}
		else if(reply->type == REDIS_REPLY_INTEGER)
		{
			last_qsize = reply->integer;
		}
		return 1;
	}
	else if(this->full())
	{
		return 0;
	}
	return 1;
}

template<class T>
int RedisQueue<T>::put(bool block, const T &obj)
{
	if(block == false)
	{
		return this->put_nowait(obj);
	}

	time_t start_time, cur_time;
	unsigned delay;
	int lasted;
	int ret = 1;

	time(&start_time);
	while(true)
	{
		//队列满了，等待消费者消费腾出空间
		ret = put_nowait(obj);

		if(ret == 0 && ret == -1)
		{
			time(cur_time);
			lasted = cur_time - start_time;
			if(time_out > lasted)
			{
				delay = ((time_out) > (time_out - lasted)) ? (time_out - lasted): (time_out);
				sleep(delay);
			}
		}
		else if(ret == 1)
		{
			break;
		}
	}
	return 1;
}

template<class T>
int RedisQueue<T>::get(bool block, T &obj)
{
	string buffer;
	if(block == false)
	{
		return get_nowait();
	}
	time_t start_time, cur_time;
	unsigned delay;
	int lasted;
	int ret = 1;

	time(&start_time);
	while(true)
	{
		//队列空，等待消费者消费腾出空间
		ret = get_nowait(obj);

		if(ret == 0 && ret == -1)
		{
			time(cur_time);
			lasted = cur_time - start_time;
			if(time_out > lasted)
			{
				delay = ((time_out) > (time_out - lasted)) ? (time_out - lasted): (time_out);
				sleep(delay);
			}
		}
		else if(ret == 1)
		{
			break;
		}
	}
	return 1;
}

/*
 * 功能： 非等待出队，类似非阻塞出队
 * 返回值: 0 队列空   1 出队成功  -1 redis执行指令
 * */
template<class T>
int RedisQueue<T>::get_nowait(T &obj)
{
	if(this->empty())
	{
		printf("queue empty\n");
		return 0;
	}
	string buffer;
	reply = (redisReply *)redisCommand(redis,"lpop %s", name.c_str());
    if(reply->type == REDIS_REPLY_ERROR)
	{
    	return -1;
	}
	else if(reply->type == REDIS_REPLY_STRING)
	{
		printf("get_nowait %s, %zu\n", reply->str, reply->len);
		buffer.assign(reply->str, reply->len);
		unpack_data(buffer, obj);
	}
	else
	{
		printf("get_nowait error\n");
	}
	return 1;
}

template<class T>
void RedisQueue<T>::unpack_data(string &str, T &obj)
{
	print(str);
	msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());
	msgpack::object deserialized = oh.get();
	deserialized.convert(obj);
}

template<class T>
void RedisQueue<T>::pack_data(const T &t, string &str)
{
	std::stringstream buffer;
	msgpack::pack(buffer, t);
    // send the buffer ...
    buffer.seekg(0);
    str.assign(buffer.str());
    print(str);
}

template<class T>
void RedisQueue<T>::print(string const& buf)
{
	for (std::string::const_iterator it = buf.begin(), end = buf.end();
		 it != end;
		 ++it) {
		std::cout
			<< std::setw(2)
			<< std::hex
			<< std::setfill('0')
			<< (static_cast<int>(*it) & 0xff)
			<< ' ';
	}
	std::cout << std::dec << std::endl;
}

#endif /* REDIS_QUEUE_H_ */
