#Best way to understand the Compilation model by trying below commands

# Stage 1: preprocessor only — just text substitution
g++ -E main.cpp -o main.i
wc -l main.i          # -> 1063 lines, from a 5-line source (headers pasted in)

# Stage 2: compile down to assembly
g++ -S main.i -o main.s
grep -A 12 "^main:" main.s
# key line inside: call _Z6squarei@PLT
# -> a call to a NAME, not a real address. Compiler doesn't know where square() lives yet.

# Stage 3: assemble to object files
g++ -c main.s -o main.o
g++ -c math_utils.s -o math_utils.o

// (each .cpp compiled independently --> that's the concept behind parallel compilation)

nm -C main.o
#                  U square(int)   <- Undefined: "I call this, fill in the address later"
# 0000000000000000 T main          <- Defined (Text section)
#                  U printf

nm -C math_utils.o
# 0000000000000000 T square(int)   <- fully defined here

# Proof main.o alone is unusable:
g++ main.o -o broken
# /usr/bin/ld: main.o: in function `main':
# main.cpp:(.text+0x12): undefined reference to `square(int)'

# The literal "hole" waiting to be patched:
objdump -dr main.o | grep -B3 -A1 "square"
#  11: e8 00 00 00 00        call   16 <main+0x16>      <- 4 zero bytes = placeholder
#      12: R_X86_64_PLT32    _Z6squarei-0x4              <- relocation entry: "patch this"

# Stage 4: link both object files -> linker finds the definition, patches the address
g++ main.o math_utils.o -o program
./program        # -> 25

nm -C program | grep -E "square|main$"
# 0000000000001182 T square(int)   <- real address now, no more "U"
# 0000000000001149 T main

# Bonus: ODR violation demo (two definitions of square)
echo "int square(int x) { return x*x + 1; }" > math_utils2.cpp
g++ -c math_utils2.cpp -o math_utils2.o
g++ main.o math_utils.o math_utils2.o -o broken2
# /usr/bin/ld: math_utils2.o: in function `square(int)':
# math_utils2.cpp:(.text+0x0): multiple definition of `square(int)'

#Dynamic linking

## Linux / WSL (.so)

# 1. Compile with -fPIC (Position Independent Code --> (currentAddress + fixed Offset)) — required for shared libs
g++ -c -fPIC math_utils.cpp -o math_utils.o

# 2. Link into a shared object
g++ -shared math_utils.o -o libmath_utils.so

# 3. Compile main.cpp and link against the shared lib (This is required to check that funcs used are defined in shared lib or not)
g++ main.cpp -L. -lmath_utils -o program_dynamic

nm -D libmath_utils.so | grep square
# 0000000000001119 T square(int)   <- exported dynamic symbol, lives in the .so, not in program_dynamic

ldd program_dynamic
# libmath_utils.so => not found        <- loader can't find it yet (not on the search path)

# 4. Run — the loader needs to find libmath_utils.so at runtime
LD_LIBRARY_PATH=. ./program_dynamic
# 25                                    <- works once the loader knows where to look

## Windows (MinGW-w64 g++) -- produces a .dll instead of a .so

g++ -shared -o math_utils.dll math_utils.cpp -Wl,--out-implib,libmath_utils.dll.a
g++ main.cpp -L. -lmath_utils.dll -o program_dynamic.exe
./program_dynamic.exe
# 25   <- math_utils.dll must sit next to the exe (or be on PATH) at run time

# Key takeaway: with static linking (Stage 4), the "hole" from Stage 3 gets patched once, permanently, into the exe by the linker. With dynamic linking, the hole is left open in program_dynamic and patched by the OS loader every time you run it -- which is why a missing .so/.dll fails at STARTUP, not at compile/link time.
