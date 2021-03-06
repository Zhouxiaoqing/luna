#include "VM.h"
#include "State.h"
#include "Table.h"
#include "Function.h"
#include <assert.h>

namespace luna
{
#define GET_CONST_VALUE(i)      (proto->GetConstValue(Instruction::GetParamB(i)))
#define GET_REGISTER_A(i)       (call->register_ + Instruction::GetParamA(i))
#define GET_REGISTER_B(i)       (call->register_ + Instruction::GetParamB(i))
#define GET_REGISTER_C(i)       (call->register_ + Instruction::GetParamC(i))
#define GET_UPVALUE_B(i)        (cl->GetUpvalue(Instruction::GetParamB(i)))
#define SET_NEW_TOP(a)          (state_->stack_.IncToNewTop(a + 1))

    VM::VM(State *state) : state_(state)
    {
    }

    void VM::Execute()
    {
        assert(!state_->calls_.empty());

        while (!state_->calls_.empty())
            ExecuteFrame();
    }

    void VM::ExecuteFrame()
    {
        CallInfo *call = &state_->calls_.back();
        Closure *cl = call->func_ ? call->func_->closure_ : nullptr;
        Function *proto = cl ? cl->GetPrototype() : nullptr;
        Value *a = nullptr;
        Value *b = nullptr;

        while (call->instruction_ < call->end_)
        {
            Instruction i = *call->instruction_++;

            switch (Instruction::GetOpCode(i)) {
                case OpType_LoadConst:
                    a = GET_REGISTER_A(i);
                    b = GET_CONST_VALUE(i);
                    *a = *b;
                    SET_NEW_TOP(a);
                    break;
                case OpType_Move:
                    a = GET_REGISTER_A(i);
                    b = GET_REGISTER_B(i);
                    *a = *b;
                    SET_NEW_TOP(a);
                    break;
                case OpType_Call:
                    a = GET_REGISTER_A(i);
                    if (Call(a, i)) return ;
                    break;
                case OpType_SetTop:
                    a = GET_REGISTER_A(i);
                    state_->stack_.SetNewTop(a);
                    break;
                case OpType_GetUpTable:
                    GetUpTable(GET_REGISTER_A(i), GET_UPVALUE_B(i), GET_REGISTER_C(i));
                    break;
            }
        }

        // For bootstrap CallInfo, we use call->register_ as new top
        Value *new_top = call->func_ ? call->func_ : call->register_;
        // Reset top value
        state_->stack_.SetNewTop(new_top);
        // Set expect result
        if (call->expect_result != EXP_VALUE_COUNT_ANY)
            state_->stack_.SetNewTop(new_top + call->expect_result);

        // Pop current CallInfo, and return to last CallInfo
        state_->calls_.pop_back();
    }

    bool VM::Call(Value *a, Instruction i)
    {
        int expect_result = Instruction::GetParamsBx(i);
        if (a->type_ == ValueT_Closure)
        {
            // We need enter next ExecuteFrame
            CallClosure(a, expect_result);
            return true;
        }
        else if (a->type_ == ValueT_CFunction)
        {
            CallCFunction(a, expect_result);
            return false;
        }
        else
        {
            // TODO: report error
            return true;
        }
    }

    void VM::CallClosure(Value *a, int expect_result)
    {
        CallInfo callee;
        Function *callee_proto = a->closure_->GetPrototype();

        callee.func_ = a;
        callee.instruction_ = callee_proto->GetOpCodes();
        callee.end_ = callee.instruction_ + callee_proto->OpCodeSize();
        callee.expect_result = expect_result;

        Value *arg = a + 1;
        Value *top = state_->stack_.top_;
        int fixed_args = callee_proto->FixedArgCount();

        // Fixed arg start from base register
        if (callee_proto->HasVararg())
        {
            callee.register_ = top;
            int count = top - arg;
            for (int i = 0; i < count && i < fixed_args; ++i)
                *top++ = *arg++;
        }
        else
        {
            callee.register_ = arg;
        }

        state_->stack_.SetNewTop(callee.register_ + fixed_args);
        state_->calls_.push_back(callee);
    }

    void VM::CallCFunction(Value *a, int expect_result)
    {
        // Push the c function CallInfo
        CallInfo callee;
        callee.register_ = a + 1;
        callee.func_ = a;
        callee.expect_result = expect_result;
        state_->calls_.push_back(callee);

        // Call c function
        CFunctionType cfunc = a->cfunc_;
        int res_count = cfunc(state_);

        Value *src = nullptr;
        if (res_count > 0)
            src = state_->stack_.top_ - res_count;

        // Copy c function result to caller stack
        Value *dst = a;
        for (int i = 0; i < res_count && i < expect_result; ++i)
            *dst++ = *src++;

        // Set all register nil after dst and set new stack top pointer
        state_->stack_.SetNewTop(dst);
        state_->stack_.SetNewTop(a + expect_result);

        // Pop the c function CallInfo
        state_->calls_.pop_back();
    }

    void VM::GetUpTable(Value *dst, Value *t, Value *k)
    {
        if (t->type_ != ValueT_Table)
        {
            // TODO: report error
            return ;
        }

        *dst = t->table_->GetValue(*k);
    }
} // namespace luna
