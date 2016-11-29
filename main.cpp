#include <cstdio>
#include <string>
#include "tinygc/tinygc.h"

void println(const std::string &msg) {
	std::printf("%s\n", msg.c_str());
}

struct Point : public TinyGC::GCObject
{
	Point(TinyGC::GCValue<int> *x, TinyGC::GCValue<int> *y)
		: x(x), y(y) {}
	~Point() {
		println("Release Point");
	}

	std::string to_string() const {
		return "(" + std::to_string(*x) + ", " + std::to_string(*y) + ")";
	}

	TinyGC::GCValue<int> *x, *y;

private:
	void GCMarkSub() {
		x->GCMark();
		y->GCMark();
	}
};

struct Test
{
	~Test() {
		println("Release Test");
	}
};

TinyGC::GC GC;

int main(void)
{
	println("Hello World!");

	auto *i1 = GC.newValue<int>(5);
	auto *i2 = GC.newValue<int>(6);
	auto *i3 = GC.newValue<int>(7);
	auto *i4 = GC.newValue<int>(8);
	auto *tmp = GC.newValue<Test>();

	Point *p1 = new Point(i1, i2);
	Point *p2 = new Point(i3, i4);
	Point *p3 = new Point(i1, i2);

	GC.addObject(p1);
	GC.addObject(p2);
	GC.addObject(p3);
	GC.addRoot(p1);
	GC.addRoot(p2);

	println(p1->to_string());
	println(p2->to_string());
	println(p3->to_string());

	GC.collect();


	println(p1->to_string());
	println(p2->to_string());

	return 0;
}
