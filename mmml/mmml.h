#include <map>
#include <vector>
#include <deque>
#include <cassert>

// 轻量级 Map 内存管理 （线程内使用）
//    需要保证，在销毁 MapMemoryManagerLight 时，所有 MapMemoryManagerLight::shared_ptr 均销毁。
//    当然使用 std::shared_ptr 等在 MapMemoryManagerLight::shared_ptr 内部储存，用于引用并销毁。

class MapMemoryManagerLight
{
public:
	template <typename T>
	class shared_ptr
	{
	public:
		shared_ptr() = default;

		explicit shared_ptr(MapMemoryManagerLight &mmml, T *data)
			: mmml(&mmml), index(mmml.insert(data)) {}

		~shared_ptr() {
			if (mmml) {
				if (mmml->dec_count(index)) {
					mmml->destory<T>(index);
				}
			}
		}

		shared_ptr(const shared_ptr &sp)
			: mmml(sp.mmml), index(sp.index) {
			mmml->inc_count(index);
		}

		shared_ptr& operator=(const shared_ptr &sp) {
			mmml = sp.mmml;
			index = sp.index;
			mmml->inc_count(index);
			return *this;
		}

		T* get() const {
			return reinterpret_cast<T*>(mmml->get(index));
		}
		T* operator->() const {
			return get();
		}

	private:
		MapMemoryManagerLight *mmml = nullptr;
		size_t index = 0;
	};

	struct Data {
		void *data = nullptr;
		size_t counter = 0;
	};

public:
	MapMemoryManagerLight() = default;
	~MapMemoryManagerLight() {
		assert(_keymap.empty());
	}

	size_t insert(void *dat) {
		auto iter = _keymap.find(dat);
		if (iter != _keymap.end()) {
			return iter->second;
		}
		else {
			if (_allocable_index.empty()) {
				size_t id = _data.size();
				_data.push_back(Data{ dat, 1 });
				_keymap[dat] = id;
				return id;
			}
			else {
				size_t id = _allocable_index.front();
				_keymap[dat] = id;
				_data[id] = Data{ dat, 1 };
				_allocable_index.pop_front();
				return id;
			}
		}
	}

	template <typename T>
	void destory(size_t index) {
		void *ptr = _data[index].data;
		delete reinterpret_cast<T*>(ptr);
		_allocable_index.push_back(index);
		_keymap.erase(ptr);
	}

	void inc_count(size_t index) {
		_data[index].counter++;
	}
	bool dec_count(size_t index) {
		_data[index].counter--;
		return _data[index].counter == 0;
	}
	void* get(size_t index) const {
		return _data.at(index).data;
	}

private:
	std::map<void*, size_t> _keymap;
	std::vector<Data> _data;
	std::deque<size_t> _allocable_index;
};
