#include <istream>
#include <variant>

struct Token {
    enum class Type {
        INT_CONST,
        BOOL_CONST,
        WHILE_KW,
    };

    Type type;
    std::variant<int, float, bool, std::string> value;
};

class Lexer {
   private:
    std::istream& stream;

   public:
    Lexer(std::istream& stream);
    Token getToken();
};
