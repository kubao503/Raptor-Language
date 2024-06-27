#include <gtest/gtest.h>

#include "IRGenerator.hpp"
#include "lexer.hpp"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/TargetSelect.h"
#include "parser.hpp"
#include "print.hpp"

std::string captureStdout(std::function<void()> func) {
    std::streambuf* originalBuf = std::cout.rdbuf();
    std::ostringstream capturedOutput;
    std::cout.rdbuf(capturedOutput.rdbuf());

    func();

    std::cout.rdbuf(originalBuf);
    return capturedOutput.str();
}

void executeLLVMModule(std::unique_ptr<llvm::Module> module) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    llvm::sys::DynamicLibrary::AddSymbol("printValue", (void*)printValue);

    std::string errStr;
    llvm::ExecutionEngine* executionEngine =
        llvm::EngineBuilder(std::move(module)).create();

    if (!executionEngine) {
        llvm::errs() << "Failed to create ExecutionEngine: " << errStr << "\n";
        return;
    }

    llvm::Function* mainFunc = executionEngine->FindFunctionNamed("main");

    if (!mainFunc) {
        llvm::errs() << "Function 'main' not found in module.\n";
        return;
    }

    executionEngine->runFunction(mainFunc, {});
}

class CompilerTest : public testing::Test {
   protected:
    std::istringstream stream_;
    std::unique_ptr<Source> source_;
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<Parser> parser_;
    Program program_;
    IRGenerator irGenerator_;

    void Init(const std::string& input) {
        stream_ = std::istringstream(input);
        source_ = std::make_unique<Source>(stream_);
        lexer_ = std::make_unique<Lexer>(*source_);
        parser_ = std::make_unique<Parser>(*lexer_);
        program_ = parser_->parseProgram();
    }

    std::string compileAndGetOutput() {
        irGenerator_.genIR(program_);
        auto runModule = [&]() { executeLLVMModule(irGenerator_.takeModule()); };
        return captureStdout(runModule);
    }
};

TEST_F(CompilerTest, empty_program) {
    Init("");
    EXPECT_EQ(compileAndGetOutput(), "");
}

TEST_F(CompilerTest, print_new_line) {
    Init("print;");
    EXPECT_EQ(compileAndGetOutput(), "\n");
}

TEST_F(CompilerTest, print_int) {
    Init("print 5;");
    EXPECT_EQ(compileAndGetOutput(), "5\n");
}

TEST_F(CompilerTest, print_bool) {
    Init("print true;");
    EXPECT_EQ(compileAndGetOutput(), "true\n");
}