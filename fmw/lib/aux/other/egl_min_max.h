// #define MIN(a, b) (a) > (b) ? (b) : (a)
// #define MAX(a, b) (a) > (b) ? (a) : (b)

#define MAX(a, b)            \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define MIN(a, b)            \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})