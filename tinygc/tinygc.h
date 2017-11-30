#ifndef _TINYGC_H_
#define _TINYGC_H_
#include <type_traits>
#include <list>

namespace TinyGC
{
	//===================================
	// * Forward declarations
	//===================================
	class GCObject;
	template <typename T>
	class GCValue;
	template <typename _GCTy>
	class GCRootPtr;

	namespace details {
		class GCRootPtrBase;
		class GCRootObserver;
	}

#define CHECK_POINTER_CONVERTIBLE(From, To) \
	static_assert(std::is_convertible<From*, To*>::value, \
				"Invalid pointer conversion from "#From"* to "#To"*")

#define CHECK_GCOBJECT_TYPE(Type) \
	static_assert(std::is_base_of<GCObject, Type>::value, \
				#Type" is not a subclass of GCObject")

	//===================================
	// * Class GCObject
	//===================================
	class GCObject
	{
	public:
		GCObject() : _Mark(false) {}
		virtual ~GCObject() {}

		void GCMark() {
			if (!this->_Mark) {
				this->_Mark = true;
				GCMarkAllSub();
			}
		}
	protected:
		void GCMarkSub(GCObject* sub) {
			if(sub != nullptr){
				sub->GCMark();
			}
		}
		virtual void GCMarkAllSub() {}
	private:
		bool _Mark;
		GCObject *_Next;

		
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
		explicit GCValue(Args &&... args)
			: data(std::forward<Args>(args)...) {}

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
	// * Class GC
	//===================================
	class GC
	{
	public:
		GC() : objectHead(nullptr) {}
		~GC() {
			sweep();
		}

		void collect() {
			mark();
			sweep();
		}

		template <typename T, typename... Args>
		GCRootPtr<T> newObject(Args &&... args) {
			CHECK_GCOBJECT_TYPE(T);
			T *p = new T(std::forward<Args>(args)...);
			addObject(p);
			return GCRootPtr<T>(p, this);
		}

		template <typename T, typename... Args>
		GCRootPtr<GCValue<T>> newValue(Args &&... args) {
			return newObject<GCValue<T>>(std::forward<Args>(args)...);
		}

		details::GCRootObserver& addRoot(const details::GCRootPtrBase* p);

	private:
		GCObject* objectHead;
		std::list<details::GCRootObserver> observers;
		
		void addObject(GCObject *p);
		void mark();
		void sweep();
	};

	namespace details {
		//===================================
		// * Class GCRootObserver
		// * Observes a GCRootPtr
		//===================================
		class GCRootObserver {
			const GCRootPtrBase* observed;
			GC *master;
		public:
			GCRootObserver(const GCRootPtrBase* p, GC *master): 
				observed(p), master(master){
			}

			void reset(const GCRootPtrBase* newRoot = nullptr) {
				observed = newRoot;
			}

			const GCRootPtrBase* get() const {
				return observed;
			}

			GCRootObserver& createNew(const GCRootPtrBase* newRoot){
				return master->addRoot(newRoot);
			}
		};

		//===================================
		// * Class GCRootPtrBase
		// * Not a template class
		// * Does not guarantee type safety
		//===================================
		class GCRootPtrBase {
		protected:
			friend class ::TinyGC::GC;

			GCObject* ptr;
			GCRootObserver* observer;

			GCRootPtrBase() = delete;
			
			GCRootPtrBase(GCObject *ptr, GC *master): 
				ptr(ptr), observer(&master->addRoot(this)) {}
			
			GCRootPtrBase(const GCRootPtrBase & root): 
				ptr(root.ptr), observer(&root.observer->createNew(this)){}
				
			// move construction, does not have to create a new observer again
			GCRootPtrBase(GCRootPtrBase && root): 
				ptr(root.ptr), observer(root.observer) {
				root.observer = nullptr;
				observer->reset(this);
			}

			~GCRootPtrBase() {
				if(observer != nullptr) {
					observer->reset();
				}
			}
		};
	}
	//===================================
	// * Class GCRootPtr
	// * Template class, object type is specified 
	// * supposed to guarantee type safety
	//===================================
	template <typename _GCTy = GCObject>
	class GCRootPtr: private details::GCRootPtrBase
	{
		CHECK_GCOBJECT_TYPE(_GCTy);
	public:
		GCRootPtr(GC *master)
			: GCRootPtrBase(nullptr, master) {}
		GCRootPtr(_GCTy *ptr, GC *master)
			: GCRootPtrBase(ptr, master) {}

		GCRootPtr() = delete;
		GCRootPtr(const GCRootPtr<_GCTy> & gcrp)
			: GCRootPtrBase(gcrp) {}

		template <typename Object>
		GCRootPtr(const GCRootPtr<Object> & gcrp)
			: GCRootPtrBase(gcrp) {
			CHECK_POINTER_CONVERTIBLE(Object, _GCTy);
		}
		GCRootPtr(GCRootPtr<_GCTy> && gcrp)
			: GCRootPtrBase(std::move(gcrp)) {}

		template <typename Object>
		GCRootPtr(GCRootPtr<Object> && gcrp)
			: GCRootPtrBase(std::move(gcrp)) {
			CHECK_POINTER_CONVERTIBLE(Object, _GCTy);
		}

		GCRootPtr<_GCTy>& operator=(const GCRootPtr<_GCTy> & gcrp) {
			this->ptr = gcrp.ptr;
			return *this;
		}

		template <typename Object>
		GCRootPtr<_GCTy>& operator=(const GCRootPtr<Object> & gcrp) {
			CHECK_POINTER_CONVERTIBLE(Object, _GCTy);
			this->ptr = gcrp.ptr;
			return *this;
		}

		/* Does not propagate `const` by default */
		_GCTy* get() const { return reinterpret_cast<_GCTy*>(ptr); }
		_GCTy* operator->() const { return get(); }
		_GCTy& operator*() const { return *get(); }
		operator _GCTy*() const { return get(); }
	};
}

#endif
