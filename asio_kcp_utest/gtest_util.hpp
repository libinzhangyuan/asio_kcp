#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <functional>

namespace testing {

template <typename return_t,
          typename cmp_t,
          typename Pred,
          typename T1>
AssertionResult AssertCmpPred1Helper(const char* pred_text,
                                    const char* e1,
                                    const return_t& ret,
                                    const cmp_t& cmp_value,
                                    Pred pred,
                                    const T1& v1) {
  if (pred(v1)) return AssertionSuccess();

  return AssertionFailure() << pred_text << "("
                            << e1 << ") = " << ret << ". Want " << cmp_value << ".  where"
                            << "\n" << e1 << " evaluates to " << v1;
}

#define GTEST_CMP_PRED1_(cmp_value, pred, v1, on_failure)\
  GTEST_ASSERT_(::testing::AssertCmpPred1Helper(#pred, \
                                                #v1, \
                                                pred(v1), \
                                                cmp_value, \
                                                [](decltype(v1) a) {return (pred(a) == cmp_value);}, \
                                                v1), on_failure)

// Binary predicate assertion macros.
#define EXPECT_CMP_PRED1(cmp_value, pred, v1) \
  GTEST_CMP_PRED1_(cmp_value, pred, v1, GTEST_NONFATAL_FAILURE_)
#define ASSERT_CMP_PRED1(cmp_value, pred, v1) \
  GTEST_CMP_PRED1_(cmp_value, pred, v1, GTEST_FATAL_FAILURE_)




template <typename return_t,
          typename cmp_t,
          typename Pred,
          typename T1,
          typename T2>
AssertionResult AssertCmpPred2Helper(const char* pred_text,
                                    const char* e1,
                                    const char* e2,
                                    const return_t& ret,
                                    const cmp_t& cmp_value,
                                    Pred pred,
                                    const T1& v1,
                                    const T2& v2) {
  if (pred(v1, v2)) return AssertionSuccess();

  return AssertionFailure() << pred_text << "("
                            << e1 << ", "
                            << e2 << ") = " << ret << ". Want " << cmp_value << ".  where"
                            << "\n" << e1 << " evaluates to " << v1
                            << "\n" << e2 << " evaluates to " << v2;
}

#define GTEST_CMP_PRED2_(cmp_value, pred, v1, v2, on_failure)\
  GTEST_ASSERT_(::testing::AssertCmpPred2Helper(#pred, \
                                                #v1, \
                                                #v2, \
                                                pred(v1, v2), \
                                                cmp_value, \
                                                [](decltype(v1) a, decltype(v2) b) {return (pred(a, b) == cmp_value);}, \
                                                v1, \
                                                v2), on_failure)

// Binary predicate assertion macros.
#define EXPECT_CMP_PRED2(cmp_value, pred, v1, v2) \
  GTEST_CMP_PRED2_(cmp_value, pred, v1, v2, GTEST_NONFATAL_FAILURE_)
#define ASSERT_CMP_PRED2(cmp_value, pred, v1, v2) \
  GTEST_CMP_PRED2_(cmp_value, pred, v1, v2, GTEST_FATAL_FAILURE_)



template <typename return_t,
          typename cmp_t,
          typename Pred,
          typename T1,
          typename T2,
          typename T3>
AssertionResult AssertCmpPred3Helper(const char* pred_text,
                                    const char* e1,
                                    const char* e2,
                                    const char* e3,
                                    const return_t& ret,
                                    const cmp_t& cmp_value,
                                    Pred pred,
                                    const T1& v1,
                                    const T2& v2,
                                    const T3& v3) {
  if (pred(v1, v2, v3)) return AssertionSuccess();

  return AssertionFailure() << pred_text << "("
                            << e1 << ", " << e2 << ", "
                            << e3 << ") = " << ret << ". Want " << cmp_value << ".  where"
                            << "\n" << e1 << " evaluates to " << v1
                            << "\n" << e2 << " evaluates to " << v2
                            << "\n" << e3 << " evaluates to " << v3;
}

#define GTEST_CMP_PRED3_(cmp_value, pred, v1, v2, v3, on_failure)\
  GTEST_ASSERT_(::testing::AssertCmpPred3Helper(#pred, \
                                                #v1, \
                                                #v2, \
                                                #v3, \
                                                pred(v1, v2, v3), \
                                                cmp_value, \
                                                [](decltype(v1) a, decltype(v2) b, decltype(v3) c) {return (pred(a, b, c) == cmp_value);}, \
                                                v1, v2, \
                                                v3), on_failure)

// Binary predicate assertion macros.
#define EXPECT_CMP_PRED3(cmp_value, pred, v1, v2, v3) \
  GTEST_CMP_PRED3_(cmp_value, pred, v1, v2, v3, GTEST_NONFATAL_FAILURE_)
#define ASSERT_CMP_PRED3(cmp_value, pred, v1, v2, v3) \
  GTEST_CMP_PRED3_(cmp_value, pred, v1, v2, v3, GTEST_FATAL_FAILURE_)


} // namespace testing

