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
- Robin Nowak
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

# TODO

add BOOL_LIT in lexer
two combined emojis like 🖕🏿 do not work: "🖕🏿🏿🏿"

## Resources
https://dreampuf.github.io/GraphvizOnline/