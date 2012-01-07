#include "Error.h"
#include <sstream>

namespace lua
{
    // static
    void LexError::ThrowError(Type type, int line, int column, const std::string& desc)
    {
        throw LexError(type, line, column, desc);
    }

    std::string LexError::ConvertToReadable(const LexError& error)
    {
        std::ostringstream es;
        es << "Error [" << error.error_line << "]: ";
        switch (error.type)
        {
        case NO_MULTILINE_COMMENT_ENDER:
            es << "no expcet multi-line ender" << " \"" << error.desc_helper << "\"";
            break;
        case NO_STRING_ENDER:
            es << "no expect string ender" << " \"" << error.desc_helper << "\"";
            break;
        case NO_LONG_STRING_ENDER:
            es << "no expect long string ender" << " \"" << error.desc_helper << "\"";
            break;
        case NO_COMPLETE_NOT_EQUAL_OP:
            es << error.desc_helper << " expect '='";
            break;
        case INVALIDATE_NUMBER:
            es << "invalidate number here \"" << error.desc_helper << "\"";
            break;
        case ILLEGAL_CHARACTER:
            es << "illegal char is here";
            break;
        default:
            es << "unkown error";
            break;
        }

        es << " at column " << error.error_column << ".";

        return es.str();
    }

    void ParserError::ThrowError(int line, int column, const std::string& desc)
    {
        throw ParserError(line, column, desc);
    }
} // namespace lua