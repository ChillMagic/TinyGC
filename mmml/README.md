# MMML : Map Memory Manager Light

MMML : 轻量级 Map 内存管理 （线程内使用）

目前只提供一个基本的思路。

需要保证，在销毁 `MapMemoryManagerLight` 时，所有 `MapMemoryManagerLight::shared_ptr` 均销毁。

当然可以使用 `std::shared_ptr` 等在 `MapMemoryManagerLight::shared_ptr` 内部储存，用于引用并销毁。

`MapMemoryManagerLight::shared_ptr<T>` 是一个模板类，用于根据类型销毁对象。

有许多改进的思路，分别适用于不同的情况。

可以改进为 只分配内存（`malloc + free`）、分配&销毁对象（`new + delete`）等模式。
