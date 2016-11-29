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

struct DoublePoint : public TinyGC::GCObject
{
	DoublePoint(Point *p0, Point *p1)
		: p0(p0), p1(p1) {}
	~DoublePoint() {
		println("Release DoublePoint");
	}

	std::string to_string() const {
		return "(" + p0->to_string() + ", " + p1->to_string() + ")";
	}

	Point *p0, *p1;

private:
	void GCMarkSub() {
		p0->GCMark();
		p1->GCMark();
	}
};

struct Test
{
	~Test() {
		println("Release Test");
	}
};

int main(void)
{
	TinyGC::GC GC;

	auto *i1 = GC.newValue<int>(5);
	auto *i2 = GC.newValue<int>(6);
	auto *i3 = GC.newValue<int>(7);
	auto *i4 = GC.newValue<int>(8);
	auto *tmp = GC.newValue<Test>();

	Point *p1 = GC.newObject<Point>(i1, i2);
	Point *p2 = GC.newObject<Point>(i3, i4);
	Point *p3 = GC.newObject<Point>(i1, i2);

	DoublePoint *dp = GC.newObject<DoublePoint>(p1, p2);

	GC.addRoot(dp);

	println(p1->to_string());
	println(p2->to_string());
	println(p3->to_string());
	println(dp->to_string());

	GC.collect();


	println(p1->to_string());
	println(p2->to_string());
	println(dp->to_string());

	return 0;
}
