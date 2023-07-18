#include <string>
#include <sstream>
#include <memory>
#include <cdk/types/types.h>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/frame_size_calculator.h"
#include "targets/symbol.h"
#include "ast/all.h"  // all.h is automatically generated

const int tPRIVATE = 0;
const int tPUBLIC = 1;
const int tMODULE = 2;

//---------------------------------------------------------------------------
void fir::postfix_writer::do_data_node(cdk::data_node *const node, int lvl) {
    // EMPTY
}

void fir::postfix_writer::do_nil_node(cdk::nil_node *const node, int lvl) {
    // EMPTY
}

void fir::postfix_writer::do_sequence_node(cdk::sequence_node *const node, int lvl) {
    for (size_t i = 0; i < node->size(); i++) {
        node->node(i)->accept(this, lvl);
    }
}

void fir::postfix_writer::do_block_node(fir::block_node *const node, int lvl) {

//    std::cout << "postfix_writer-> doin block thingy" << std::endl;

    if (node->declarations())
        node->declarations()->accept(this, lvl + 2);
    if (node->instructions())
        node->instructions()->accept(this, lvl + 2);
}

void fir::postfix_writer::do_return_node(fir::return_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin return thingy" << std::endl;


    _pf.JMP(_currentBodyRetLabel);
    _returnSeen = true;
}

void fir::postfix_writer::do_var_decl_node(fir::var_decl_node *const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
//    std::cout << "postfix_writer-> doin variable decl thingy" << std::endl;

    auto id = node->identifier();

//    std::cout << "INITIAL OFFSET: " << _offset << std::endl;

    int offset = 0, typesize = node->type()->size();
//    std::cout << "ARG: " << id << ", " << typesize << std::endl;

    if (_inFunctionBody) {
//        std::cout << "IN BODY" << std::endl;
        _offset -= typesize;
        offset = _offset;
    } else if (_inFunctionArgs) {
//        std::cout << "IN ARGS" << std::endl;
        offset = _offset;
        _offset += typesize;
    } else {
//        std::cout << "GLOBAL! " << std::endl;
        offset = 0;
    }

//    std::cout << "OFFSET: " << id << ", " << offset << std::endl;

    auto symbol = new_symbol();
    if (symbol) {
        symbol->set_offset(offset);
        reset_new_symbol();
    }

    if (_inFunctionBody) {
        if (node->initializer()) {
            node->initializer()->accept(this, lvl);
            if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_STRING) || node->is_typed(cdk::TYPE_POINTER)) {
//                std::cout << "\tsymbol: " << symbol->offset() << std::endl;
                _pf.LOCAL(symbol->offset());
                _pf.STINT();
            } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
                if (node->initializer()->is_typed(cdk::TYPE_INT))
                    _pf.I2D();
                _pf.LOCAL(symbol->offset());
                _pf.STDOUBLE();
            } else {
                std::cerr << "cannot initialize" << std::endl;
            }
        }
    } else {
        if (!_function) {
            if (node->initializer() == nullptr) {
                _pf.BSS();
                _pf.ALIGN();
                _pf.LABEL(id);
                _pf.SALLOC(typesize);
            } else {
                if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_DOUBLE) || node->is_typed(cdk::TYPE_POINTER)) {
                    _pf.DATA();
                    _pf.ALIGN();
                    _pf.LABEL(id);

                    if (node->is_typed(cdk::TYPE_INT)) {
                        node->initializer()->accept(this, lvl);
                    } else if (node->is_typed(cdk::TYPE_POINTER)) {
                        node->initializer()->accept(this, lvl);
                    } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
                        if (node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
                            node->initializer()->accept(this, lvl);
                        } else if (node->initializer()->is_typed(cdk::TYPE_INT)) {
                            cdk::integer_node *dclini = dynamic_cast<cdk::integer_node*>(node->initializer());
                            cdk::double_node ddi(dclini->lineno(), dclini->value());
                            ddi.accept(this, lvl);
                        } else {
                            std::cerr << node->lineno() << ": '" << id << "' has bad initializer for real value\n";
                            _errors = true;
                        }
                    }
                } else if (node->is_typed(cdk::TYPE_STRING)) {
                    _pf.DATA();
                    _pf.ALIGN();
                    _pf.LABEL(id);
                    node->initializer()->accept(this, lvl);
                } else {
                    std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer\n";
                    _errors = true;
                }

            }
        }
    }
}

//===========================================================================

