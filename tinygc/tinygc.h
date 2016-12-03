#ifndef _TINYGC_H_
#define _TINYGC_H_
#include <set>
#include <list>
#include <mutex>

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

	class GC;

	//===================================
	// * Class GCObjectRefList
	//===================================
	template <typename _GCTy = GCObject>
	class GCRootPtr
	{
	public:
		GCRootPtr(_GCTy *ptr, GC *master)
			: ptr(ptr), master(master) {}
		~GCRootPtr() {
			this->master->removeRoot(this->ptr);
		}

		_GCTy* get() { return ptr; }
		const _GCTy* get() const { return ptr; }
		_GCTy* operator->() { return ptr; }
		const _GCTy* operator->() const { return ptr; }
		_GCTy& operator*() { return *ptr; }
		const _GCTy& operator*() const { return *ptr; }

	private:
		_GCTy *ptr;
		GC *master;
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
		template <typename _GCTy>
		GCRootPtr<_GCTy> addRoot(_GCTy *p) {
			mtx.lock();
			Root.add(p);
			mtx.unlock();
			return GCRootPtr<_GCTy>(p, this);
		}

	private:
		std::list<GCObject*> GCObjectList;
		GCObjectRefList Root;
		std::mutex mtx;

		void addObject(GCObject *p) {
			GCObjectList.push_back(p);
		}

		void mark();
		void sweep();

		template <typename _GCTy> friend class GCRootPtr;

		void removeRoot(GCObject *p) {
			mtx.lock();
			Root.remove(p);
			mtx.unlock();
		}
	};
}

#endif
