#include "gcc-plugin.h"
#include "plugin-version.h"
#include "tree.h"
#include "coretypes.h"
#include "c-family/c-common.h"
#include "libiberty.h"
#include "print-tree.h"

int plugin_is_GPL_compatible;

#define PRINT_FLAG(name, flag) \
printf("  %-33s | \x1b[%dm%d\x1b[0m\n", name, (flag) ? 32 : 31, (flag))

static void print_optimization_flags(tree fndecl) {
    struct cl_optimization *opts = opts_for_fn(fndecl);

    if (!opts) {
        printf("\x1b[1;31mNo optimization info available.\x1b[0m\n");
        return;
    }

    printf("\n\x1b[1;34m=== Optimization Flags ===\x1b[0m\n\n");
    printf("%-35s | %s\n", "Optimization Flag", "Enabled");
    printf("------------------------------------|--------\n");

    PRINT_FLAG("-funroll-loops", opts->x_flag_unroll_loops);
    PRINT_FLAG("-fpeel-loops", opts->x_flag_peel_loops);
    PRINT_FLAG("-funswitch-loops", opts->x_flag_unswitch_loops);
    PRINT_FLAG("-ftree-loop-distribute-patterns", opts->x_flag_tree_loop_distribute_patterns);
    PRINT_FLAG("-fselective-scheduling", opts->x_flag_selective_scheduling);
    PRINT_FLAG("-fselective-scheduling2", opts->x_flag_selective_scheduling2);
    PRINT_FLAG("-fschedule-insns", opts->x_flag_schedule_insns);
    PRINT_FLAG("-fschedule-insns2", opts->x_flag_schedule_insns_after_reload);
    PRINT_FLAG("-fsched-interblock", opts->x_flag_schedule_interblock);
    PRINT_FLAG("-ftree-vectorize", opts->x_flag_tree_vectorize);
    PRINT_FLAG("-ftree-tail-merge", opts->x_flag_tree_tail_merge);
    PRINT_FLAG("-fbranch-probabilities", opts->x_flag_branch_probabilities);
    PRINT_FLAG("-fcode-hoisting", opts->x_flag_code_hoisting);

    printf("\n");
}

static void print_optimization_tree(tree fndecl) {
    if (DECL_FUNCTION_SPECIFIC_OPTIMIZATION(fndecl)) {
        debug_tree(DECL_FUNCTION_SPECIFIC_OPTIMIZATION(fndecl));
    } else {
        printf("Doesn't have function specific optimizations!\n");
    }
}

static void print_global_optimization_tree() {
    tree fn_opts = optimization_default_node;
    if (fn_opts) {
        debug_tree(fn_opts);
    } else {
        printf("Doesn't have global optimizations!\n");
    }
}

static signed char read_flag(const char* name, signed char current_value) {
    signed char new_value = -1;

    printf("Set %-33s [currently %d]: ", name, current_value);
    int res = scanf("%hhd", &new_value);
    if (res == 1 && (new_value == 0 || new_value == 1))
        return new_value;

    printf("  \x1b[33mInvalid input; keeping current value: %d\x1b[0m\n", current_value);
    return current_value;
}

static void set_optimization_flags(tree fndecl) {
    if (DECL_FUNCTION_SPECIFIC_OPTIMIZATION (fndecl) == NULL_TREE)
        DECL_FUNCTION_SPECIFIC_OPTIMIZATION (fndecl) = copy_node(optimization_default_node);
    struct cl_optimization *opts = TREE_OPTIMIZATION(DECL_FUNCTION_SPECIFIC_OPTIMIZATION (fndecl));

    if (!opts) {
        printf("\x1b[1;31mNo optimization info available.\x1b[0m\n");
        return;
    }

    printf("\n\x1b[1;36m=== Set Function-Specific Optimization Flags ===\x1b[0m\n");

    opts->x_flag_unroll_loops = read_flag("-funroll-loops", opts->x_flag_unroll_loops);
    opts->x_flag_peel_loops = read_flag("-fpeel-loops", opts->x_flag_peel_loops);
    opts->x_flag_unswitch_loops = read_flag("-funswitch-loops", opts->x_flag_unswitch_loops);
    opts->x_flag_tree_loop_distribute_patterns = read_flag("-ftree-loop-distribute-patterns", opts->x_flag_tree_loop_distribute_patterns);
    opts->x_flag_selective_scheduling = read_flag("-fselective-scheduling", opts->x_flag_selective_scheduling);
    opts->x_flag_selective_scheduling2 = read_flag("-fselective-scheduling2", opts->x_flag_selective_scheduling2);
    opts->x_flag_schedule_insns = read_flag("-fschedule-insns", opts->x_flag_schedule_insns);
    opts->x_flag_schedule_insns_after_reload = read_flag("-fschedule-insns2", opts->x_flag_schedule_insns_after_reload);
    opts->x_flag_schedule_interblock = read_flag("-fsched-interblock", opts->x_flag_schedule_interblock);
    opts->x_flag_tree_vectorize = read_flag("-ftree-vectorize", opts->x_flag_tree_vectorize);
    opts->x_flag_tree_tail_merge = read_flag("-ftree-tail-merge", opts->x_flag_tree_tail_merge);
    opts->x_flag_branch_probabilities = read_flag("-fbranch-probabilities", opts->x_flag_branch_probabilities);
    opts->x_flag_code_hoisting = read_flag("-fcode-hoisting", opts->x_flag_code_hoisting);

    printf("\n\x1b[1;32mOptimization flags updated.\x1b[0m\n\n");
}

