#include <exception>
#include <string>

class InvalidToken : public std::exception {
   public:
    InvalidToken(char c) : invalidChar(c) {}

   private:
    char invalidChar;
};
