## 2. Types

### Primitve Types

The language currently has the following primitive types:

| Type   | Description                                              | Size |
| ------ | -------------------------------------------------------- | ---: |
| `i32`  | Signed integer type representing a 32-bit integer number |  4B  |
| `bool` | Boolean type, value is either `true` or `false`          |  1B  |
| `char` | A UTF-32 encoded character                               |  4B  |
| `()`   | Unit type; denotes the absence of a value                |  1B  |

### Pointer Types

Pointer Types enable reference-like semantics. They are denoted by placing an asterisk (`*`) in front of any type: `*T`, where `T` can be any type. Pointers can be nested indefinitely.

Examples:

| Type    | Description                              |
|---------|------------------------------------------|
| `*i32`  | Pointer to a 32-bit integer              |
| `**i32` | Pointer to a pointer to a 32-bit integer |
| `*()`   | Pointer to the Unit type                 |

### Array Types

Array types enable references to a contiguous span of objects of the same type. They are denoted by placing `[]` in front of any type: `[]T`, where `T` can be any type. Arrays can be nested indefinitely.

Examples:

| Type     | Description                          |
|----------|--------------------------------------|
| `[]char` | Array of characters                  |
| `[]*i32` | Array of pointers to 32-bit integers |
| `[][]()` | Array of arrays of units             |


### Function Types (WIP)

Function types describe references to functions. They are denoted by `(U) -> V`, where `U` is zero, one, or more parameter types separated by commas (`,`), and `V` is a single return type. Function types can be used like any other type.

Examples:

| Type                | Description                                                      |
|---------------------|------------------------------------------------------------------|
| `() -> ()`          | Function type that takes no arguments and returns a unit         |
| `(i32, i32) -> i32` | Function type that takes two i32 values and returns an i32       |
| `[](() -> ())`      | Array of function types that take no arguments and return a unit |

#### NOTE: 
While all globally defined functions have function types associated with them and can be referenced by their function types, function values are not yet fully implemented. As a result, constructs such as the following are not currently possible:

```
func foo(a: i32) -> i32 { return a; }

func main() -> i32 {
    bar: (i32) -> i32 = foo;
    return bar(10);
}
```