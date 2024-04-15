#ifndef DONSUS_CODEGEN_H
#define DONSUS_CODEGEN_H

#include "../parser.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
// opt
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"

#include <memory>

// Select platform
#ifdef _WIN32
#include "../src/codegen/platform/windows_platform.h"
#endif

#ifdef __unix__
#include "../../src/codegen/platform/linux_platform.h"
#endif

/*#ifdef _WIN32
using PlatformClass = Window
#endif*/

#ifdef __unix__
using PlatformClass = LinuxPlatform;
#endif

namespace DonsusCodegen {
Bitness GetBitness();
class Platform {
public:
  std::string
  GetLinkerCommand(const std::vector<std::filesystem::path> &obj_paths,
                   const std::filesystem::path &exe_path,
                   Bitness bitness) const {

    return obj.GetLinkerCommand(obj_paths, exe_path, bitness);
  }
  PlatformClass obj;
};
class DonsusCodeGenerator {
public:
  DonsusCodeGenerator() = default;
  DonsusCodeGenerator(std::unique_ptr<llvm::LLVMContext> context,
                      std::unique_ptr<llvm::Module> module,
                      std::unique_ptr<llvm::IRBuilder<>> builder);

  void Finish() const;

  void Link() const;

  int create_object_file();

  // run llvm's default optimisation pipeline
  void default_optimisation();

  void create_entry_point();

  std::vector<llvm::Type *>
      parameters_for_function(std::vector<NAME_DATA_PAIR>);

  llvm::StructType multiple_return_types(std::vector<DONSUS_TYPE>);

  void compile_donsus_expr(DonsusParser::end_result &ast);
  llvm::Value *compile(utility::handle<donsus_ast::node> &node,
                       utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(utility::handle<donsus_ast::node> &ast,
                     donsus_ast::variable_decl &ca_ast,
                     utility::handle<DonsusSymTable> &table,
                     bool is_definition = false);

  llvm::Value *visit(utility::handle<donsus_ast::node> &ast,
                     donsus_ast::assignment &ac_ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(donsus_ast::identifier &ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(utility::handle<donsus_ast::node> &ast,
                     donsus_ast::number_expr &ac_ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(donsus_ast::function_decl &ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(donsus_ast::function_def &ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(donsus_ast::function_call &ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(donsus_ast::if_statement &ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(donsus_ast::else_statement &ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(utility::handle<donsus_ast::node> &ast,
                     donsus_ast::return_kw &ca_ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(donsus_ast::string_expr &ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(utility::handle<donsus_ast::node> &ast,
                     donsus_ast::expression &ca_ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(utility::handle<donsus_ast::node> &ast,
                     donsus_ast::print_expr &ca_ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(utility::handle<donsus_ast::node> &ast,
                     donsus_ast::bool_expr &ca_ast,
                     utility::handle<DonsusSymTable> &table);

  llvm::Value *visit(utility::handle<donsus_ast::node> &ast,
                     donsus_ast::unary_expr &ca_ast,
                     utility::handle<DonsusSymTable> &table);
  llvm::Value *printf_format(utility::handle<donsus_ast::node> node);

  llvm::Type *map_type(DONSUS_TYPE type);
  // meta
  llvm::BasicBlock *main_block;
  std::unique_ptr<llvm::LLVMContext> TheContext;
  std::unique_ptr<llvm::IRBuilder<>> Builder;
  std::unique_ptr<llvm::Module> TheModule;
  Platform platform;
};

} // namespace DonsusCodegen
#endif