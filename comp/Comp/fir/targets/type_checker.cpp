#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

#include <cdk/types/primitive_type.h>

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

const int tPRIVATE = 0;
const int tPUBLIC = 1;
const int tMODULE = 2;

//---------------------------------------------------------------------------

void fir::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
//    std::cout << "doin " << node->size() << " nodes in sequence" << std::endl;
    for (size_t i = 0; i < node->size(); i++) {
        node->node(i)->accept(this, lvl);
    }
}

//---------------------------------------------------------------------------

void fir::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
    // EMPTY
}

void fir::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
    // EMPTY
}

void fir::type_checker::do_block_node(fir::block_node *const node, int lvl) {
//    std::cout << "-----> doin block node thingy" << std::endl;
    if (node->declarations()) {
        node->declarations()->accept(this, lvl);
    }
    if (node->instructions()) {
        node->instructions()->accept(this, lvl);
    }
}

void fir::type_checker::do_restart_node(fir::restart_node *const node, int lvl) {
    if (node->lvls() < 0) {
        throw std::string("only positive integers can be used in restart expression");
    }
}

void fir::type_checker::do_leave_node(fir::leave_node *const node, int lvl) {
    if (node->lvls() < 0) {
        throw std::string("only positive integers can be used in leave expression");
    }
}

//---------------------------------------------------------------------------

void fir::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->argument()->accept(this, lvl + 2);
    if (node->argument()->is_typed(cdk::TYPE_INT)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else {
        throw std::string("wrong type in unary logical expression");
    }
}

//---------------------------------------------------------------------------

void fir::type_checker::processBooleanLogic(cdk::binary_operation_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    if (!node->left()->is_typed(cdk::TYPE_INT)) {
        throw std::string("integer expression expected in binary expression");
    }

    node->right()->accept(this, lvl + 2);
    if (!node->right()->is_typed(cdk::TYPE_INT)) {
        throw std::string("integer expression expected in binary expression");
    }

    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}


void fir::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
    processBooleanLogic(node, lvl);
}

void fir::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
    processBooleanLogic(node, lvl);
}

void fir::type_checker::do_alloc_node(fir::alloc_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->argument()->accept(this, lvl + 2);
    if (!node->argument()->is_typed(cdk::TYPE_INT)) {
        throw std::string("expected integer as the amount of slots to allocate");
    }
    //why??? TODO FIXME
    node->type(cdk::reference_type::create(4, cdk::primitive_type::create(8, cdk::TYPE_DOUBLE)));
}

void fir::type_checker::do_body_node(fir::body_node *const node, int lvl) {
//    std::cout << "\tdoin body node thingy" << std::endl;
    if (node->prologue()) {
        node->prologue()->accept(this, lvl);
    }
    if (node->block()) {
        node->block()->accept(this, lvl);
    }
    if (node->epilogue()) {
        node->epilogue()->accept(this, lvl);
    }
}

void fir::type_checker::do_func_call_node(fir::func_call_node *const node, int lvl) {
    ASSERT_UNSPEC;

    const std::string &id = node->identifier();
    auto symbol = _symtab.find(id);
    if (symbol == nullptr) {
        throw std::string("symbol '" + id + "' is undeclared.");
    }
    if (!symbol->isFunction()) {
        throw std::string("symbol '" + id + "' is not a function.");
    }

    node->type(symbol->type());

    if (node->arguments()) {
        if (node->arguments()->size() == symbol->number_of_arguments()) {
            node->arguments()->accept(this, lvl + 4);
            for (size_t ax = 0; ax < node->arguments()->size(); ax++) {
                if (node->argument(ax)->type() == symbol->argument_type(ax))
                    continue;
                if (symbol->argument_is_typed(ax, cdk::TYPE_DOUBLE) && node->argument(ax)->is_typed(cdk::TYPE_INT))
                    continue;
                throw std::string("type mismatch for argument " + std::to_string(ax + 1) + " of '" + id + "'.");
            }
        } else {
            throw std::string(
                    "number of arguments in call (" + std::to_string(node->arguments()->size()) + ") must match declaration ("
                    + std::to_string(symbol->number_of_arguments()) + ").");
        }
    }
}