static void on_parse_function(void *event_data, void *user_data) {
    tree fndecl = static_cast<tree>(event_data);
    if (fndecl->base.code != FUNCTION_DECL) {
        return;
    }

    const char *name = IDENTIFIER_POINTER(DECL_NAME(fndecl));
    const char *required_name = static_cast<const char *>(user_data);
    if (strcmp(required_name, name)) {
        return;
    }

    printf("\nFound function: \x1b[1;32m<%s>\x1b[0m\n", name);

    printf("\x1b[1;36m┌──────────────────────────────────────────────────────┐\x1b[0m\n");
    printf("\x1b[1;36m│\x1b[0m Choose an action:                                    \x1b[1;36m│\x1b[0m\n");
    printf("\x1b[1;36m│\x1b[0m  [\x1b[1;32mp\x1b[0m] Print function-specific optimization flags      \x1b[1;36m│\x1b[0m\n");
    printf("\x1b[1;36m│\x1b[0m  [\x1b[1;32mo\x1b[0m] Print function-specific optimization tree       \x1b[1;36m│\x1b[0m\n");
    printf("\x1b[1;36m│\x1b[0m  [\x1b[1;32mt\x1b[0m] Print function tree                             \x1b[1;36m│\x1b[0m\n");
    printf("\x1b[1;36m│\x1b[0m  [\x1b[1;32mg\x1b[0m] Print global optimization tree                  \x1b[1;36m│\x1b[0m\n");
    printf("\x1b[1;36m│\x1b[0m  [\x1b[1;32ms\x1b[0m] Set function-specific optimizations             \x1b[1;36m│\x1b[0m\n");
    printf("\x1b[1;36m│\x1b[0m  [\x1b[1;32mc\x1b[0m] Continue compilation without any changes        \x1b[1;36m│\x1b[0m\n");
    printf("\x1b[1;36m└──────────────────────────────────────────────────────┘\x1b[0m\n\n");

    char answer[256];
    while (true) {
        printf("[\x1b[1;32mp\x1b[0m/\x1b[1;32mo\x1b[0m/\x1b[1;32mt\x1b[0m/\x1b[1;32mg\x1b[0m/\x1b[1;32ms\x1b[0m/\x1b[1;32mc\x1b[0m]: ");
        scanf("%255s", answer);
        if (!strcmp(answer, "p")) {
            print_optimization_flags(fndecl);
        } else if (!strcmp(answer, "s")) {
            set_optimization_flags(fndecl);
        } else if (!strcmp(answer, "o")) {
            print_optimization_tree(fndecl);
        } else if (!strcmp(answer, "t")) {
            debug_tree(fndecl);
        } else if (!strcmp(answer, "g")) {
            print_global_optimization_tree();
        } else if (!strcmp(answer, "c")) {
            break;
        }
    }
}

int plugin_init(struct plugin_name_args *plugin_info,
                struct plugin_gcc_version *version) {
    if (!plugin_default_version_check(version, &gcc_version)) {
        fprintf(stderr, "GCC plugin version mismatch!\n");
        return 1;
    }

    auto function_name = static_cast<char *>(xmalloc(sizeof(char) * 256));
    printf("Name the function you want to inspect: \x1b[1;32m");
    scanf("%255s", function_name);
    printf("\x1b[0m");

    char answer[256];
    printf("Do you want to inspect \x1b[1;33mbefore\x1b[0m parsing or \x1b[1;34mafter\x1b[0m?\n");

    while (true) {
        printf("[\x1b[1;33mb\x1b[0m/\x1b[1;34ma\x1b[0m]: ");
        scanf("%255s", answer);
        if (!strcmp(answer, "a")) {
            register_callback(
                plugin_info->base_name,
                PLUGIN_FINISH_PARSE_FUNCTION,
                on_parse_function,
                function_name
            );
            break;
        }
        if (!strcmp(answer, "b")) {
            register_callback(
                plugin_info->base_name,
                PLUGIN_START_PARSE_FUNCTION,
                on_parse_function,
                function_name
            );
            break;
        }
    }

    return 0;
}
