#include "mmml.h"
#include <cstdio>

class Test
{
public:
	explicit Test(size_t n)
		: num(n) {
		printf("Create Test(%zu).\n", num);
	}

	Test(const Test &test)
		: num(test.num) {
		printf("Copy Test(%zu).\n", num);
	}

	~Test() {
		printf("Destory Test(%zu).\n", num);
	}

	Test& operator=(const Test &test) {
		num = test.num;
		printf("Call oper= Test(%zu).\n", num);
		return *this;
	}

	size_t num;
};

int main()
{
	using MMML = MapMemoryManagerLight;

	printf("===1 Begin===\n");

	{
		Test a(1);
	}

	printf("===1 End===\n");
	printf("===2 Begin===\n");

	{
		Test *p = new Test(2);
	}

	printf("===2 End===\n");
	printf("===3 Begin===\n");

	MMML mmml;

	{
		MMML::shared_ptr<Test> p(mmml, new Test(3));
	}

	printf("===3 End===\n");
	printf("===4 Begin===\n");

	{
		Test *p = new Test(4);
		MMML::shared_ptr<Test> p1(mmml, p);
		MMML::shared_ptr<Test> p2(mmml, p);
	}

	printf("===4 End===\n");
	printf("===5 Begin===\n");

	MMML::shared_ptr<Test> *Lp1;

	{
		Test *p = new Test(5);
		Lp1 = new MMML::shared_ptr<Test>(mmml, p);
	}

	printf("===5 End===\n");

	delete Lp1;

	printf("===6 Begin===\n");
	
	MMML::shared_ptr<Test> Lp2;

	{
		Test *p = new Test(6);
		Lp2 = MMML::shared_ptr<Test>(mmml, p);
	}

	printf("===6 End===\n");
	printf("===7 Begin===\n");

	{
		MMML::shared_ptr<Test> p(mmml, new Test(7));
		printf("%zu\n", p->num);
	}
	printf("===7 End===\n");


    return 0;
}