void fir::type_checker::do_func_decl_node(fir::func_decl_node *const node, int lvl) {
//    std::cout << "type_checker-> doin function declaration thingy" << std::endl;
    std::string id;

    if (node->identifier() == "fir")
        id = "_main";
    else if (node->identifier() == "_main")
        id = "._main";
    else
    id = node->identifier();

    // remember symbol so that args know
    auto function = fir::make_symbol(0, "", false, false, node->access_lvl(), node->type(), id, false, true, true);
    if (node->arguments()) {
        std::vector<std::shared_ptr<cdk::basic_type>> argtypes;
        for (size_t ax = 0; ax < node->arguments()->size(); ax++)
            argtypes.push_back(node->argument(ax)->type());
        function->set_argument_types(argtypes);
    }

    std::shared_ptr<fir::symbol> previous = _symtab.find(function->name());
    if (previous) {
        if (false) {
            throw std::string("conflicting declaration for '" + function->name() + "'");
        }
    } else {
        _symtab.insert(function->name(), function);
        _parent->set_new_symbol(function);
    }
}

void fir::type_checker::do_func_def_node(fir::func_def_node *const node, int lvl) {
//    std::cout << "doing function definition thingy" << std::endl;
    std::string id;
    bool has_ret_val = false;

    if (node->identifier() == "fir")
        id = "_main";
    else if (node->identifier() == "_main")
        id = "._main";
    else
    id = node->identifier();

    _inBlockReturnType = nullptr;

    if(node->ret_value())
        has_ret_val = true;

    // remember symbol so that args know
    auto function = fir::make_symbol(0, "", has_ret_val, false, node->access_lvl(), node->type(), id, false, true);

    if (node->arguments()) {
        std::vector<std::shared_ptr<cdk::basic_type>> argtypes;
        for (size_t ax = 0; ax < node->arguments()->size(); ax++)
            argtypes.push_back(node->argument(ax)->type());
        function->set_argument_types(argtypes);
    }

    std::shared_ptr<fir::symbol> previous = _symtab.find(function->name());
    if (previous) {
        if (previous->forward()
            && ((previous->qualifier() == tPUBLIC && node->access_lvl() == tPUBLIC)
                || (previous->qualifier() == tPRIVATE && node->access_lvl() == tPRIVATE)
                || (previous->qualifier() == tMODULE && node->access_lvl() == tPUBLIC)
                || (previous->qualifier() == tPUBLIC && node->access_lvl() == tMODULE)
                || (previous->qualifier() == tMODULE && node->access_lvl() == tPRIVATE)
                || (previous->qualifier() == tPUBLIC && node->access_lvl() == tPRIVATE)
                || (previous->qualifier() == tMODULE && node->access_lvl() == tMODULE)
                || (previous->qualifier() == tPRIVATE && node->access_lvl() == tPUBLIC)
                || (previous->qualifier() == tPRIVATE && node->access_lvl() == tMODULE))) {
            _symtab.replace(function->name(), function);
            _parent->set_new_symbol(function);
        } else {
            throw std::string("conflicting definition for '" + function->name() + "'");
        }
    } else {
//    std::cout << "function being defined: " << function->name() << std::endl;
        _symtab.insert(function->name(), function);
        _parent->set_new_symbol(function);
    }
}

