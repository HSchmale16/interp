# Interp
A simple stack-based virtual machine with just a few instructions. It does
almost all operations by using addition. It works exclusively on integers.

The machine consists of an infinite size stack and a single register. The
register shall be known as `_acc` in this document.

The register is initialized to 0 at machine start, and the stack is empty.

Example programs may be found in the `test` directory.

# Example Usage

    $BIN < $PROG

`$BIN` is the compiled executable.

`$PROG` is a given program file.

You can also type your program directly into the interpretor and close the
input stream to start running it.

# Instructions
This section describes the instructions as understood by the machine.

A instruciton looks like so:

    LINENO  INSTRUCTION [OP1 OP2 .. OPN]

## PUSH v1 [ .. vN ]
pushes the values on to the stack in order the values were specified from
left to right.

## POP [N]
Pops N values from the stack.

If N is not specifed then it assumes that one(1) value should be popped.

## IFEQ ADD
Tests if the stack top is equal to zero, then jumps to ADD

## IFAC ADD
Tests if the stack top and register are equal, then jumps to ADD

## JUMP ADD
Sets the program counter equal to ADD, for next cycle.

## DUP
Pushes a copy of the current stack top to the stack.

## PRINT
Prints the value at the top of the stack to stdout.

## NOP
No operation.

If an instruction that is not known is encountered during compilation, it
will be converted to this operation, and a warning will be printed on
stderr.

## STACKSZ
Pushes the current size of the stack to the top of the stack.

## PUSHA
Pushes the value in `_acc` to the top of the stack.

## LOADA
Writes the value at stack top into `_acc`.

## HLT
Stops the machine. This instruction is required at least once in a given
program if the program is halting. The presense of this instruction does not
mean that the program halts.