void fir::postfix_writer::do_double_node(cdk::double_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin double thingy" << std::endl;

    if(_inFunctionBody) {
        _pf.DOUBLE(node->value());
    } else {
        _pf.SDOUBLE(node->value());
    }
}

void fir::postfix_writer::do_integer_node(cdk::integer_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin int thingy" << std::endl;

    if (_inFunctionBody) {
        _pf.INT(node->value()); // integer literal is on the stack: push an integer
    } else {
        _pf.SINT(node->value()); // integer literal is on the DATA segment
    }
}

void fir::postfix_writer::do_string_node(cdk::string_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin string thingy" << std::endl;

    int lbl1;

    /* generate the string */
    _pf.RODATA(); // strings are DATA readonly
    _pf.ALIGN(); // make sure we are aligned
    _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
    _pf.SSTRING(node->value()); // output string characters

    /* leave the address on the stack */

    if (_function) {
        // local variable initializer
        _pf.TEXT(); // return to the TEXT segment
        _pf.ADDR(mklbl(lbl1)); // the string to be printed

    } else {
        // global variable initializer
        _pf.DATA();
        _pf.SADDR(mklbl(lbl1)); // the string to be printed
    }
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_variable_node(cdk::variable_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin variable_node thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    const std::string &id = node->name();
    auto symbol = _symtab.find(id);

    if (symbol->isFunction()) {
        symbol = _symtab.find("_ret" + id);
    }

    if(symbol->global()){
        _pf.ADDR(node->name());
    } else {
        _pf.LOCAL(symbol->offset());
    }
}

void fir::postfix_writer::do_index_node(fir::index_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin index thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    if(node->base()) {
        node->base()->accept(this, lvl);
    } else {
        if (_function) {
            _pf.LOCV(-_function->type()->size());
        } else {
            std::cerr << "FATAL: " << node->lineno() << ": trying to use return value outside function" << std::endl;
        }
    }

    node->index()->accept(this, lvl);
    _pf.INT(3);
    _pf.SHTL();
    _pf.ADD(); // add pointer and index
}

void fir::postfix_writer::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin rvalue thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    node->lvalue()->accept(this, lvl);

    if (node->type()->name() == cdk::TYPE_DOUBLE) {
        _pf.LDDOUBLE();
    } else {
        _pf.LDINT(); // depends on type size
    }
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_assignment_node(cdk::assignment_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin assignment thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    node->rvalue()->accept(this, lvl + 2); // determine the new value

    if (node->type()->name() == cdk::TYPE_DOUBLE) {
        if (node->rvalue()->type()->name() == cdk::TYPE_INT)
            _pf.I2D();
        _pf.DUP64();
    } else {
        _pf.DUP32();
    }

    node->lvalue()->accept(this, lvl);
    if (node->type()->name() == cdk::TYPE_DOUBLE) {
        _pf.STDOUBLE();
    } else {
        _pf.STINT();
    }

    /*if (new_symbol() == nullptr) {
        node->lvalue()->accept(this, lvl); // where to store the value
    } else {
        _pf.DATA(); // variables are all global and live in DATA
        _pf.ALIGN(); // make sure we are aligned
        _pf.LABEL(new_symbol()->name()); // name variable location
        reset_new_symbol();
        _pf.SINT(0); // initialize it to 0 (zero)
        _pf.TEXT(); // return to the TEXT segment
        node->lvalue()->accept(this, lvl);  //DAVID: bah!
    }
    _pf.STINT(); // store the value at address*/
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_read_node(fir::read_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin read thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    if (_lvalueType == cdk::TYPE_DOUBLE) {
        _functions_to_declare.insert("readd");
        _pf.CALL("readd");
        _pf.LDFVAL64();
    } else if (_lvalueType == cdk::TYPE_INT) {
        _functions_to_declare.insert("readi");
        _pf.CALL("readi");
        _pf.LDFVAL32();
    } else {
        std::cerr << "FATAL: " << node->lineno() << ": cannot read type" << std::endl;
        return;
    }
}


void fir::postfix_writer::do_print_node(fir::print_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin print thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    for (size_t ix = 0; ix < node->exprs()->size(); ix++) {
        auto child = dynamic_cast<cdk::expression_node*>(node->exprs()->node(ix));

        std::shared_ptr<cdk::basic_type> etype = child->type();
        child->accept(this, lvl); // expression to print
        if (etype->name() == cdk::TYPE_INT) {
            _functions_to_declare.insert("printi");
            _pf.CALL("printi");
            _pf.TRASH(4); // trash int
        } else if (etype->name() == cdk::TYPE_DOUBLE) {
            _functions_to_declare.insert("printd");
            _pf.CALL("printd");
            _pf.TRASH(8); // trash double
        } else if (etype->name() == cdk::TYPE_STRING) {
            _functions_to_declare.insert("prints");
            _pf.CALL("prints");
            _pf.TRASH(4); // trash char pointer
        } else {
            std::cerr << "cannot print expression of unknown type" << std::endl;
            return;
        }

    }

    if (node->newline()) {
        _functions_to_declare.insert("println");
        _pf.CALL("println");
    }
}

void fir::postfix_writer::do_evaluation_node(fir::evaluation_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin evaluation thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    node->argument()->accept(this, lvl); // determine the value
    _pf.TRASH(node->argument()->type()->size());
}
//---------------------------------------------------------------------------

void fir::postfix_writer::do_neg_node(cdk::neg_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin neg thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    node->argument()->accept(this, lvl); // determine the value
    _pf.NEG(); // 2-complement
}

void fir::postfix_writer::do_not_node(cdk::not_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin not thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    node->argument()->accept(this, lvl + 2);
    _pf.INT(0);
    _pf.EQ();

}

//---------------------------------------------------------------------------


void fir::postfix_writer::do_add_node(cdk::add_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin add thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    node->left()->accept(this, lvl + 2);

    if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) {
        _pf.I2D();
    } else if (node->type()->name() == cdk::TYPE_POINTER && node->left()->type()->name() == cdk::TYPE_INT ) {
        _pf.INT(3);
        _pf.SHTL();
    }

    node->right()->accept(this, lvl + 2);

    if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) {
        _pf.I2D();
    } else if (node->type()->name() == cdk::TYPE_POINTER && node->right()->type()->name() == cdk::TYPE_INT) {
        _pf.INT(3);
        _pf.SHTL();
    }

    if (node->type()->name() == cdk::TYPE_DOUBLE)
        _pf.DADD();
    else
        _pf.ADD();
}

