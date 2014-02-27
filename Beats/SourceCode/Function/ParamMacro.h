#ifndef BEATS_FUNCTION_PARAMMACRO_H__INCLUDE
#define BEATS_FUNCTION_PARAMMACRO_H__INCLUDE

// Basic macro for 1 data

#define GEN_0(macro, data, n) 
#define GEN_1(macro, data, n) macro(data, 0)
#define GEN_2(macro, data, n) GEN_1(macro, data, n), macro(data, 1)
#define GEN_3(macro, data, n) GEN_2(macro, data, n), macro(data, 2)
#define GEN_4(macro, data, n) GEN_3(macro, data, n), macro(data, 3)
#define GEN_5(macro, data, n) GEN_4(macro, data, n), macro(data, 4)
#define GEN_6(macro, data, n) GEN_5(macro, data, n), macro(data, 5)
#define GEN_7(macro, data, n) GEN_6(macro, data, n), macro(data, 6)

#define GEN(macro, data, n)      GEN_##n(macro, data, n)

#define GEN_PARAM_IMPL(data, n)  data##n
#define GEN_PARAM(n, data)       GEN( GEN_PARAM_IMPL, data, n)

// Basic macro for 2 datas

#define GEN2_0(macro, data1, data2, n) 
#define GEN2_1(macro, data1, data2, n) macro(data1, data2, 0)
#define GEN2_2(macro, data1, data2, n) GEN2_1(macro, data1, data2, n), macro(data1, data2, 1)
#define GEN2_3(macro, data1, data2, n) GEN2_2(macro, data1, data2, n), macro(data1, data2, 2)
#define GEN2_4(macro, data1, data2, n) GEN2_3(macro, data1, data2, n), macro(data1, data2, 3)
#define GEN2_5(macro, data1, data2, n) GEN2_4(macro, data1, data2, n), macro(data1, data2, 4)
#define GEN2_6(macro, data1, data2, n) GEN2_5(macro, data1, data2, n), macro(data1, data2, 5)
#define GEN2_7(macro, data1, data2, n) GEN2_6(macro, data1, data2, n), macro(data1, data2, 6)

#define GEN2(macro, p1, p2, n)          GEN2_##n(macro, p1, p2, n)

#define GEN_PARAM_EX_IMPL(p1, p2, n)    p1##n p2##n
#define GEN_PARAM_EX(n, p1, p2)         GEN2( GEN_PARAM_EX_IMPL, p1, p2, n)


// For typedef (e.g. typedef Arg0 ArgType0;)

#define GEN3_0(macro, data1, data2, n) 
#define GEN3_1(macro, data1, data2, n) macro(data1, data2, 0);
#define GEN3_2(macro, data1, data2, n) GEN3_1(macro, data1, data2, n) macro(data1, data2, 1);
#define GEN3_3(macro, data1, data2, n) GEN3_2(macro, data1, data2, n) macro(data1, data2, 2);
#define GEN3_4(macro, data1, data2, n) GEN3_3(macro, data1, data2, n) macro(data1, data2, 3);
#define GEN3_5(macro, data1, data2, n) GEN3_4(macro, data1, data2, n) macro(data1, data2, 4);
#define GEN3_6(macro, data1, data2, n) GEN3_5(macro, data1, data2, n) macro(data1, data2, 5);
#define GEN3_7(macro, data1, data2, n) GEN3_6(macro, data1, data2, n) macro(data1, data2, 6);

#define GEN3(macro, p1, p2, n)               GEN3_##n(macro, p1, p2, n)

#define GEN_PARAM_EX2_IMPL(p1, p2, n)    p1##n p2##n
#define GEN_PARAM_EX2(n, p1, p2)         GEN3( GEN_PARAM_EX2_IMPL, p1, p2, n)

// Generate name with number

#define GEN_NAME_IMP(name, n) name##n
#define GEN_NAME(name, n) GEN_NAME_IMP(name, n)

#endif