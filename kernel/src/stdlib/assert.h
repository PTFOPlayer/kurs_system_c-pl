#define assert(cond)                                                       \
    if (!cond) {                                                           \
        printf("\nPanic on assert: %s \n\tin function: %s\n\ton line: %d", \
               #cond, __func__, __LINE__);                                 \
        while (1) {                                                        \
        }                                                                  \
    }
#define assert_eq(lhs, rhs) assert((lhs == rhs))
#define assert_ne(lhs, rhs) assert((lhs != rhs))