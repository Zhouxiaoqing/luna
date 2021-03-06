#ifndef FUNCTION_H
#define FUNCTION_H

#include "GCObject.h"
#include "OpCode.h"
#include "String.h"
#include "Value.h"
#include <vector>

namespace luna
{
    // Function prototype class, all runtime function(closure) reference this
    // class object. This class contains some static information generated after
    // parse.
    class Function : public GCObject
    {
    public:
        Function();

        // Get function instructions and size
        const Instruction * GetOpCodes() const;
        std::size_t OpCodeSize() const;

        // Add instruction, 'line' is line number of the instruction 'i'
        void AddInstruction(Instruction i, int line);

        // Set and get this function has vararg
        void SetHasVararg();
        bool HasVararg() const;

        // Set and get fixed arg count
        void SetFixedArgCount(int count);
        int FixedArgCount() const;

        // Set module and function define start line
        void SetBaseInfo(String *module, int line);

        // Set superior function
        void SetSuperior(Function *superior);

        // Get next register index, but do not real alloca it
        int GetNextRegister() const;

        // Alloca next register
        int AllocaNextRegister();

        // Set next register
        void SetNextRegister(int reg);

        // Get register count
        int GetRegisterCount() const;

        // Set register count
        void SetRegisterCount(int count);

        // Get remain free register count
        int GetFreeRegisterCount() const;

        // Add const number and return index of the const value
        int AddConstNumber(double num);

        // Add const String and return index of the const value
        int AddConstString(String *str);

        // Add const Value and return index of the const value
        int AddConstValue(const Value &v);

        // Get const Value by index
        Value * GetConstValue(int i);

    private:
        static const int kMaxRegisterCount = 250;

        // function instruction opcodes
        std::vector<Instruction> opcodes_;
        // opcodes' line number
        std::vector<int> opcode_lines_;
        // const values in function
        std::vector<Value> const_values_;
        // function define module name
        String *module_;
        // function define line at module
        int line_;
        // count of args
        int args_;
        // has '...' param or not
        bool is_vararg_;
        // register allocator
        int reg_allocator_;
        // total register count
        int reg_count_;
        // superior function pointer
        Function *superior_;
    };

    // All runtime function are closure, this class object point to a
    // prototype Function object and add upvalues.
    class Closure : public GCObject
    {
    public:
        Closure();

        // Get and set closure prototype Function
        Function * GetPrototype() const;
        void SetPrototype(Function *prototype);

        // Get upvalue by index
        Value * GetUpvalue(int index) const;

        // Add upvalue by type, return upvalue index
        int AddUpvalue(Value *value, Upvalue::Type type);

    private:
        // prototype Function
        Function *prototype_;
        // upvalues
        std::vector<Upvalue> upvalues_;
    };
} // namespace luna

#endif // FUNCTION_H
