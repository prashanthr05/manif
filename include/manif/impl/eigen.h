#ifndef _MANIF_MANIF_EIGEN_H_
#define _MANIF_MANIF_EIGEN_H_


#include <Eigen/Core>

/**
 * @note static_cast<int> to avoid -Wno-enum-compare
 */

//////////////////////
/// Static Asserts ///
//////////////////////

#define static_assert_rows_dim(x, dim) \
  static_assert(static_cast<int>(std::decay<decltype(x)>::type::RowsAtCompileTime) == dim, \
                "x.rows != "#dim" .");

#define static_assert_cols_dim(x, dim) \
  static_assert(static_cast<int>(std::decay<decltype(x)>::type::ColsAtCompileTime) == dim, \
                "x.cols != "#dim" .");

#define static_assert_dim(x, rows, cols) \
  static_assert_rows_dim(x, rows); \
  static_assert_cols_dim(x, cols);

#define static_assert_dim_eq(l,r) \
  static_assert(static_cast<int>(std::decay<decltype(l)>::type::ColsAtCompileTime) == \
                static_cast<int>(std::decay<decltype(r)>::type::ColsAtCompileTime), \
                "lhs.cols != rhs.cols !"); \
  static_assert(static_cast<int>(std::decay<decltype(l)>::type::RowsAtCompileTime) == \
                static_cast<int>(std::decay<decltype(r)>::type::RowsAtCompileTime), \
                "lhs.rows != rhs.rows !");

#define static_assert_is_vector(x) \
  static_assert_cols_dim(x, 1);

#define static_assert_vector_dim(x, dim) \
  static_assert_is_vector(x); \
  static_assert_rows_dim(x, dim);

#define static_assert_is_colmajor_vector(x) \
  static_assert_rows_dim(x, 1);

#define static_assert_colmajor_vector_dim(x, dim) \
  static_assert_is_colmajor_vector(x); \
  static_assert_cols_dim(x, dim);

///////////////
/// Asserts ///
///////////////

#define assert_rows_dim(x, dim) \
  static_assert(static_cast<int>(std::decay<decltype(x)>::type::RowsAtCompileTime) == dim or \
                std::decay<decltype(x)>::type::RowsAtCompileTime == Eigen::Dynamic, \
                "x.rows != "#dim" ."); \
  assert(x.rows() == dim && "x.cols != "#dim" .");

#define assert_cols_dim(x, dim) \
  static_assert(static_cast<int>(std::decay<decltype(x)>::type::ColsAtCompileTime) == dim or \
                std::decay<decltype(x)>::type::ColsAtCompileTime == Eigen::Dynamic, \
                "x.cols != "#dim" ."); \
  assert(x.cols() == dim && "x.rows != "#dim" .");

#define assert_dim(x, rows, cols) \
  assert_rows_dim(x, rows); \
  assert_cols_dim(x, cols);

#define assert_dim_eq(l,r) \
  static_assert(static_cast<int>(std::decay<decltype(l)>::type::ColsAtCompileTime) == \
                static_cast<int>(std::decay<decltype(r)>::type::ColsAtCompileTime) or \
                std::decay<decltype(l)>::type::ColsAtCompileTime == Eigen::Dynamic or \
                std::decay<decltype(r)>::type::ColsAtCompileTime == Eigen::Dynamic, \
                "lhs.cols != rhs.cols !"); \
  static_assert(static_cast<int>(std::decay<decltype(l)>::type::RowsAtCompileTime) == \
                static_cast<int>(std::decay<decltype(r)>::type::RowsAtCompileTime) or \
                std::decay<decltype(l)>::type::RowsAtCompileTime == Eigen::Dynamic or \
                std::decay<decltype(r)>::type::RowsAtCompileTime == Eigen::Dynamic, \
                "lhs.rows != rhs.rows !"); \
  assert(l.rows() == r.rows() && "lhs.rows != rhs.rows !"); \
  assert(l.cols() == r.cols() && "lhs.cols != rhs.cols !"); \

#define assert_is_vector(x) \
  static_assert(std::decay<decltype(x)>::type::ColsAtCompileTime ==  1 or \
                std::decay<decltype(x)>::type::ColsAtCompileTime == Eigen::Dynamic, \
                "Expected a vector !"); \
  assert(x.cols() == 1 && "Expected a vector !"); \

#define assert_vector_dim(x, dim) \
  assert_is_vector(x); \
  assert_rows_dim(x, dim);

#define assert_is_colmajor_vector(x) \
  static_assert(std::decay<decltype(x)>::type::RowsAtCompileTime ==  1 or \
                std::decay<decltype(x)>::type::RowsAtCompileTime == Eigen::Dynamic, \
                "Expected a column-major vector !"); \
  assert(x.rows() == 1 && "Expected a column-major vector !"); \

#define assert_colmajor_vector_dim(x, dim) \
  assert_is_colmajor_vector(x); \
  assert_cols_dim(x, dim);

namespace manif {
namespace internal {

/**
 * @brief traitscast specialization that come handy when writing thing like
 * using Matrix3f = typename traitscast<Matrix3d, float>::cast;
 */
template <typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols, typename NewScalar>
struct traitscast<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>, NewScalar>
{
  using cast = Eigen::Matrix<NewScalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>;
};

} /* namespace internal */

template <typename _Scalar>
typename std::enable_if<std::is_arithmetic<_Scalar>::value,
                        Eigen::Matrix<_Scalar, 2, 2>>::type
skew(const _Scalar v)
{
  return (Eigen::Matrix<_Scalar, 2, 2>() <<
             _Scalar(0.), -v,
             v, _Scalar(0.) ).finished();
}

template <typename _Derived>
typename std::enable_if<std::is_base_of<Eigen::MatrixBase<_Derived>,
                                        _Derived>::value and
                        _Derived::RowsAtCompileTime == 3,
                        Eigen::Matrix<typename _Derived::Scalar, 3, 3>>::type
skew(const Eigen::MatrixBase<_Derived>& v)
{
  assert_vector_dim(v, 3);

  using T = typename _Derived::Scalar;

  return (Eigen::Matrix<T, 3, 3>() <<
             T(0.),  -v(2),   +v(1),
            +v(2),    T(0.),  -v(0),
            -v(1),   +v(0),    T(0.) ).finished();
}

//template <typename _Scalar>
//Eigen::Matrix<_Scalar, 2, 2>
//skew2(const _Scalar v)
//{
//  return (Eigen::Matrix<_Scalar, 2, 2>() <<
//             _Scalar(0.), -v,
//             v, _Scalar(0.) ).finished();
//}

//template <typename _Derived>
//Eigen::Matrix<typename _Derived::Scalar, 3, 3>
//skew3(const Eigen::MatrixBase<_Derived>& v)
//{
//  assert_vector_dim(v, 3);

//  using T = typename _Derived::Scalar;

//  return (Eigen::Matrix<T, 3, 3>() <<
//             T(0.),  -v(2),   +v(1),
//            +v(2),    T(0.),  -v(0),
//            -v(1),   +v(0),    T(0.) ).finished();
//}

} /* namespace manif */

#endif /* _MANIF_MANIF_EIGEN_H_ */