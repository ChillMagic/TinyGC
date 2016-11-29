#ifndef _TINYGC_H_
#define _TINYGC_H_
#include <set>
#include <list>

namespace TinyGC
{
	//===================================
	// * Class GCObject
	//===================================
	class GCObject
	{
	public:
		GCObject() : _Mark(false) {}
		virtual ~GCObject() {}

		void GCMark();

	private:
		bool _Mark;
		virtual void GCMarkSub() {}

		friend class GC;
	};

	//===================================
	// * Class GCValue
	//===================================
	template <typename T>
	class GCValue : public GCObject
	{
	public:
		GCValue() = default;
		~GCValue() = default;

		template <typename... Args>
		explicit GCValue(Args... args)
			: data(args...) {}

		GCValue& operator=(const T &o) {
			this->data = o;
			return *this;
		}
		operator T&() { return this->get(); }
		operator const T&() const { return this->get(); }
		T& get() { return data; }
		const T& get() const { return data; }

	private:
		T data;
	};

	//===================================
	// * Class GCObjectRefList
	//===================================
	class GCObjectRefList : public std::set<GCObject*>
	{
	public:
		void add(GCObject *op) {
			this->insert(op);
		}
		void remove(GCObject *op) {
			this->erase(op);
		}

		void GCMark();
	};

	//===================================
	// * Class GC
	//===================================
	class GC
	{
	public:
		GC() {}
		~GC() {
			sweep();
		}

		void collect();

		template <typename T, typename... Args>
		GCValue<T>* newValue(Args... args) {
			GCValue<T> *p = new GCValue<T>(args...);
			addObject(p);
			return p;
		}
		template <typename T, typename... Args>
		T* newObject(Args... args) {
			T *p = new T(args...);
			addObject(p);
			return p;
		}
		void addRoot(GCObject *p) {
			Root.add(p);
		}
		void removeRoot(GCObject *p) {
			Root.remove(p);
		}

	private:
		std::list<GCObject*> GCObjectList;
		GCObjectRefList Root;

		void addObject(GCObject *p) {
			GCObjectList.push_back(p);
		}

		void mark();
		void sweep();
	};
}

#endif
