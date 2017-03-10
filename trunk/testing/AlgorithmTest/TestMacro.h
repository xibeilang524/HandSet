#ifndef _TEST_MACRO_H_
#define _TEST_MACRO_H_

///< 浮点数测试宏（默认测试精度1e-5）
///< 以指定精度测试浮点数相等宏
#define AST_F_EX(v1, v2, prt) ASSERT_NEAR(v1, v2, prt) \
    <<"----------------------------------------------------------------------" \
    "----------"                                                          

#define EPT_F_EX(v1, v2, prt) EXPECT_NEAR(v1, v2, prt) \
    <<"----------------------------------------------------------------------" \
    "----------"                                                                

///< 以精度为0.01测试浮点数相等宏
#define AST_F(v1, v2) ASSERT_NEAR(v1, v2, 1e-5) \
    <<"----------------------------------------------------------------------" \
    "----------"                                                          

#define EPT_F(v1, v2) EXPECT_NEAR(v1, v2, 1e-5) \
    <<"----------------------------------------------------------------------" \
    "----------"                                                                

#endif