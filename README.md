# TinyGC

A **light-weighted GC** in C++ (C++11) based on **mark-and-sweep** algorithm.

## Features

- Accurate GC
- Controllable collection
- Low memory consumption, compatible with other memory managements 
- Allow multiple instances of GC

## Use

1. Create `TinyGC::GC` object.
2. Call `newValue` method to create collectable object.
3. Call `newObject` method to create collectable object whose type is a subclass of `GCObject`.
4. Call `collect` to collect garbage.

### Examples

Java:

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

C++ with TinyGC:

```C++
class Point : public TinyGC::GCObject
{
    using Int = TinyGC::GCValue<int>*;
public:
    Point(Int x, Int y)
        : x(x), y(y) {}
    
    Int x, y;

private:
    void GCMarkAllSub() {
        GCMarkSub(x);
        GCMarkSub(y);
    }
}

int main()
{
    TinyGC::GC GC;
    {
        auto p = GC.newObject<Point>(
            GC.newValue<int>(5),
            GC.newValue<int>(6)
        );
    }
    GC.collect();
}
```

## Note

- For TinyGC, `GC::newValue` and `GC::newObject` is the **only** correct way to create collectable objects¡£
- All the objects allocated by `GC` are owned by the `GC` object.
- `GC` will release all resources once go out of scope, therefore can be used within a function, as a non-static menber of class or as thread local.
- `GC::newValue` and `GC::newObject` returns a `GCRootPtr` smart pointer that would guarantee the object it points to will not be collected.


## License

Apache License 2.0