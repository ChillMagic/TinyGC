#include "tinygc.h"

namespace TinyGC
{
	void GCObject::GCMark() {
		if (!this->_Mark) {
			this->_Mark = true;
			GCMarkSub();
		}
	}

	void GCObjectRefList::GCMark() {
		for (GCObject *p : *this) {
			if (p)
				p->GCMark();
		}
	}

	void GC::collect() {
		mark();
		sweep();
	}

	void GC::mark() {
		Root.GCMark();
	}

	void GC::sweep() {
		for (auto iter = GCObjectList.begin(); iter != GCObjectList.end(); ) {
			GCObject *p = *iter;
			if (p->_Mark) {
				p->_Mark = false;
				++iter;
			}
			else {
				delete p;
				iter = GCObjectList.erase(iter);
			}
		}
	}
}
