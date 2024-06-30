#include <gtest/gtest.h>

#include "ir_generator.hpp"
#include "lexer.hpp"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/TargetSelect.h"
#include "parser.hpp"
#include "print.hpp"
#include "semantic_errors.hpp"

using compiler::IRGenerator;

std::string captureStdout(std::function<void()> func) {
    std::streambuf* originalBuf = std::cout.rdbuf();
    std::ostringstream capturedOutput;
    std::cout.rdbuf(capturedOutput.rdbuf());

    func();

    std::cout.rdbuf(originalBuf);
    return capturedOutput.str();
}

void executeLLVMModule(std::unique_ptr<llvm::Module> module) {
    llvm::ExecutionEngine* executionEngine =
        llvm::EngineBuilder(std::move(module)).create();

    if (!executionEngine) {
        llvm::errs() << "Failed to create ExecutionEngine.\n";
        return;
    }

    llvm::Function* mainFunc = executionEngine->FindFunctionNamed("main");

    if (!mainFunc) {
        llvm::errs() << "Function 'main' not found in module.\n";
        return;
    }

    executionEngine->runFunction(mainFunc, {});
    delete executionEngine;
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

    static void SetUpTestSuite() {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();

        llvm::sys::DynamicLibrary::AddSymbol("printValue", (void*)printValue);
    }

    std::string executeAndGetOutput() {
        irGenerator_.genIR(program_);
        auto runModule = [&]() { executeLLVMModule(irGenerator_.takeModule()); };
        return captureStdout(runModule);
    }

    template <typename Exception>
    void executeAndExpectThrowAt(Position position) {
        EXPECT_THROW(
            {
                try {
                    executeAndGetOutput();
                } catch (const Exception& e) {
                    EXPECT_EQ(e.getPosition().line, position.line);
                    EXPECT_EQ(e.getPosition().column, position.column);
                    throw;
                }
            },
            Exception);
    }
};

TEST_F(CompilerTest, empty_program) {
    Init("");
    EXPECT_EQ(executeAndGetOutput(), "");
}

TEST_F(CompilerTest, print_new_line) {
    Init("print;");
    EXPECT_EQ(executeAndGetOutput(), "\n");
}

TEST_F(CompilerTest, print_int) {
    Init("print 5;");
    EXPECT_EQ(executeAndGetOutput(), "5\n");
}

TEST_F(CompilerTest, print_bool) {
    Init("print true;");
    EXPECT_EQ(executeAndGetOutput(), "true\n");
}

TEST_F(CompilerTest, var_def) {
    Init(
        "int x = 5;"
        "print x;");
    EXPECT_EQ(executeAndGetOutput(), "5\n");
}

TEST_F(CompilerTest, var_not_found) {
    Init("print x;");
    executeAndExpectThrowAt<SymbolNotFound>({1, 7});
}