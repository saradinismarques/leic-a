#ifndef COMPILADORES_RESTART_NODE_H
#define COMPILADORES_RESTART_NODE_H

namespace fir {
    class restart_node : public cdk::basic_node {
        int _lvls;

    public:
        restart_node(int lineno, int lvls = 1) : cdk::basic_node(lineno), _lvls(lvls) {}

        int lvls() {
            return _lvls;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_restart_node(this, level);
        }
    };
} // fir

#endif
