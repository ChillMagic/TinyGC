#include "tinygc.h"

namespace TinyGC
{
	void GC::mark() {
		for(auto iter = observers.begin(), end = observers.end(); iter != end; ){
			auto root_pp = iter->get();
			if(root_pp != nullptr) {  // if false, observer invalidated
				auto root = root_pp->ptr;
				if(root != nullptr) {  // if false, GCRootPtr is null
					root->GCMark();
				}
				++iter;
			} else { // remove invalidated observer
				iter = observers.erase(iter);
			}
		}
	}

	void GC::sweep() {
		GCObject* newObjectHead = nullptr; // Linked list of not collected Objects
		GCObject* next;
		for(GCObject* p = objectHead; p != nullptr; p = next){
			next = p->_Next;
			if (p->_Mark) {
				p->_Mark = false;
				p->_Next = newObjectHead;
				newObjectHead = p;
			} else {
				delete p;
			}
		}
		objectHead = newObjectHead;
	}

	void GC::addObject(GCObject *p) {
		p->_Next = objectHead;
		objectHead = p;
	}

	details::GCRootObserver& GC::addRoot(const details::GCRootPtrBase* p) {
		observers.emplace_back(p, this);
		return observers.back();
	}
}
