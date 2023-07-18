#include <string>
#include <iostream>
#include <memory>
#include <cdk/types/types.h>
#include "targets/symbol.h"
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

//---------------------------------------------------------------------------

static std::string access_lvl_name(int access_lvl) {
    if (access_lvl == 1) {
        return "public";
    } else if (access_lvl == 0) {
        return "private";
    } else if (access_lvl == 2) {
        return "module";
    } else {
        return "unknown access level";
    }
}

void fir::xml_writer::do_nil_node(cdk::nil_node *const node, int lvl) {
    openTag(node, lvl);
    closeTag(node, lvl);
}


void fir::xml_writer::do_data_node(cdk::data_node *const node, int lvl) {
    // EMPTY
}

void fir::xml_writer::do_double_node(cdk::double_node *const node, int lvl) {
    process_literal(node, lvl);
}

void fir::xml_writer::do_not_node(cdk::not_node *const node, int lvl) {
    do_unary_operation(node, lvl);
}

void fir::xml_writer::do_and_node(cdk::and_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_or_node(cdk::or_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_alloc_node(fir::alloc_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    openTag(node, lvl);
    node->argument()->accept(this, lvl + 2);
    closeTag(node, lvl);
}

void fir::xml_writer::do_body_node(fir::body_node *const node, int lvl) {
//    std::cout << "xml_writer-> doin body node thingy" << std::endl;
    openTag(node, lvl);
    openTag("prologue", lvl);
    if (node->prologue()) {
        node->prologue()->accept(this, lvl + 4);
    }
    closeTag("prologue", lvl);
    openTag("block", lvl);
    if (node->block()) {
        node->block()->accept(this, lvl + 4);
    }
    closeTag("block", lvl);
    openTag("epilogue", lvl);
    if (node->epilogue()) {
        node->epilogue()->accept(this, lvl + 4);
    }
    closeTag("epilogue", lvl);
    closeTag(node, lvl);
}

void fir::xml_writer::do_block_node(fir::block_node *const node, int lvl) {
//    std::cout << "xml_writer-> doin block node thingy" << std::endl;
    openTag(node, lvl);
    openTag("declarations", lvl);
    if (node->declarations()) {
        node->declarations()->accept(this, lvl + 4);
    }
    closeTag("declarations", lvl);
    openTag("instructions", lvl);
    if (node->instructions()) {
        node->instructions()->accept(this, lvl + 4);
    }
    closeTag("instructions", lvl);
    closeTag(node, lvl);
}

void fir::xml_writer::do_func_call_node(fir::func_call_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
//    std::cout << "xml_writer-> doin function call thingy" << std::endl;
    os() << std::string(lvl, ' ') << "<" << node->label() << " name='" << node->identifier() << "'>" << std::endl;
    openTag("arguments", lvl);
    if (node->arguments()) {
        node->arguments()->accept(this, lvl + 4);
    }
    closeTag("arguments", lvl);
    closeTag(node, lvl);
}

void fir::xml_writer::do_func_decl_node(fir::func_decl_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
//    std::cout << "xml_writer-> doin function declaration thingy" << std::endl;

    os() << std::string(lvl, ' ') << "<" << node->label() << " name='" << node->identifier() << "' qualifier='"
         << access_lvl_name(node->access_lvl()) << "' type='" << cdk::to_string(node->type()) << "'>" << std::endl;
    openTag("arguments", lvl);
    if (node->arguments()) {
        _symtab.push();
        node->arguments()->accept(this, lvl + 4);
        _symtab.pop();
    }
    closeTag("arguments", lvl);
    closeTag(node, lvl);
}

void fir::xml_writer::do_func_def_node(fir::func_def_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
//    std::cout << "xml_writer-> doin function definition thingy" << std::endl;
    _function = new_symbol();
    reset_new_symbol();

    _inFunctionBody = true;
    _symtab.push();

    os() << std::string(lvl, ' ') << "<" << node->label() << " name='" << node->identifier() << "' qualifier='"
         << access_lvl_name(node->access_lvl()) << "' type='" << cdk::to_string(node->type()) << "'>" << std::endl;

    openTag("arguments", lvl);
    if (node->arguments()) {
        _inFunctionArgs = true;
        node->arguments()->accept(this, lvl + 4);
        _inFunctionArgs = false;
    }
    closeTag("arguments", lvl);
    node->body()->accept(this, lvl + 2);
    closeTag(node, lvl);

    _symtab.pop();
    _inFunctionBody = false;
}

void fir::xml_writer::do_pos_node(fir::pos_node *const node, int lvl) {
    openTag(node, lvl);

    openTag("argument", lvl);
    if (node->argument()) {
        node->argument()->accept(this, lvl + 2);
    }
    closeTag("argument", lvl);

    closeTag(node, lvl);
}

void fir::xml_writer::do_index_node(fir::index_node *const node, int lvl) {
    openTag(node, lvl);
    openTag("base", lvl);
    node->base()->accept(this, lvl + 2);
    closeTag("base", lvl);
    openTag("index", lvl);
    node->index()->accept(this, lvl + 2);
    closeTag("index", lvl);
    closeTag(node, lvl);
}

void fir::xml_writer::do_leave_node(fir::leave_node *const node, int lvl) {
    openTag(node, lvl);
    closeTag(node, lvl);
}

void fir::xml_writer::do_restart_node(fir::restart_node *const node, int lvl) {
    openTag(node, lvl);
    closeTag(node, lvl);
}

void fir::xml_writer::do_return_node(fir::return_node *const node, int lvl) {
    openTag(node, lvl);
    closeTag(node, lvl);
}

void fir::xml_writer::do_addressof_node(fir::addressof_node *const node, int lvl) {
    openTag(node, lvl);
    node->lvalue()->accept(this, lvl + 2);
    closeTag(node, lvl);
}

void fir::xml_writer::do_nullptr_node(fir::nullptr_node *const node, int lvl) {
    openTag(node, lvl);
    closeTag(node, lvl);
}

void fir::xml_writer::do_sizeof_node(fir::sizeof_node *const node, int lvl) {
    openTag(node, lvl);
    node->expression()->accept(this, lvl + 2);
    closeTag(node, lvl);
}

void fir::xml_writer::do_var_decl_node(fir::var_decl_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    reset_new_symbol();

    openTag(node, lvl);

    if (node->initializer()) {
        openTag("initializer", lvl + 2);
        node->initializer()->accept(this, lvl + 4);
        closeTag("initializer", lvl + 2);
    }
    closeTag(node, lvl);
}


//---------------------------------------------------------------------------

void fir::xml_writer::do_sequence_node(cdk::sequence_node *const node, int lvl) {
    os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
//    std::cout << "xml_writer-> sequence size: " << node->size() << std::endl;
    for (size_t i = 0; i < node->size(); i++) {
        if (node->node(i)) {
            node->node(i)->accept(this, lvl + 2);
        }
    }
    closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_integer_node(cdk::integer_node *const node, int lvl) {
    process_literal(node, lvl);
}

void fir::xml_writer::do_string_node(cdk::string_node *const node, int lvl) {
    process_literal(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_unary_operation(cdk::unary_operation_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    openTag(node, lvl);
    node->argument()->accept(this, lvl + 2);
    closeTag(node, lvl);
}

void fir::xml_writer::do_neg_node(cdk::neg_node *const node, int lvl) {
    do_unary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_binary_operation(cdk::binary_operation_node *const node, int lvl) {
    openTag(node, lvl);
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);
    closeTag(node, lvl);
}

void fir::xml_writer::do_add_node(cdk::add_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_sub_node(cdk::sub_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_mul_node(cdk::mul_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_div_node(cdk::div_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_mod_node(cdk::mod_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_lt_node(cdk::lt_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_le_node(cdk::le_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_ge_node(cdk::ge_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_gt_node(cdk::gt_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_ne_node(cdk::ne_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

void fir::xml_writer::do_eq_node(cdk::eq_node *const node, int lvl) {
    do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_variable_node(cdk::variable_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    os() << std::string(lvl, ' ') << "<" << node->label() << ">" << node->name() << "</" << node->label() << ">"
         << std::endl;
}

void fir::xml_writer::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    openTag(node, lvl);
    node->lvalue()->accept(this, lvl + 4);
    closeTag(node, lvl);
}

void fir::xml_writer::do_assignment_node(cdk::assignment_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    openTag(node, lvl);

    node->lvalue()->accept(this, lvl);
    reset_new_symbol();

    node->rvalue()->accept(this, lvl + 4);
    closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_evaluation_node(fir::evaluation_node *const node, int lvl) {
    openTag(node, lvl);
    node->argument()->accept(this, lvl + 2);
    closeTag(node, lvl);
}

void fir::xml_writer::do_print_node(fir::print_node *const node, int lvl) {
    openTag(node, lvl);
    node->exprs()->accept(this, lvl + 2);
    closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_read_node(fir::read_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    openTag(node, lvl);
    closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_while_node(fir::while_node *const node, int lvl) {
    openTag(node, lvl);
    openTag("condition", lvl + 2);
    node->condition()->accept(this, lvl + 4);
    closeTag("condition", lvl + 2);
    openTag("block", lvl + 2);
    node->block()->accept(this, lvl + 4);
    closeTag("block", lvl + 2);
    closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_if_node(fir::if_node *const node, int lvl) {
    openTag(node, lvl);
    openTag("condition", lvl + 2);
    node->condition()->accept(this, lvl + 4);
    closeTag("condition", lvl + 2);
    openTag("then", lvl + 2);
    node->block()->accept(this, lvl + 4);
    closeTag("then", lvl + 2);
    closeTag(node, lvl);
}

void fir::xml_writer::do_if_else_node(fir::if_else_node *const node, int lvl) {
    openTag(node, lvl);
    openTag("condition", lvl + 2);
    node->condition()->accept(this, lvl + 4);
    closeTag("condition", lvl + 2);
    openTag("then", lvl + 2);
    node->thenblock()->accept(this, lvl + 4);
    closeTag("then", lvl + 2);
    openTag("else", lvl + 2);
    node->elseblock()->accept(this, lvl + 4);
    closeTag("else", lvl + 2);
    closeTag(node, lvl);
}
