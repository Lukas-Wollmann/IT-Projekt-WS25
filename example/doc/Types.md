## 2. Types

### Primitve Types

The language currently has the following primitive types:

| Type   | Description                                              |    Size |
| ------ | -------------------------------------------------------- | ------: |
| `i32`  | Signed integer type representing a 32-bit integer number | 4 Bytes |
| `bool` | Boolean type, value is either `true` or `false`          | 1 Bytes |
| `char` | A UTF-32 encoded character                               | 4 Bytes |
| `()`   | Unit type; denotes the absence of a value                | 1 Bytes |

### Pointer Types

Pointer Types enable reference-like semantics. They are denoted by placing an asterisk (`*`) in front of any type: `*T`, where `T` can be any type. Pointers can be nested indefinitely. They have implicit reference counting and garbage collection. Both take another 8 Byte on the Heap.
The maximum number of References per Pointer is $2^{64}$.

Examples:

| Type    | Description                              | Size on Stack | Size on Heap |
| ------- | ---------------------------------------- | ------------: | -----------: |
| `*i32`  | Pointer to a 32-bit integer              |       8 Bytes | 4 + 16 Bytes |
| `**i32` | Pointer to a pointer to a 32-bit integer |       8 Bytes | 8 + 16 Bytes |
| `*()`   | Pointer to the Unit type                 |       8 Bytes | 1 + 16 Bytes |

### Array Types

Array types enable references to a contiguous span of objects of the same type. They are denoted by placing `[]` in front of any type: `[]T`, where `T` can be any type. Arrays can be nested indefinitely. Under the Hood they are Pointers with an extended Controll Block. They manage Reference Countng, Garbage Collection, Array Sizes and Element Sizes (each 8 Bytes).

Examples:

| Type     | Description                          | Size on Stack |      Size on Heap |
| -------- | ------------------------------------ | ------------: | ----------------: |
| `[]char` | Array of characters                  |       8 Bytes | 4 \* T + 32 Bytes |
| `[]*i32` | Array of pointers to 32-bit integers |       8 Bytes | 8 \* T + 32 Bytes |
| `[][]()` | Array of arrays of units             |       8 Bytes | 8 \* T + 32 Bytes |

### Function Types (WIP)

Function types describe references to functions. They are denoted by `(U) -> V`, where `U` is zero, one, or more parameter types separated by commas (`,`), and `V` is a single return type. Function types can be used like any other type.

Examples:

| Type                | Description                                                 | Size on Stack |      Size on Heap |
| ------------------- | ----------------------------------------------------------- | ------------: | ----------------: |
| `() -> ()`          | Function type that takes no arguments and returns a unit    |       8 Bytes |           0 Bytes |
| `(i32, i32) -> i32` | Function type that takes two i32 values and returns an i32  |       8 Bytes |           0 Bytes |
| `[](() -> ())`      | Array of functions that take no arguments and return a unit |       8 Bytes | 8 \* T + 32 Bytes |

#### NOTE:

While all globally defined functions have function types associated with them and can be referenced by their function types, function values are not yet fully implemented. As a result, constructs such as the following are not currently possible:

```
func foo(a: i32) -> i32 { return a; }

func main() -> i32 {
    bar: (i32) -> i32 = foo;
    return bar(10);
}
```

### Structs

Structs are a structure to bundle related Values together.
They have to be defined somewhere in the code like:

```
struct Foo {
	a: i32,
	b: char
}
```

Structs can be initalized and assigned via aggregate initalization:

```
f: Foo = Foo { 5, 'a' };
```

Their members can be accessed via member access operator `.`:

```
a: i32 = f.a;
```