void fir::type_checker::do_pos_node(fir::pos_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->argument()->accept(this, lvl + 2);
    if (node->argument()->is_typed(cdk::TYPE_INT)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else {
        throw std::string("wrong type in unary logical expression");
    }
}

void fir::type_checker::do_index_node(fir::index_node *const node, int lvl) {
    ASSERT_UNSPEC;
    std::shared_ptr<cdk::reference_type> base_type;

    if (node->base()) {
        node->base()->accept(this, lvl + 2);
        base_type = cdk::reference_type::cast(node->base()->type());
        if (!node->base()->is_typed(cdk::TYPE_POINTER)) throw std::string("trying to index something that's not a pointer expression!");
    } else {
        base_type = cdk::reference_type::cast(_function->type());
        if (!_function->is_typed(cdk::TYPE_POINTER)) throw std::string("return pointer expression expected in index left-value");
    }

    node->index()->accept(this, lvl + 2);
    if (!node->index()->is_typed(cdk::TYPE_INT)) throw std::string("integer expression expected while indexing");

    node->type(base_type->referenced());
}

void fir::type_checker::do_return_node(fir::return_node *const node, int lvl) {
    // EMPTY
}

void fir::type_checker::do_addressof_node(fir::addressof_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->lvalue()->accept(this, lvl + 2);
    node->type(cdk::reference_type::create(4, node->lvalue()->type()));
}

void fir::type_checker::do_sizeof_node(fir::sizeof_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->expression()->accept(this, lvl + 2);
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_var_decl_node(fir::var_decl_node *const node, int lvl) {
    if (node->initializer() != nullptr) {
        node->initializer()->accept(this, lvl + 2);

        if (node->is_typed(cdk::TYPE_INT)) {
            if (!node->initializer()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type for initializer (integer expected).");
        } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
            if (!node->initializer()->is_typed(cdk::TYPE_INT) && !node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
                throw std::string("wrong type as initializer (integer or double expected).");
            }
        } else if (node->is_typed(cdk::TYPE_STRING)) {
            if (!node->initializer()->is_typed(cdk::TYPE_STRING)) {
                throw std::string("wrong type as initializer (string expected).");
            }
        } else if (node->is_typed(cdk::TYPE_POINTER)) {
            //why?? FIXME: trouble!!!
            if (!node->initializer()->is_typed(cdk::TYPE_POINTER)) {
                auto in = (cdk::literal_node<int>*)node->initializer();
                if (in == nullptr || in->value() != 0) throw std::string("wrong type for initializer (pointer expected).");
            }
        } else {
            throw std::string("unknown type as initializer!");
        }
    }

    const std::string &id = node->identifier();
    auto symbol = fir::make_symbol(0, "", false, false, node->access_lvl(), node->type(), id, (bool)node->initializer(), false);
//    std::cout << "before var decl" << std::endl;
    if (_symtab.insert(id, symbol)) {
        _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
    } else {
        throw std::string("variable '" + id + "' redeclared!");
    }
//    std::cout << "after var decl" << std::endl;
}

//---------------------------------------------------------------------------

void fir::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}

void fir::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

void fir::type_checker::do_nullptr_node(fir::nullptr_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(cdk::reference_type::create(4, nullptr));
}

//---------------------------------------------------------------------------

void fir::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
    node->argument()->accept(this, lvl + 2);
    if (node->argument()->type() == nullptr) throw std::string("No type assigned to argument of unary expression");

    node->type(node->argument()->type());
}

void fir::type_checker::do_neg_node(cdk::neg_node *const node, int lvl) {
    processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void fir::type_checker::processBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
    ASSERT_UNSPEC;

//    std::cout << "yellow: " << (((fir::func_call_node*)node->left())->type() == nullptr) << std::endl;
//    std::cout << "red: " << (node->right()->type() == nullptr) << std::endl;

    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);

    if (node->left()->type() == node->right()->type()) {
        /* Both sides of comparison match types! */
    } else if ((node->left()->type()->name() == cdk::TYPE_DOUBLE)
        && (node->right()->type()->name() == cdk::TYPE_INT)) {

    } else {
        throw std::string("argument types in comparison don't match");
    }

    node->type(node->left()->type());
}

void fir::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
    do_pid_expression(node, lvl);
}

void fir::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
    do_pid_expression(node, lvl);
}

void fir::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
    do_id_expression(node, lvl);
}

void fir::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
    do_id_expression(node, lvl);
}

void fir::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    if (!node->left()->is_typed(cdk::TYPE_INT)) {
        throw std::string("expected integer expression in binary operator (left)");
    }

    node->right()->accept(this, lvl + 2);
    if (!node->right()->is_typed(cdk::TYPE_INT)) {
        throw std::string("expected integer expression in binary operator (right)");
    }

    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
    do_scal_expression(node, lvl);
}

void fir::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
    do_scal_expression(node, lvl);
}

void fir::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
    do_scal_expression(node, lvl);
}

void fir::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
    do_scal_expression(node, lvl);
}

void fir::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
    do_gen_expression(node, lvl);
}

