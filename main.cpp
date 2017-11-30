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
		println("Release Point " +  to_string());
	}

	std::string to_string() const {
		return "(" + std::to_string(*x) + ", " + std::to_string(*y) + ")";
	}

	TinyGC::GCValue<int> *x, *y;

protected:
	void GCMarkAllSub() override {
		GCMarkSub(x);
		GCMarkSub(y);
	}
};

struct DoublePoint : public TinyGC::GCObject
{
	DoublePoint(Point *p0, Point *p1)
		: p0(p0), p1(p1) {}
	~DoublePoint() {
		println("Release DoublePoint " + to_string());
	}

	std::string to_string() const {
		return "(" + p0->to_string() + ", " + p1->to_string() + ")";
	}

	Point *p0, *p1;

protected:
	void GCMarkAllSub() override {
		GCMarkSub(p0);
		GCMarkSub(p1);
	}
};

struct AnotherDoublePoint : public Point
{
	AnotherDoublePoint(Point *p0, Point *p1)
		: Point(*p0), p1(p1) {}
	~AnotherDoublePoint() {
		println("Release AnotherDoublePoint " + to_string());
	}

	std::string to_string() const {
		return "(" + Point::to_string() + ", " + p1->to_string() + ")";
	}

	Point *p1;

protected:
	void GCMarkAllSub() override {
		Point::GCMarkAllSub();
		GCMarkSub(p1);
	}
};

struct Test
{
	~Test() {
		println("Release Test");
	}
};

template<typename DP>
TinyGC::GCRootPtr<DP> make_double_point(TinyGC::GC &GC, int x1, int y1, int x2, int y2) {
	return GC.newObject<DP>(
		GC.newObject<Point>(
			GC.newValue<int>(x1), 
			GC.newValue<int>(y1)), 
		GC.newObject<Point>(
			GC.newValue<int>(x2), 
			GC.newValue<int>(y2)));
}

int main(void)
{
	TinyGC::GC GC;

	auto dp = make_double_point<DoublePoint>(GC, 1, 2, 3, 4);

	{
		auto tdp = make_double_point<AnotherDoublePoint>(GC, 1, 2, 3, 4);
		auto tdp2 = tdp;
		{
			auto ttdp = GC.newValue<Test>();
			tdp2->p1 = nullptr;
		}
		println("GC Start");
		GC.collect();
		println("GC End");
	}

	println("GC Start");
	GC.collect();
	println("GC End");

	return 0;
}
