# TinyGC

基于 **标记·清除** 算法，配合C++的 **轻量级GC**（使用C++11）

## 特征

- 准确式GC
- 可控的垃圾回收
- 不独占内存，与其他内存管理方式兼容
- 允许拥有多个GC实例

## 使用方法

1. 创建 TinyGC::GC 对象。
2. 使用 newValue 方法 创建相关的可回收的**不含**可回收对象的引用的对象。
3. 使用 newObject 方法 创建相关的可回收的**含有**可回收对象的引用对象。
4. 使用 addRoot 方法 将**不可回收的对象**加入Root中。
5. 使用 removeRoot 方法 将**可回收的对象**从Root中移除。
6. 使用 collect 方法 进行垃圾回收。

### 示例

Java 代码：

```Java
class Point
{
    Point(int x, int y) {
        this.x = x;
        this.y = y;
    }
    int x, y;
}

Point p = new Point(5, 6);
```

C++ with TinyGC 代码：

```C++
class Point : public TinyGC::GCObject
{
    using Int = TinyGC::GCValue<int>*;
public:
    Point(Int x, Int y)
        : x(x), y(y) {}
    
    Int x, y;

private:
    void GCMarkSub() {
        x->GCMark();
        y->GCMark();
    }
}

int main()
{
    TinyGC::GC GC;

    Point *p = GC.newObject<Point>(
        GC.newValue<int>(5),
        GC.newValue<int>(6)
    );

    GC.collect(); // 进行GC，注意此后p是悬空的。
}
```

# 备注

- 对于TinyGC来说，GC.newValue 和 GC.newObject 是**唯一**正确的创建可回收对象的方式。
- 资源是由GC对象独占的， TinyGC::GC 相当于高级别的 std::unique_ptr 指针组。
- Tiny::GC 对象会在生命周期结束后自动回收所有对象，因此可在其他类的内部建立GC对象，用于管理类内的可回收资源。

## 许可

Apache Licene 2.0