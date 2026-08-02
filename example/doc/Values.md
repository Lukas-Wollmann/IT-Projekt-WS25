## 3. Values

Values are concrete data belonging to a type. There are different ways of creating them.

### Literals

Certain types have literals to construct values of them.

#### Numeric Literal

Any positive or negative integer number that fits into 32 bits.

Max: $2^{32}-1$ \
Min: $-2^{32}$

Examples: `-1000` `42` `0` `20` \
Type: `i32` \
Size: `8 Bytes`

#### Boolean Literal

Represents a binary truth state, primarily used for logical operations and control flow evaluation.

Examples: `true` `false` \
Type: `bool` \
Size: `1 Byte`

#### Character Literal

Any UTF-32 character is allowed, special characters can be escaped using `\`.

Examples: `'A'` `'1'` `'🌸'` `'\''` \
Type: `char` \
Size: `4 Bytes`

#### String Literal

Any sequence of UTF-32 characters is allowed, special characters can be escaped using `\`. Strings are not null-terminated.

Examples: `"Hello World 🌍 \n"` \
Type: `[]char` \
Size: `8 Bytes` (on Stack) \
Size: `32 Bytes + (length + 1) * 4 Bytes` (on Heap)

#### null Literal

The `null` literal denotes the absence of a value. It can be assigned to any pointer or array type.

Examples: `null` \
Type: `*T`, `[]T` where `T` is any type \
Size: size of pointer

#### unit Literal

The `()` literal denotes the absence of a Type. The unit Literal `()` can only be assigned to the unit Type `()`. The unit Type can represent the absence of a return value of a function. But this results in 1 unused Byte.

Examples: `()` \
Type: `()` \
Size: `1 unusable Byte` (unfortunately)

### Initialisation

#### Pointer Initialisation

Pointers must be initialized via `new`:

| Variant                | Behavior                                              |
| ---------------------- | ----------------------------------------------------- |
| `p: *i32;`             | makes space for a pointer and assigns null            |
| `p: *i32 = new i32;`   | initializes a pointer with default value              |
| `p: *i32 = new i32();` | initializes a pointer with default value              |
| `p: *T = new i32(42);` | initializes a pointer with value 42                   |
| `p: *T = q;`           | copies a pointer                                      |
| `p: *T = new i32(*q);` | initializes a pointer with value from another pointer |

Size: `8 Bytes` (on Stack) \
Size: `sizeof(T) + 16` (on Heap)

#### Array Initialisation

Arrays have a special `new`-like syntax:

| Variant                     | Behavior                                              |
| --------------------------- | ----------------------------------------------------- |
| `p: []i32 = array[10] i32;` | initializes an array with size 10 and standard values |
| `p: []i32 = q`              | ???                                                   |

Size: `sizeof(T) * length + 32`

#### Struct Initialisation

Structs can be initalized and assigned via aggregate initalization:

| Variant                 | Behavior                                             |
| ----------------------- | ---------------------------------------------------- |
| `p: P;`                 | initializes a struct with default values             |
| `p: P = P { a, b, c };` | initializes a struct with value a, b, c              |
| `p: P = q;`             | initializes a struct with values from another struct |

Size: `sum of all members + alignment + padding`
