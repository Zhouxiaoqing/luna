#include "Function.h"

namespace luna
{
    Function::Function()
        : module_(nullptr), line_(0), args_(0), is_vararg_(false),
          reg_allocator_(0), reg_count_(0), superior_(nullptr)
    {
    }

    const Instruction * Function::GetOpCodes() const
    {
        return opcodes_.empty() ? nullptr : &opcodes_[0];
    }

    std::size_t Function::OpCodeSize() const
    {
        return opcodes_.size();
    }

    void Function::AddInstruction(Instruction i, int line)
    {
        opcodes_.push_back(i);
        opcode_lines_.push_back(line);
    }

    void Function::SetHasVararg()
    {
        is_vararg_ = true;
    }

    bool Function::HasVararg() const
    {
        return is_vararg_;
    }

    void Function::SetFixedArgCount(int count)
    {
        args_ = count;
    }

    int Function::FixedArgCount() const
    {
        return args_;
    }

    void Function::SetBaseInfo(String *module, int line)
    {
        module_ = module;
        line_ = line;
    }

    void Function::SetSuperior(Function *superior)
    {
        superior_ = superior;
    }

    int Function::GetNextRegister() const
    {
        return reg_allocator_;
    }

    int Function::AllocaNextRegister()
    {
        int reg = reg_allocator_++;

        // Adjust register count
        if (reg_allocator_ > reg_count_)
            reg_count_ = reg_allocator_;

        return reg;
    }

    void Function::SetNextRegister(int reg)
    {
        reg_allocator_ = reg;
        if (reg_allocator_ > reg_count_)
            reg_count_ = reg_allocator_;
    }

    int Function::GetRegisterCount() const
    {
        return reg_count_;
    }

    void Function::SetRegisterCount(int count)
    {
        if (count > reg_count_)
            reg_count_ = count;
    }

    int Function::GetFreeRegisterCount() const
    {
        return kMaxRegisterCount - reg_allocator_;
    }

    int Function::AddConstNumber(double num)
    {
        Value v;
        v.type_ = ValueT_Number;
        v.num_ = num;
        return AddConstValue(v);
    }

    int Function::AddConstString(String *str)
    {
        Value v;
        v.type_ = ValueT_String;
        v.str_ = str;
        return AddConstValue(v);
    }

    int Function::AddConstValue(const Value &v)
    {
        const_values_.push_back(v);
        return const_values_.size() - 1;
    }

    Value * Function::GetConstValue(int i)
    {
        return &const_values_[i];
    }

    Closure::Closure()
        : prototype_(nullptr)
    {
    }

    Function * Closure::GetPrototype() const
    {
        return prototype_;
    }

    void Closure::SetPrototype(Function *prototype)
    {
        prototype_ = prototype;
    }

    Value * Closure::GetUpvalue(int index) const
    {
        int size = upvalues_.size();
        if (index < size)
        {
            auto &upvalue = upvalues_[index];
            return upvalue.type_ == Upvalue::Stack ?
                upvalue.stack_value_ : &upvalue.shared_->value_;
        }

        return nullptr;
    }

    int Closure::AddUpvalue(Value *value, Upvalue::Type type)
    {
        Upvalue upvalue;
        upvalue.stack_value_ = value;
        upvalue.type_ = type;
        upvalues_.push_back(upvalue);
        return upvalues_.size() - 1;
    }
} // namespace luna