void fir::postfix_writer::do_sub_node(cdk::sub_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin sub thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    node->left()->accept(this, lvl + 2);
    if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT)
        _pf.I2D();

    node->right()->accept(this, lvl + 2);
    if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) {
        _pf.I2D();
    } else if (node->type()->name() == cdk::TYPE_POINTER && node->right()->type()->name() == cdk::TYPE_INT) {
        _pf.INT(3);
        _pf.SHTL();
    }

    if (node->type()->name() == cdk::TYPE_DOUBLE)
        _pf.DSUB();
    else
        _pf.SUB();
}

void fir::postfix_writer::do_mul_node(cdk::mul_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin mul thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    node->left()->accept(this, lvl + 2);
    if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT)
        _pf.I2D();

    node->right()->accept(this, lvl + 2);
    if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT)
        _pf.I2D();

    if (node->type()->name() == cdk::TYPE_DOUBLE)
        _pf.DMUL();
    else
        _pf.MUL();
}

void fir::postfix_writer::do_div_node(cdk::div_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin div thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    node->left()->accept(this, lvl + 2);
    if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT)
        _pf.I2D();

    node->right()->accept(this, lvl + 2);
    if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT)
        _pf.I2D();

    if (node->type()->name() == cdk::TYPE_DOUBLE)
        _pf.DDIV();
    else
        _pf.DIV();
}

void fir::postfix_writer::do_mod_node(cdk::mod_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin mod thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);
    _pf.MOD();
}

//---------------------------------------------------------------------------


void fir::postfix_writer::do_sizeof_node(fir::sizeof_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin sizeof thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    _pf.INT(node->expression()->type()->size());
}

//---------------------------------------------------------------------------


