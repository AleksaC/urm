# URM Simulator

This repositroy contains [URM](https://proofwiki.org/wiki/Definition:Unlimited_Register_Machine)
simulator written in C along with some example URM programs. This was built in
collaboration with [Petar Vujović](https://github.com/petarvujovic98) as a
homework for our computability theory class.

## About 📖
URM (Unlimited Register Machine) is an abstraction of computing device.
As its name suggests the machine has unlimited registers whose values are natural
numbers. The machine executes instructions one after another in sequential order
unless jump instruction is encountered and its condition is satisfied. URM can
execute 4 types of instructions:
- Zero instruction `Z(i)` - sets *i-th* register to zero
- Successor `S(i)` - sets the value of *i-th* register to its successor
(increments it by one)
- Transfer `T(i, j)` - sets the value of *i-th* register to the value of *j-th* register
- Jump `J(i, j, k)` - jumps to *k-th* instruction if contents of the *i-th* and *j-th*
register match, otherwise it proceeds to the next instruction

The program terminates when reaches the instruction outside of the range of instructions
in the program. The result of the computation should be stored in the first register.

The simulator can execute arbitrary URM programs. In addition you can add comments beginning
with `#` and add labels indicating instruction number (e.g. `1: S(3)`). These labels are not
considered during the execution of the program, so if an instruction is mislabeled it will
not affect how the program is executed but you will be warned that the label number doesn't
match the instruction number.

When running a simulator you need to provide the path to your program as the first argument
and the initial register configuration as subsequent arguments. The rest of the registers
are considered to contain 0, but they will not be allocated until a write operation is
performed on them.

## Getting started ⚙️
```shell script
git clone https://github.com/AleksaC/urm
cd urm
make
./bin/release/simulator examples/min.txt 7 5 12
```
This will clone the repo, generate `simulator` binary in `bin/release` and run a program
that finds the minimum of 3 natural numbers (in this case 7, 3 and 12).

To compile the project in `debug` mode run:
```shell script
make debug
```
You can also build and run the program with the following command:
```shell script
make run ARGS="path/to/your_program.txt initial_register_configuration"
```

If you don't want to use make, the entire project can be compiled with the
following command:
```shell script
gcc -I ./include -O3 -std=c99 src/urm_simulator.c src/main.c -o bin/simulator
```
To get `debug` build replace `-O3` with `-g`.

We've used `gcc` to compile the project, but you can use `clang` as a drop-in
replacement by setting `CC = clang` in the `Makefile` or replacing `gcc` with
`clang` in the command above.

Once you've compiled the program you can run your own programs or the ones
provided in the [examples directory](https://github.com/AleksaC/urm/tree/master/examples).

## LICENSE️ ⚖️
[![license](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/AleksaC/urm/blob/master/LICENSE)

## TODO 📝
- [ ] Test on Windows

---

**Note**: This started as an attempt to write proper C, but due to lack of time we
quickly had to give up on that and write working C instead.
