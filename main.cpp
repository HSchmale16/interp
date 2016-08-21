/**\file main.cpp
 * \author Henry J Schmale
 * \brief A very simple stack machine language with a single register.
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cstdio>

/** \brief Defines the commands that the interpretor understands.
 */
enum command_type {
    PUSH = 0,   //!< push n1 n2 .. nN on to stack
    POP,        //!< remove > values from the top of the stack
    IFEQ,       //!< if stack top is 0 jump to x instruction
    JUMP,       //!< Jump unconditionally to x instruction
    ADD,        //!< pops top 2 values and adds them, then pushes the result
    DUP,        //!< Pushes a copy of whatever was at the top of the stack
    PRINT,      //!< Prints the value at the top of the stack
    NOP,        //!< no operation
    STACKSZ,    //!< push the current stack size to the top of the stack
    PUSHA,      //!< pushes the value in _acc to stack top.
    LOADA,      //!< loads value at stack top into _acc
    HLT         //!< Halt the machine
};
const std::vector<std::string> command_type_Strings = {
    "PUSH", "POP", "IFEQ", "JUMP", "ADD", "DUP",
    "PRINT", "NOP", "STACKSZ", "PUSHA", "LOADA",
    "HLT"
};

/** \brief Defines a single command
 */
struct command {
    int lineno;                 //!< The memory address of command
    command_type type;          //!< Types of commands.
    std::vector<int> operands;  //!< Operands of the command

    command() {
        lineno = -1;
    }
};

std::string getTextForCommandType(command_type t) {
    return command_type_Strings[t];
}

command_type getCommandType(std::string cmd) {
    for(size_t i = 0 ; i < command_type_Strings.size(); i++) {
        if(command_type_Strings[i] == cmd) {
            return static_cast<command_type>(i);
        }
    }
    std::cerr << "WARNING: Unknown instruction converted to NOP"
              << std::endl;
    return NOP;
}

/**\return true if the command is valid
 */
bool verifyCommand(const command& command) {
    return false;
}

/**\brief Parse a single instruction line
 * \return a command struct. lineno will be -1 on error
 */
command parseCommand(std::string str) {
    command cmd;
    if(str[0] == '#'){
        // is a comment
        cmd.lineno = -2;
        return cmd;
    }

    std::stringstream sstr(str);
    std::string word;
    sstr >> cmd.lineno;                 // fetch the address location
    sstr >> word;                       // load the instruction
    cmd.type = getCommandType(word);

    // parse out the operands
    int operand;
    while(sstr >> operand) {
        cmd.operands.push_back(operand);
    }
    return cmd;
}

void printCommand(command cmd) {
    std::cout << cmd.lineno << " "
              << getTextForCommandType(cmd.type) << " ";
    for(auto op : cmd.operands) {
        std::cout << op << " ";
    }
    std::cout << std::endl;
}

/** The virtual machine
 */
class Interp {
private:
    std::map<int, command> _instruct;
    std::map<command_type, int> _instructHit;
    std::vector<int> _stack;
    int _progCounter;
    int _maxInstruct;
    int _acc;

    void pushStack(std::vector<int> values) {
        for(size_t i = 0; i < values.size(); i++) {
            _stack.push_back(values[i]);
        }
    }

    void pushStack(int val){
        _stack.push_back(val);
    }

    /** Pops x values from the stack. The last value popped gets placed
     *  in the accumulator.
     */
    void popStack(int times) {
        int i = 0, x;
        do {
            x = _stack.back();
            _stack.pop_back();
            i++;
        } while(i < times);
        _acc = x;
    }

    void print() {
        int chr = _stack.back();
        if(isprint(chr) || iscntrl(chr)) {
            std::cout << (char)chr;
        } else {
            std::cout << chr;
        }
        std::cout.flush();
    }

    void dup() {
        _stack.push_back(_stack.back());
    }

    void add() {
        int a = _stack.back();
        _stack.pop_back();
        int b = _stack.back();
        _stack.pop_back();
        _stack.push_back(a + b);
    }

    void jump(int pos) {
        if(_instruct.find(pos) != _instruct.end()) {
            _progCounter = pos;
        }else{
            std::cerr << "Nonexistant Instruction to jump" << std::endl;
        }
    }

    void loada() {
        _acc = _stack.back();
    }
public:
    Interp() {}

    /**\todo Check to make sure there is a halt in the instructions
     */
    void startInterp() {
        _progCounter = _instruct.begin()->first;
        _maxInstruct = _instruct.rbegin()->first;
        _stack.clear();
        _acc = 0;
    }

    void addInstruct(command cmd) {
        _instruct[cmd.lineno] = cmd;
    }

    int stacksz() {
        int sz = _stack.size();
        _stack.push_back(sz);
        //std::cerr << "ssz: " << sz << std::endl;
        return sz;
    }

    /**\brief Steps the virtual machine
     * \return 1 if the machine should HALT
     */
    bool step() {
        command& cmd = _instruct[_progCounter];
        _instructHit[cmd.type]++;
        switch(cmd.type) {
        case NOP: // No action
            break;
        case POP:
            if(cmd.operands.size() > 0)
                this->popStack(cmd.operands[0]);
            else
                this->popStack(1);
            break;
        case PUSH:
            this->pushStack(cmd.operands);
            break;
        case PUSHA:
            this->pushStack(_acc);
            break;
        case LOADA:
            this->loada();
            break;
        case ADD:
            this->add();
            break;
        case PRINT:
            this->print();
            break;
        case DUP:
            this->dup();
            break;
        case STACKSZ:
            this->stacksz();
            break;
        case JUMP:
            if(cmd.operands.size() < 1){
                std::cerr << "JUMP Command is invalid\n";
                return false;
            }else
                this->jump(cmd.operands[0]);
            return true;
        case IFEQ:
            if(cmd.operands.size() < 1){
                std::cerr << "IFEQ is invalid\n";
                return false;
            }else{
                if(_stack.back() == 0){
                    this->jump(cmd.operands[0]);
                    // this command modifies the program counter
                    return true;
                }
            }
            break;
        case HLT:
            return false;
        }
        // update prog counter
        auto instructIt = _instruct.find(_progCounter);
        if(instructIt == _instruct.end()) {
            return false;
        } else {
            instructIt++;
        }
        _progCounter = instructIt->first;
        return true;
    }
};

int main() {
    std::string line;
    Interp interp;

    int fileLineno = 1;
    while(getline(std::cin, line)) {
        command cmd = parseCommand(line);
        if(cmd.lineno == -1) {
            std::cerr << "BAD COMMAND " << fileLineno << std::endl;
        } else if(cmd.lineno < 0) {
            // capture other invalid commands that aren't syntax errors
        } else {
            //printCommand(cmd);
            interp.addInstruct(cmd);
        }
        fileLineno++;
    }
    interp.startInterp();
    while(interp.step());
    return 0;
}
