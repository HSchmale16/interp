#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

enum command_type {
    PUSH = 0,   //!< push <number1> <number2> <number n> on to stack
    POP,        //!< remove <x> values from the top of the stack
    IFEQ,       //!< if stack top is 0 jump to <x> instruction
    JUMP,       //!< Jump unconditionally to <x> instruction
    ADD,        //!< pops top 2 values and adds them, then pushes the result
    DUP,        //!< Pushes a copy of whatever was at the top of the stack
    PRINT,      //!< Prints the value at the top of the stack
    NOP,        //!< no operation
    HLT         //!< Halt the machine
};
const std::vector<std::string> command_type_Strings = {
    "PUSH", "POP", "IFEQ", "JUMP", "ADD", "DUP",
    "PRINT", "NOP", "HLT"
};


struct command {
    int lineno;
    command_type type;
    std::vector<int> operands;

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
    return NOP;
}

/**\return true if the command is valid
 */
bool verifyCommand(const command& command) {
    return false;
}

command parseCommand(std::string str) {
    command cmd;
    std::stringstream sstr(str);
    std::string word;
    sstr >> cmd.lineno;                 //!< fetch the address location
    sstr >> word;                       //!< load the instruction
    cmd.type = getCommandType(word);
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

/** The interpretor or virtual machine
 */
class Interp {
private:
    std::map<int, command> _instruct;
    std::vector<int> _stack;
    int _progCounter;
    int _maxInstruct;

    void pushStack(std::vector<int> values) {
        for(size_t i = 0; i < values.size(); i++) {
            _stack.push_back(values[i]);
        }
    }

    void popStack(int times) {
        std::cerr << "pop:" << _stack.size() << ' ';
        int i = 0;
        do {
            _stack.pop_back();
            i++;
        }while(i < times);
        std::cerr << _stack.size() << std::endl;
    }

    void print() {
        std::cout << _stack.back() << std::endl;
    }
public:
    Interp() {}

    void startInterp() {
        _progCounter = _instruct.begin()->first;
        _maxInstruct = _instruct.rbegin()->first;
        _stack.clear();
    }

    void addInstruct(command cmd) {
        _instruct[cmd.lineno] = cmd;
    }

    bool step() {
        command& cmd = _instruct[_progCounter];
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
        case PRINT:
            this->print();
            break;
        case HLT:
            return false;
        }
        // update prog counter
        auto instructIt = _instruct.find(_progCounter);
        if(instructIt == _instruct.end()){
            return false;
        }else{
            instructIt++;
        }
        _progCounter = instructIt->first;
        return true;
    }
};

int main() {
    std::string line;
    Interp interp;

    while(getline(std::cin, line)) {
        command cmd = parseCommand(line);
        if(cmd.lineno == -1) {
            std::cout << "BAD COMMAND" << std::endl;
        } else {
            //printCommand(cmd);
            interp.addInstruct(cmd);
        }
    }
    interp.startInterp();
    while(interp.step());

    return 0;
}