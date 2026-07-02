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

// (each .cpp compiled independently)

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