void fir::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
    do_gen_expression(node, lvl);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
    ASSERT_UNSPEC;
    const std::string &id = node->name();
    auto symbol = _symtab.find(id);

    if (symbol != nullptr) {
        node->type(symbol->type());
    } else {
        symbol = _symtab.find("_ret" + id);

        if (symbol == nullptr) {
            throw std::string("undeclared variable: '" + id + "'");
        } else {
            node->type(symbol->type());
        }
    }
}

void fir::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
    ASSERT_UNSPEC;
    try {
        node->lvalue()->accept(this, lvl);
        node->type(node->lvalue()->type());
    } catch (const std::string &id) {
        throw "undeclared variable '" + id + "'";
    }
}

void fir::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
    ASSERT_UNSPEC;

    node->lvalue()->accept(this, lvl + 4);
    node->rvalue()->accept(this, lvl + 4);

    if (node->lvalue()->is_typed(cdk::TYPE_INT)) {
        if (node->rvalue()->is_typed(cdk::TYPE_INT)){
            node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        } else if (node->rvalue()->is_typed(cdk::TYPE_INT)){
            node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
            node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        } else {
            throw std::string("wrong assignment to integer");
        }
    } else if (node->lvalue()->is_typed(cdk::TYPE_POINTER)) {
        //TODO: check pointer level

        if (node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
            node->type(node->rvalue()->type());
        } else if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
            // int* arr = 0;
            //TODO: check that the integer is a literal and that it is zero
            node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
        } else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
            node->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
            node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
        } else {
            throw std::string("wrong assignment to pointer");
        }
    } else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {

        if (node->rvalue()->is_typed(cdk::TYPE_DOUBLE) || node->rvalue()->is_typed(cdk::TYPE_INT)){
            node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
        } else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)){
            node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
            node->rvalue()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
        } else {
            throw std::string("wrong assignment to double");
        }

    } else if (node->lvalue()->is_typed(cdk::TYPE_STRING)) {
        if (node->rvalue()->is_typed(cdk::TYPE_STRING)) {
            node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
        } else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)){
            node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
            node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
        } else {
            throw std::string("wrong assignment to string");
        }
    } else {
        throw std::string("wrong types in assignment");
    }

}

//---------------------------------------------------------------------------

void fir::type_checker::do_evaluation_node(fir::evaluation_node *const node, int lvl) {
    node->argument()->accept(this, lvl + 2);
}

void fir::type_checker::do_print_node(fir::print_node *const node, int lvl) {
    node->exprs()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_read_node(fir::read_node *const node, int lvl) {
    node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//---------------------------------------------------------------------------

void fir::type_checker::do_while_node(fir::while_node *const node, int lvl) {
    if (node->block()) {
        node->block()->accept(this, lvl);
    }
    if (node->finally_block()) {
        node->finally_block()->accept(this, lvl);
    }
}

//---------------------------------------------------------------------------

void fir::type_checker::do_pid_expression(cdk::binary_operation_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);

    if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
        node->type(node->left()->type());
    } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)) {
        node->type(node->right()->type());
    } else {
        throw std::string("wrong types in binary expression");
    }
}

void fir::type_checker::do_id_expression(cdk::binary_operation_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);

    if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else {
        throw std::string("wrong types in binary expression");
    }
}

void fir::type_checker::do_scal_expression(cdk::binary_operation_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    if (!node->left()->is_typed(cdk::TYPE_INT)) {
        throw std::string("integer expression expected in binary logical expression (left)");
    }

    node->right()->accept(this, lvl + 2);
    if (!node->right()->is_typed(cdk::TYPE_INT)) {
        throw std::string("integer expression expected in binary logical expression (right)");
    }

    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_gen_expression(cdk::binary_operation_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);
    if (node->left()->type() != node->right()->type()) {
        throw std::string("both sides have to be same type in equality operation");
    }
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void fir::type_checker::do_if_node(fir::if_node *const node, int lvl) {
    node->condition()->accept(this, lvl + 4);
    if(!node->condition()->is_typed(cdk::TYPE_INT)) throw std::string("expected integer condition");
}

void fir::type_checker::do_if_else_node(fir::if_else_node *const node, int lvl) {
    node->condition()->accept(this, lvl + 4);
    if(!node->condition()->is_typed(cdk::TYPE_INT)) throw std::string("expected integer condition");
}
