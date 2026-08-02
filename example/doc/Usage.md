## 3. Usage

### Basic Variables

```
func main() -> i32 {
	a: i32 = 42;
	b: i32 = 69;
	c: i32 = a + c;
	print_i32(c);
	return 0;
}
```

### Functions

```
func main() -> i32 {
	a: i32 = 42;
	a = double_value(a);
	print_i32(a);
	return 0;
}

func double_value(i: i32) -> i32 {
	return i + i;
}
```

### Pointer

```
func main() -> i32 {
	a: *i32;								// 1. Only space for pointer, no pointer creation
	a = get_pointer_to(42);					// 5. pointer gets assigned
	print_i32(*a);							// OK
	return 0;
}											// a dies, pointer gets dropped

func get_pointer_to(i: i32) -> *i32 {
	b: *i32 = new i32(i);					// 2. pointer created
	return b;								// 3. pointer gets copied
}											// 4. b dies
```

### Control Flow

```
func main() -> i32 {
	a: i32 = 42;
	a = clamp(a, 10, 20);
	print_i32(a);
	return 0;
}

func clamp(i: i32, min: i32, max: i32) -> i32 {
	if (i < min) {
		return min;
	}
	if (i > max) {
		return max;
	}
	return i;
}
```

### Scopes / Lifetimes

```
func main() -> i32 {
	a: i32 = 42;
	{
		b: i32 = a * 2;
		print_i32(b);		// OK
	}						//  <- b dies here
	print_i32(a);			// OK
	print_i32(b);			// ERR
	return 0;
}
```

### Scopes / Lifetimes (Pointers)

```
func main() -> i32 {
	a: *i32;
	{
		b: *i32 = new i32(42);		// Pointer created here
		a = b;						// Pointer copied here
		print_i32(b);
	}								//  <- b dies here, Pointer persists through a
	print_i32(b);					// ERR, b not in scope
	print_i32(a);					// OK, pointer alive
	return 0;
}
```

### Structs

```
struct Person {
	age: i32,
	name: []char,
	glasses: bool
}

func main() -> i32 {
	p: Person;
	p = Person { 22, "Wolle", true };
	q: Person = Person { 21, "Phillip", false };

	if (p.age > q.age) {
		print_str(p.name);
		print_str(" is older\n");
	} else if (p.age < q.age) {
		print_str(q.name);
		print_str(" is older\n");
	} else  {
		print_str(p.name);
		print_str(" und ");
		print_str(q.name);
		print_str(" are the same age\n");
	}
	return 0;
}
```

### Structs (with Pointer)

```
struct Linked_List_str {
	content: []char,
	next: *Linked_List_str
}

func main() -> i32 {
	head: *Linked_List_str = new Linked_List_str { "Hello", null };
	append(head, ",");
	append(head, " ");
	append(head, "World");
	append(head, "!");
	append(head, " ");
	append(head, "🌍");
	append(head, "\n");

	current: *Linked_List_str = head;
	while(*current.next != null){
		print_str(*current.content);
		current = *current.next;
	}
	print_str(*current.content);

	return 0;
}

func append(head: *Linked_List_str, content: []char) {
	while(*head.next != null) {
		head = *head.next;
	}
	*head.next = new Linked_List_str { content, null };
}
```
