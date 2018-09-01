# Layout Specifications

## overview
This document is the specification of layout system.

The layout system imitates WPF and Android layout system.

## rules

### about `width` and `height` in `LayoutParams`
There is three mode in measure: `Content`, `Exactly`, `Stretch`.

- `Exactly` means the control should be of an exact size.

- `Content` means the control has the size that contains the children.

- `Stretch` means the control stretch to the max size that it can fill. If parent is `Content`, it occupies all available room parent provides, which means its parent will stretch as well.

### about `max_size`, `min_size`

`max_size` specifies the max size and `min_size` specifies the min size. They are of higher priority of `width` and `height`. Calculated size should be adjusted according to the four properties.

## structure

### enum `MeasureMode`
``` c++
enum class MeasureMode
{
    Content,
    Stretch,
    Exactly
};
```

### struct `MeasureLength`
``` c++
struct MeaureLength
{
    float length;
    MeasureMode mode;
};
```

### struct `MeaureSize`
``` c++
struct MeasureSize
{
    MeasureLength width;
    MeasureLength height;
};
```

### struct `OptionalSize`
``` c++
struct OptionalSize
{
    optional<float> width;
    optional<float> height;
}
```

### struct `BasicLayoutParams`
``` c++
struct BasicLayoutParams
{
    MeasureSize size;
    OptionalSize max_size;
    OptionalSize min_size;
}
```

### interface `ILayoutable`
``` c++
struct ILayoutable : virtual Interface
{
    virtual void Measure(const Size&) = 0;
    virtual void Layout(const Rect&) = 0;

    virtual BasicLayoutParams* GetLayoutParams() = 0;
    virtual void SetLayoutParams(BasicLayoutParams* params) = 0;

    virtual Size GetDesiredSize() = 0;
    virtual void SetDesiredSize(const Size& size) = 0;

/*
protected:
    virtual Size OnMeasure(const Size& size);
    virtual void OnLayout(const Rect& rect);
*/
};
```

## process


