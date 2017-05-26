#ifndef QUEUE_H_
#define QUEUE_H_

#include <string>

typedef struct name
{
		std::string name;
		int age;
}Name;

class Queue
{
	public:
		Queue();
		~Queue();

	private:
		Name test;
};


#endif /* QUEUE_H_ */