void fir::postfix_writer::do_ge_node(cdk::ge_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin greater equal thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    node->left()->accept(this, lvl + 2);
    if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();

    node->right()->accept(this, lvl + 2);
    if (node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();

    if(node->left()->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.DCMP();
    else
        _pf.GE();
}


void fir::postfix_writer::do_gt_node(cdk::gt_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin greater than thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    node->left()->accept(this, lvl + 2);
    if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();

    node->right()->accept(this, lvl + 2);
    if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();

    _pf.GT(); //TODO DCMP?
}

void fir::postfix_writer::do_le_node(cdk::le_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin less or equal thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    node->left()->accept(this, lvl + 2);
    if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();

    node->right()->accept(this, lvl + 2);
    if (node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();

    _pf.LE();  //TODO DCMP?
}

void fir::postfix_writer::do_lt_node(cdk::lt_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin less than thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    node->left()->accept(this, lvl + 2);
    if(node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();

    node->right()->accept(this, lvl + 2);
    if(node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();

    _pf.LT(); //TODO DCMP?
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_and_node(cdk::and_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin and thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    int lbl = ++_lbl;
    node->left()->accept(this, lvl + 2);
    _pf.DUP32();
    _pf.JZ(mklbl(lbl));
    node->right()->accept(this, lvl + 2);
    _pf.AND();
    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl));
}

void fir::postfix_writer::do_or_node(cdk::or_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin or thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    int lbl = ++_lbl;
    node->left()->accept(this, lvl + 2);
    _pf.DUP32();
    _pf.JZ(mklbl(lbl));
    node->right()->accept(this, lvl + 2);
    _pf.OR();
    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl));
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_eq_node(cdk::eq_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin equal thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    node->left()->accept(this, lvl);
    if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();
    node->right()->accept(this, lvl);
    if (node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();

    _pf.EQ();
}

void fir::postfix_writer::do_ne_node(cdk::ne_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin not equal thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    node->left()->accept(this, lvl);
    if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();
    node->right()->accept(this, lvl);
    if (node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();

    _pf.NE();
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_addressof_node(fir::addressof_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin address of thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    node->lvalue()->accept(this, lvl + 2);
}

void fir::postfix_writer::do_alloc_node(fir::alloc_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin alloc thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    node->argument()->accept(this, lvl);
    _pf.INT(3);
    _pf.SHTL();
    _pf.ALLOC(); // allocate
    _pf.SP(); // put base pointer in stack
}

void fir::postfix_writer::do_nullptr_node(fir::nullptr_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin nullptr thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    if (_inFunctionBody) {
        _pf.INT(0);
    } else {
        _pf.SINT(0);
    }
}

//---------------------------------------------------------------------------
//TODO KIKA -> VOLTAR AQUI
void fir::postfix_writer::do_while_node(fir::while_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin while thingy" << std::endl;

    _whileCond.push(++_lbl);
    _whileEnd.push(++_lbl);

    os() << "        ;; WHILE condition " << std::endl;

    _symtab.push();
    _inWhileCond = true;  // remember this for local declarations

    // prepare to test
    _pf.ALIGN();
    _pf.LABEL(mklbl(_whileCond.top()));
    node->condition()->accept(this, lvl + 2);
    _pf.JZ(mklbl(_whileEnd.top()));

    os() << "        ;; WHILE block" << std::endl;
    // execute instruction
    node->block()->accept(this, lvl + 2);

    os() << "        ;; WHILE jump to condition" << std::endl;
    _pf.JMP(mklbl(_whileCond.top()));

    os() << "        ;; WHILE finally block" << std::endl;
    if (node->finally_block()) {
        _pf.ALIGN();
        _pf.LABEL(mklbl(_whileEnd.top()));
        _inWhileFinally = true;
        node->finally_block()->accept(this, lvl + 2);
        _inWhileFinally = false;
    } else {
        os() << "        ;; WHILE end" << std::endl;
        _pf.ALIGN();
        _pf.LABEL(mklbl(_whileEnd.top()));
    }


    _inWhileCond = false;  // remember this for local declarations
    _symtab.pop();

    _whileCond.pop();
    _whileEnd.pop();
}

void fir::postfix_writer::do_restart_node(fir::restart_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin restart thingy" << std::endl;
    //TODO ver se esta no finally
    if(_inWhileFinally)
        std::cerr << node->lineno() << ": " << "can't 'restart' inside of a 'finally' block" << std::endl;
    if (_whileCond.size() < (size_t)node->lvls()) {
        std::cerr << node->lineno() << ": " << "trying to 'restart' an outside 'while(" << node->lvls() << ")' that doesn't exist" << std::endl;
    }

    if (_whileCond.size() != 0) {
        _pf.JMP(mklbl(_whileCond.c[_whileCond.size() - node->lvls()])); // jump to next cycle
    } else
        std::cerr << node->lineno() << ": " << "'restart' outside 'for'" << std::endl;
}

void fir::postfix_writer::do_leave_node(fir::leave_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin leave thingy" << std::endl;
//TODO ver se esta no finally
    if(_inWhileFinally)
        std::cerr << node->lineno() << ": " << "can't 'leave' inside of a 'finally' block" << std::endl;

    if (_whileCond.size() < (size_t)node->lvls()) {
        std::cerr << node->lineno() << ": " << "trying to 'leave' an outside 'while(" <<  node->lvls() << ")' that doesn't exist" << std::endl;
    }

    if (_whileCond.size() != 0) {
        _pf.JMP(mklbl(_whileEnd.c[_whileCond.size() - node->lvls()])); // jump to for end
    } else
        std::cerr << "FATAL: " << node->lineno() << "'leave' outside 'for'" << std::endl;
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_if_node(fir::if_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin if thingy" << std::endl;
    int lbl1;
    node->condition()->accept(this, lvl);
    _pf.JZ(mklbl(lbl1 = ++_lbl));
    node->block()->accept(this, lvl + 2);
    _pf.LABEL(mklbl(lbl1));
}

void fir::postfix_writer::do_if_else_node(fir::if_else_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin if else thingy" << std::endl;
    ASSERT_SAFE_EXPRESSIONS;
    int lbl1, lbl2;
    node->condition()->accept(this, lvl);
    _pf.JZ(mklbl(lbl1 = ++_lbl));
    node->thenblock()->accept(this, lvl + 2);
    if (node->elseblock()) {
        _pf.JMP(mklbl(lbl2 = ++_lbl));
        _pf.LABEL(mklbl(lbl1));
        node->elseblock()->accept(this, lvl + 2);
    }
    _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_func_call_node(fir::func_call_node *const node, int lvl) {

//    std::cout << "postfix_writer-> doin function call thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

    auto symbol = _symtab.find(node->identifier());

    size_t argsSize = 0;

    if (node->arguments()){
        if (node->arguments()->size() > 0) {
            for (int ax = node->arguments()->size() - 1; ax >= 0; ax--) {
                cdk::expression_node *arg = dynamic_cast<cdk::expression_node *>(node->arguments()->node(ax));
                arg->accept(this, lvl + 2);
                if (symbol->argument_is_typed(ax, cdk::TYPE_DOUBLE) && arg->is_typed(cdk::TYPE_INT)) {
                    _pf.I2D();
                }
                argsSize += symbol->argument_size(ax);
            }
        }
    }
    _pf.CALL(node->identifier());

    if(argsSize != 0) {
        _pf.TRASH(argsSize);
    }

    if (symbol->is_typed(cdk::TYPE_INT) || symbol->is_typed(cdk::TYPE_POINTER) || symbol->is_typed(cdk::TYPE_STRING)) {
        _pf.LDFVAL32();
    } else if (symbol->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.LDFVAL64();
    } else if (symbol->is_typed(cdk::TYPE_VOID)) {
        // It's a void function
    } else {
        std::cerr << "FATAL: " << node->lineno() << "Type doesn't exist!" << std::endl;
    }
}

void fir::postfix_writer::do_func_decl_node(fir::func_decl_node *const node, int lvl) {

//    std::cout << "postfix_writer-> doin function declaration thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

//    std::cout << "\tfunc_decl: in body: " << _inFunctionBody << std::endl;
//    std::cout << "\tfunc_decl: in args: " << _inFunctionArgs << std::endl;


    if (_inFunctionBody || _inFunctionArgs) {
        std::cerr << "FATAL: " << node->lineno() << "can't declare function in body or in args" << std::endl;
        return;
    }

    if (!new_symbol())
        return;

    auto function = new_symbol();
    _functions_to_declare.insert(function->name());
    reset_new_symbol();
}

void fir::postfix_writer::do_func_def_node(fir::func_def_node *const node, int lvl) {

//    std::cout << "postfix_writer-> doin function definition thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;

//    std::cout << "\tfunc_def: in body: " << _inFunctionBody << std::endl;
//    std::cout << "\tfunc_def: in args: " << _inFunctionArgs << std::endl;


    if (_inFunctionBody || _inFunctionArgs) {
        std::cerr << "FATAL: " << node->lineno() << "Defining a function in a function body or arguments!" << std::endl;
    }

    _function = new_symbol();
    _functions_to_declare.erase(_function->name());
    reset_new_symbol();

    _currentBodyRetLabel = mklbl(++_lbl);

    _offset = 8;

    if (node->arguments()){
        if (node->arguments()->size() > 0) {
            _inFunctionArgs = true;
            for (size_t i = 0; i < node->arguments()->size(); i++) {
                cdk::basic_node *arg = node->arguments()->node(i);
                if (arg) {
                    arg->accept(this, 0);
                } else {
                    break;
                }
            }
            _inFunctionArgs = false;
        }
    }


    _pf.TEXT();
    _pf.ALIGN();
    if (node->access_lvl() == tPUBLIC) _pf.GLOBAL(_function->name(), _pf.FUNC());
    _pf.LABEL(_function->name());

    // compute stack size to be reserved for local variables
    frame_size_calculator lsc(_compiler, _symtab, _function);
    node->accept(&lsc, lvl);

    // check function type (4 or 8 bytes)
    if(node->type()->name() == cdk::TYPE_INT || node->type()->name() == cdk::TYPE_POINTER || node->type()->name() == cdk::TYPE_STRING)
        _pf.ENTER(lsc.localsize() + 4); // total stack size reserved for local variables
    else if (node->type()->name() == cdk::TYPE_DOUBLE)
        _pf.ENTER(lsc.localsize() + 8);
    else if (node->type()->name() == cdk::TYPE_VOID)
        _pf.ENTER(lsc.localsize() + 0);
    else
        std::cerr << "FATAL: " << node->lineno() << "Function type doesn't exist" << std::endl;

//    std::cout << "\t| function name: " << node->identifier() << std::endl;
    std::string name = std::string("_ret" + node->identifier());
//    std::cout << "\t| function variable name: " << name << std::endl;
//    do_var_decl_node( new fir::var_decl_node(node->lineno(), 0, node->type(), name, node->ret_value()), lvl);


    _inFunctionBody = true;
    _offset = -node->type()->size(); // prepare for local variable

//    std::cout << "postfix_writer-> about to accept ret_value thingy. function void?: " << node->is_typed(cdk::TYPE_VOID) << std::endl;
    if(node->ret_value() && !(node->is_typed(cdk::TYPE_VOID))) {
//        std::cout << "postfix_writer-> gonna accept ret_value thingy" << std::endl;
        node->ret_value()->accept(this, lvl);
    } else if (!node->ret_value() && !(node->is_typed(cdk::TYPE_VOID))) {
        _pf.INT(0);
    }
    if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_STRING) || node->is_typed(cdk::TYPE_POINTER)) {
        _pf.LOCAL(_offset);
        _pf.STINT();
    } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
        if (node->ret_value()) {
            if (node->ret_value()->is_typed(cdk::TYPE_INT))
                _pf.I2D();
        } else if (!node->ret_value() && _function->is_typed(cdk::TYPE_DOUBLE)) {
            _pf.I2D();
        }
        _pf.LOCAL(_offset);
        _pf.STDOUBLE();
    }

    _symtab.push();
    // the following flag is a slight hack: it won't work with nested functions
    _inFunctionBody = true;
    os() << "        ;; before body " << std::endl;
    node->body()->accept(this, lvl + 4); // block has its own scope
    os() << "        ;; after body " << std::endl;
    _inFunctionBody = false;
    _returnSeen = false;

    _pf.LABEL(_function->name() + mklbl(++_lbl));

    if (node->type()->name() != cdk::TYPE_VOID) {
        _pf.LOCAL(-node->type()->size());

        if (node->type()->name() == cdk::TYPE_INT || node->type()->name() == cdk::TYPE_STRING
            || node->type()->name() == cdk::TYPE_POINTER) {
            _pf.LDINT();
            _pf.STFVAL32();
        } else if (node->type()->name() == cdk::TYPE_DOUBLE) {
            _pf.LDDOUBLE();
            _pf.STFVAL64();
        } else {
            std::cerr << node->lineno() << ": should not happen: unknown return type" << std::endl;
        }
    }

    _pf.LEAVE();
    _pf.RET();

    _symtab.pop(); // scope of arguments

    if (node->identifier() == "fir") {
        // declare external functions
        for (std::string s : _functions_to_declare)
            _pf.EXTERN(s);
    }
}

void fir::postfix_writer::do_body_node(fir::body_node *const node, int lvl) {

//    std::cout << "postfix_writer-> doin body thingy" << std::endl;

    _symtab.push();
    if (node->prologue()) {
        node->prologue()->accept(this, lvl);
    }
    if (node->block()) {
        node->block()->accept(this, lvl);
    }
    if (node->epilogue()) {
        node->epilogue()->accept(this, lvl);
    }
    _symtab.pop();
}

void fir::postfix_writer::do_pos_node(fir::pos_node *const node, int lvl) {
//    std::cout << "postfix_writer-> doin pos thingy" << std::endl;

    ASSERT_SAFE_EXPRESSIONS;
    node->argument()->accept(this, lvl);
}

//---------------------------------------------------------------------------
