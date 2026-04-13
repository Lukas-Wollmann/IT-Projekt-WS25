# IT-Projekt-WS25

## Build from source

To build this project you should use docker. You can use the provided Dockerfile to build an image:

```shell
docker build . --tag it-projekt-ws25
docker run -it -v ".:/workspace" -w /workspace it-projekt-ws25
```

Run this command while in the root directory of this project. This will open an interactive terminal inside
your newly created docker image. When typing `ls` you should see all the project files. Now you can build by using:

```shell
cmake -B cmake-build
cmake --build cmake-build -t app -j
```

The built artifact can be found in `./cmake-build/app`. Just execute the file and see instructions on how to use it.

## Mitglieder:

- Jonas Ewert
- Phillip Reiß
- Leon Vogel
- Lukas Wollmann

## Ziel:

Ein Compiler/Transpiler für eine C-ähnliche Sprache, welche einige Probleme der Sprache C lösen soll, unter anderem
Speichersicherheit. Hierzu soll das Konzept der SmartPointer aus der Sprache C++ angewendet und erzwungen werden.
RawPointer sollen weiterhin möglich sein, allerdings nur durch explizites angeben des Nutzers. Desweiteren wollen wir
Array Zugriffe sicherer machen. Dabei soll der Fokus nur etwas in Richtung Sicherheit geschoben werden und nicht ein
völlig extremer Weg eingeschlagen werden, wie bspw. in der Programmiersprache Rust. Die Sprache soll entweder bis zum
Three-Address-Code (TAC) kompiliert, oder zu C transpiliert werden.

## Feature List (aktuell geplant):

- Funktionen
- If/While/For
- Struct/Klassen ähnliches Konzept
- SmartPointer per default
- Raw Pointer möglich (nur explizit)
- sicherer Array Zugriff
- In/Output

## Optional (wenn Zeit reicht)

- Tupel
- Template/Generic
- konstante Variablen
- Tagged Union
- Channel (prooducer-consumer)
- Multithreading
- Lambda Funktionen
- Referenzen

## Resources
https://dreampuf.github.io/GraphvizOnline/

- Tabs dont work in error handler

## Zum zeigen für den 17.03:
## Pointers

We need to distinguish between temporary and non-temporary pointers.
Temporary pointers can be moved into storage, meaning we do not need to increment the refcount,
we just need to make sure, the temporary expression is not cleaned up:

new i32(1);             // This is a temporary, it will be cleaned up after the ';'
a: *i32 = new i32(1);   // We MOVE the temporary into the variable a, now its not getting cleaned up
                        // after the ';'.

Variables are non-temporary storage, meaning we can not just move the pointer. We need to COPY the 
pointer, that means increment the reference count via the ocn-runtime.
b: *i32 = a;

With these rules we can implement things like 
Variable assignments:           x: i32 = y;
Assignments                     x = y;
Dereference Assignments:        *x = 10;

For pointers and recursive pointers:
a: **i32 = new *i32(new i32(10)); 

Now *a = new i32(20); needs to cleanup the old pointer etc.

## Type System Refactor

Früher wenn man einen Typ hatte, dann waren die nicht "canonical", das heißt:

*i32 war ein Object aber die Adresse des Objects war nicht unbedingt gleich mit einem anderen *i32 
Object. Typen wurden also mit einem überladenem == Operator verglichen. Jetzt haben wir eine Type
registry.

Warum?: Structs

## Structs !!!! WIP !!!!

Syntax:
struct Foo {
    a: i32,
    b: char
}

Trailing commas entfernt in Param list, Arg list und struct member list.
Structs are sem analysed for recursive structures via. simple graph algorithm.

struct Vector {
    a: Number,
    b: Number
}

struct Number {
    a: Vector
}

dann geht das natürlich nicht. Aber das geht schon:

struct Child {
    m: *Parent,
    f: *Parent
}

struct Parent {
    c:Child
}

