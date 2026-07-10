## 3. Values

Values are concrete data belonging to a type. There are different ways of creating them.

### Literals
Certain types have literals to construct values of them. Literal types have a `temporary` lifetime, 
that means they are deconstructed at the end of the expresison unless moved. 

#### Numeric Literal
Any positive or negative integer number that fits into 32 bits.

Examples: `-1000` `42` `0` `20` \
Type: `i32`

#### Boolean Literal

Examples: `true` `false` \
Type: `bool`

#### Character Literal
Any UTF-32 character is allowed, especial characters can be escaped using `\`.

Examples: `'A'` `'1'` `'🌸'` `'\''` \
Type: `char`   

#### String Literal
Any sequence of UTF-32 characters is allowed, especial characters can be escaped using `\`.

Examples: `"Hello World 🌍 \n"` \
Type: `[]char`

#### null Literal
The `null` literal denotes the absence of a value. It can be assigned to any pointer or array type.

Examples: `null` \
Type: `*T`, `[]T` where `T` is any type

### Structs
Structs can be initalized via aggregate initalization:
 
```
struct Foo { a: i32, b: char }
...
Foo { 5, 'a' };

```