//! Streaming SIMD Extensions (SSE)

use coresimd::simd::*;
use coresimd::simd_llvm::*;
use coresimd::x86::*;
use intrinsics;
use mem;
use ptr;

#[cfg(test)]
use stdsimd_test::assert_instr;

/// Adds the first component of `a` and `b`, the other components are copied
/// from `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_add_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(addss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_add_ss(a: __m128, b: __m128) -> __m128 {
    addss(a, b)
}

/// Adds __m128 vectors.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_add_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(addps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_add_ps(a: __m128, b: __m128) -> __m128 {
    simd_add(a, b)
}

/// Subtracts the first component of `b` from `a`, the other components are
/// copied from `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_sub_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(subss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_sub_ss(a: __m128, b: __m128) -> __m128 {
    subss(a, b)
}

/// Subtracts __m128 vectors.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_sub_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(subps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_sub_ps(a: __m128, b: __m128) -> __m128 {
    simd_sub(a, b)
}

/// Multiplies the first component of `a` and `b`, the other components are
/// copied from `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_mul_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(mulss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_mul_ss(a: __m128, b: __m128) -> __m128 {
    mulss(a, b)
}

/// Multiplies __m128 vectors.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_mul_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(mulps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_mul_ps(a: __m128, b: __m128) -> __m128 {
    simd_mul(a, b)
}

/// Divides the first component of `b` by `a`, the other components are
/// copied from `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_div_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(divss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_div_ss(a: __m128, b: __m128) -> __m128 {
    divss(a, b)
}

/// Divides __m128 vectors.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_div_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(divps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_div_ps(a: __m128, b: __m128) -> __m128 {
    simd_div(a, b)
}

/// Return the square root of the first single-precision (32-bit)
/// floating-point element in `a`, the other elements are unchanged.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_sqrt_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(sqrtss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_sqrt_ss(a: __m128) -> __m128 {
    sqrtss(a)
}

/// Return the square root of packed single-precision (32-bit) floating-point
/// elements in `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_sqrt_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(sqrtps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_sqrt_ps(a: __m128) -> __m128 {
    sqrtps(a)
}

/// Return the approximate reciprocal of the first single-precision
/// (32-bit) floating-point element in `a`, the other elements are unchanged.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_rcp_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(rcpss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_rcp_ss(a: __m128) -> __m128 {
    rcpss(a)
}

/// Return the approximate reciprocal of packed single-precision (32-bit)
/// floating-point elements in `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_rcp_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(rcpps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_rcp_ps(a: __m128) -> __m128 {
    rcpps(a)
}

/// Return the approximate reciprocal square root of the fist single-precision
/// (32-bit) floating-point elements in `a`, the other elements are unchanged.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_rsqrt_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(rsqrtss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_rsqrt_ss(a: __m128) -> __m128 {
    rsqrtss(a)
}

/// Return the approximate reciprocal square root of packed single-precision
/// (32-bit) floating-point elements in `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_rsqrt_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(rsqrtps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_rsqrt_ps(a: __m128) -> __m128 {
    rsqrtps(a)
}

/// Compare the first single-precision (32-bit) floating-point element of `a`
/// and `b`, and return the minimum value in the first element of the return
/// value, the other elements are copied from `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_min_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(minss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_min_ss(a: __m128, b: __m128) -> __m128 {
    minss(a, b)
}

/// Compare packed single-precision (32-bit) floating-point elements in `a` and
/// `b`, and return the corresponding minimum values.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_min_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(minps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_min_ps(a: __m128, b: __m128) -> __m128 {
    minps(a, b)
}

/// Compare the first single-precision (32-bit) floating-point element of `a`
/// and `b`, and return the maximum value in the first element of the return
/// value, the other elements are copied from `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_max_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(maxss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_max_ss(a: __m128, b: __m128) -> __m128 {
    maxss(a, b)
}

/// Compare packed single-precision (32-bit) floating-point elements in `a` and
/// `b`, and return the corresponding maximum values.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_max_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(maxps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_max_ps(a: __m128, b: __m128) -> __m128 {
    maxps(a, b)
}

/// Bitwise AND of packed single-precision (32-bit) floating-point elements.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_and_ps)
#[inline]
#[target_feature(enable = "sse")]
// i586 only seems to generate plain `and` instructions, so ignore it.
#[cfg_attr(
    all(test, any(target_arch = "x86_64", target_feature = "sse2")),
    assert_instr(andps)
)]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_and_ps(a: __m128, b: __m128) -> __m128 {
    let a: __m128i = mem::transmute(a);
    let b: __m128i = mem::transmute(b);
    mem::transmute(simd_and(a, b))
}

/// Bitwise AND-NOT of packed single-precision (32-bit) floating-point
/// elements.
///
/// Computes `!a & b` for each bit in `a` and `b`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_andnot_ps)
#[inline]
#[target_feature(enable = "sse")]
// i586 only seems to generate plain `not` and `and` instructions, so ignore
// it.
#[cfg_attr(
    all(test, any(target_arch = "x86_64", target_feature = "sse2")),
    assert_instr(andnps)
)]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_andnot_ps(a: __m128, b: __m128) -> __m128 {
    let a: __m128i = mem::transmute(a);
    let b: __m128i = mem::transmute(b);
    let mask: __m128i = mem::transmute(i32x4::splat(-1));
    mem::transmute(simd_and(simd_xor(mask, a), b))
}

/// Bitwise OR of packed single-precision (32-bit) floating-point elements.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_or_ps)
#[inline]
#[target_feature(enable = "sse")]
// i586 only seems to generate plain `or` instructions, so we ignore it.
#[cfg_attr(
    all(test, any(target_arch = "x86_64", target_feature = "sse2")),
    assert_instr(orps)
)]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_or_ps(a: __m128, b: __m128) -> __m128 {
    let a: __m128i = mem::transmute(a);
    let b: __m128i = mem::transmute(b);
    mem::transmute(simd_or(a, b))
}

/// Bitwise exclusive OR of packed single-precision (32-bit) floating-point
/// elements.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_xor_ps)
#[inline]
#[target_feature(enable = "sse")]
// i586 only seems to generate plain `xor` instructions, so we ignore it.
#[cfg_attr(
    all(test, any(target_arch = "x86_64", target_feature = "sse2")),
    assert_instr(xorps)
)]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_xor_ps(a: __m128, b: __m128) -> __m128 {
    let a: __m128i = mem::transmute(a);
    let b: __m128i = mem::transmute(b);
    mem::transmute(simd_xor(a, b))
}

/// Compare the lowest `f32` of both inputs for equality. The lowest 32 bits of
/// the result will be `0xffffffff` if the two inputs are equal, or `0`
/// otherwise. The upper 96 bits of the result are the upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpeq_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpeqss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpeq_ss(a: __m128, b: __m128) -> __m128 {
    cmpss(a, b, 0)
}

/// Compare the lowest `f32` of both inputs for less than. The lowest 32 bits
/// of the result will be `0xffffffff` if `a.extract(0)` is less than
/// `b.extract(0)`, or `0` otherwise. The upper 96 bits of the result are the
/// upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmplt_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpltss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmplt_ss(a: __m128, b: __m128) -> __m128 {
    cmpss(a, b, 1)
}

/// Compare the lowest `f32` of both inputs for less than or equal. The lowest
/// 32 bits of the result will be `0xffffffff` if `a.extract(0)` is less than
/// or equal `b.extract(0)`, or `0` otherwise. The upper 96 bits of the result
/// are the upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmple_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpless))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmple_ss(a: __m128, b: __m128) -> __m128 {
    cmpss(a, b, 2)
}

/// Compare the lowest `f32` of both inputs for greater than. The lowest 32
/// bits of the result will be `0xffffffff` if `a.extract(0)` is greater
/// than `b.extract(0)`, or `0` otherwise. The upper 96 bits of the result
/// are the upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpgt_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpltss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpgt_ss(a: __m128, b: __m128) -> __m128 {
    simd_shuffle4(a, cmpss(b, a, 1), [4, 1, 2, 3])
}

/// Compare the lowest `f32` of both inputs for greater than or equal. The
/// lowest 32 bits of the result will be `0xffffffff` if `a.extract(0)` is
/// greater than or equal `b.extract(0)`, or `0` otherwise. The upper 96 bits
/// of the result are the upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpge_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpless))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpge_ss(a: __m128, b: __m128) -> __m128 {
    simd_shuffle4(a, cmpss(b, a, 2), [4, 1, 2, 3])
}

/// Compare the lowest `f32` of both inputs for inequality. The lowest 32 bits
/// of the result will be `0xffffffff` if `a.extract(0)` is not equal to
/// `b.extract(0)`, or `0` otherwise. The upper 96 bits of the result are the
/// upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpneq_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpneqss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpneq_ss(a: __m128, b: __m128) -> __m128 {
    cmpss(a, b, 4)
}

/// Compare the lowest `f32` of both inputs for not-less-than. The lowest 32
/// bits of the result will be `0xffffffff` if `a.extract(0)` is not less than
/// `b.extract(0)`, or `0` otherwise. The upper 96 bits of the result are the
/// upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpnlt_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpnltss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpnlt_ss(a: __m128, b: __m128) -> __m128 {
    cmpss(a, b, 5)
}

/// Compare the lowest `f32` of both inputs for not-less-than-or-equal. The
/// lowest 32 bits of the result will be `0xffffffff` if `a.extract(0)` is not
/// less than or equal to `b.extract(0)`, or `0` otherwise. The upper 96 bits
/// of the result are the upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpnle_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpnless))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpnle_ss(a: __m128, b: __m128) -> __m128 {
    cmpss(a, b, 6)
}

/// Compare the lowest `f32` of both inputs for not-greater-than. The lowest 32
/// bits of the result will be `0xffffffff` if `a.extract(0)` is not greater
/// than `b.extract(0)`, or `0` otherwise. The upper 96 bits of the result are
/// the upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpngt_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpnltss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpngt_ss(a: __m128, b: __m128) -> __m128 {
    simd_shuffle4(a, cmpss(b, a, 5), [4, 1, 2, 3])
}

/// Compare the lowest `f32` of both inputs for not-greater-than-or-equal. The
/// lowest 32 bits of the result will be `0xffffffff` if `a.extract(0)` is not
/// greater than or equal to `b.extract(0)`, or `0` otherwise. The upper 96
/// bits of the result are the upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpnge_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpnless))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpnge_ss(a: __m128, b: __m128) -> __m128 {
    simd_shuffle4(a, cmpss(b, a, 6), [4, 1, 2, 3])
}

/// Check if the lowest `f32` of both inputs are ordered. The lowest 32 bits of
/// the result will be `0xffffffff` if neither of `a.extract(0)` or
/// `b.extract(0)` is a NaN, or `0` otherwise. The upper 96 bits of the result
/// are the upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpord_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpordss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpord_ss(a: __m128, b: __m128) -> __m128 {
    cmpss(a, b, 7)
}

/// Check if the lowest `f32` of both inputs are unordered. The lowest 32 bits
/// of the result will be `0xffffffff` if any of `a.extract(0)` or
/// `b.extract(0)` is a NaN, or `0` otherwise. The upper 96 bits of the result
/// are the upper 96 bits of `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpunord_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpunordss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpunord_ss(a: __m128, b: __m128) -> __m128 {
    cmpss(a, b, 3)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// The result in the output vector will be `0xffffffff` if the input elements
/// were equal, or `0` otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpeq_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpeqps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpeq_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(a, b, 0)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// The result in the output vector will be `0xffffffff` if the input element
/// in `a` is less than the corresponding element in `b`, or `0` otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmplt_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpltps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmplt_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(a, b, 1)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// The result in the output vector will be `0xffffffff` if the input element
/// in `a` is less than or equal to the corresponding element in `b`, or `0`
/// otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmple_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpleps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmple_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(a, b, 2)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// The result in the output vector will be `0xffffffff` if the input element
/// in `a` is greater than the corresponding element in `b`, or `0` otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpgt_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpltps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpgt_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(b, a, 1)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// The result in the output vector will be `0xffffffff` if the input element
/// in `a` is greater than or equal to the corresponding element in `b`, or `0`
/// otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpge_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpleps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpge_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(b, a, 2)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// The result in the output vector will be `0xffffffff` if the input elements
/// are *not* equal, or `0` otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpneq_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpneqps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpneq_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(a, b, 4)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// The result in the output vector will be `0xffffffff` if the input element
/// in `a` is *not* less than the corresponding element in `b`, or `0`
/// otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpnlt_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpnltps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpnlt_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(a, b, 5)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// The result in the output vector will be `0xffffffff` if the input element
/// in `a` is *not* less than or equal to the corresponding element in `b`, or
/// `0` otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpnle_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpnleps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpnle_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(a, b, 6)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// The result in the output vector will be `0xffffffff` if the input element
/// in `a` is *not* greater than the corresponding element in `b`, or `0`
/// otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpngt_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpnltps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpngt_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(b, a, 5)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// The result in the output vector will be `0xffffffff` if the input element
/// in `a` is *not* greater than or equal to the corresponding element in `b`,
/// or `0` otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpnge_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpnleps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpnge_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(b, a, 6)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// Returns four floats that have one of two possible bit patterns. The element
/// in the output vector will be `0xffffffff` if the input elements in `a` and
/// `b` are ordered (i.e., neither of them is a NaN), or 0 otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpord_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpordps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpord_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(b, a, 7)
}

/// Compare each of the four floats in `a` to the corresponding element in `b`.
/// Returns four floats that have one of two possible bit patterns. The element
/// in the output vector will be `0xffffffff` if the input elements in `a` and
/// `b` are unordered (i.e., at least on of them is a NaN), or 0 otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cmpunord_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cmpunordps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cmpunord_ps(a: __m128, b: __m128) -> __m128 {
    cmpps(b, a, 3)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if they are equal, or `0` otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_comieq_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(comiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_comieq_ss(a: __m128, b: __m128) -> i32 {
    comieq_ss(a, b)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if the value from `a` is less than the one from `b`, or `0` otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_comilt_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(comiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_comilt_ss(a: __m128, b: __m128) -> i32 {
    comilt_ss(a, b)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if the value from `a` is less than or equal to the one from `b`, or `0`
/// otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_comile_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(comiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_comile_ss(a: __m128, b: __m128) -> i32 {
    comile_ss(a, b)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if the value from `a` is greater than the one from `b`, or `0`
/// otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_comigt_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(comiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_comigt_ss(a: __m128, b: __m128) -> i32 {
    comigt_ss(a, b)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if the value from `a` is greater than or equal to the one from `b`, or
/// `0` otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_comige_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(comiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_comige_ss(a: __m128, b: __m128) -> i32 {
    comige_ss(a, b)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if they are *not* equal, or `0` otherwise.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_comineq_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(comiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_comineq_ss(a: __m128, b: __m128) -> i32 {
    comineq_ss(a, b)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if they are equal, or `0` otherwise. This instruction will not signal
/// an exception if either argument is a quiet NaN.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_ucomieq_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(ucomiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_ucomieq_ss(a: __m128, b: __m128) -> i32 {
    ucomieq_ss(a, b)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if the value from `a` is less than the one from `b`, or `0` otherwise.
/// This instruction will not signal an exception if either argument is a quiet
/// NaN.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_ucomilt_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(ucomiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_ucomilt_ss(a: __m128, b: __m128) -> i32 {
    ucomilt_ss(a, b)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if the value from `a` is less than or equal to the one from `b`, or `0`
/// otherwise. This instruction will not signal an exception if either argument
/// is a quiet NaN.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_ucomile_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(ucomiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_ucomile_ss(a: __m128, b: __m128) -> i32 {
    ucomile_ss(a, b)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if the value from `a` is greater than the one from `b`, or `0`
/// otherwise. This instruction will not signal an exception if either argument
/// is a quiet NaN.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_ucomigt_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(ucomiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_ucomigt_ss(a: __m128, b: __m128) -> i32 {
    ucomigt_ss(a, b)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if the value from `a` is greater than or equal to the one from `b`, or
/// `0` otherwise. This instruction will not signal an exception if either
/// argument is a quiet NaN.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_ucomige_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(ucomiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_ucomige_ss(a: __m128, b: __m128) -> i32 {
    ucomige_ss(a, b)
}

/// Compare two 32-bit floats from the low-order bits of `a` and `b`. Returns
/// `1` if they are *not* equal, or `0` otherwise. This instruction will not
/// signal an exception if either argument is a quiet NaN.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_ucomineq_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(ucomiss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_ucomineq_ss(a: __m128, b: __m128) -> i32 {
    ucomineq_ss(a, b)
}

/// Convert the lowest 32 bit float in the input vector to a 32 bit integer.
///
/// The result is rounded according to the current rounding mode. If the result
/// cannot be represented as a 32 bit integer the result will be `0x8000_0000`
/// (`std::i32::MIN`) or an invalid operation floating point exception if
/// unmasked (see [`_mm_setcsr`](fn._mm_setcsr.html)).
///
/// This corresponds to the `CVTSS2SI` instruction (with 32 bit output).
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cvtss_si32)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cvtss2si))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cvtss_si32(a: __m128) -> i32 {
    cvtss2si(a)
}

/// Alias for [`_mm_cvtss_si32`](fn._mm_cvtss_si32.html).
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cvt_ss2si)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cvtss2si))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cvt_ss2si(a: __m128) -> i32 {
    _mm_cvtss_si32(a)
}

/// Convert the lowest 32 bit float in the input vector to a 32 bit integer
/// with
/// truncation.
///
/// The result is rounded always using truncation (round towards zero). If the
/// result cannot be represented as a 32 bit integer the result will be
/// `0x8000_0000` (`std::i32::MIN`) or an invalid operation floating point
/// exception if unmasked (see [`_mm_setcsr`](fn._mm_setcsr.html)).
///
/// This corresponds to the `CVTTSS2SI` instruction (with 32 bit output).
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cvttss_si32)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cvttss2si))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cvttss_si32(a: __m128) -> i32 {
    cvttss2si(a)
}

/// Alias for [`_mm_cvttss_si32`](fn._mm_cvttss_si32.html).
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cvtt_ss2si)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cvttss2si))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cvtt_ss2si(a: __m128) -> i32 {
    _mm_cvttss_si32(a)
}

/// Extract the lowest 32 bit float from the input vector.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cvtss_f32)
#[inline]
#[target_feature(enable = "sse")]
// No point in using assert_instrs. In Unix x86_64 calling convention this is a
// no-op, and on Windows it's just a `mov`.
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cvtss_f32(a: __m128) -> f32 {
    simd_extract(a, 0)
}

/// Convert a 32 bit integer to a 32 bit float. The result vector is the input
/// vector `a` with the lowest 32 bit float replaced by the converted integer.
///
/// This intrinsic corresponds to the `CVTSI2SS` instruction (with 32 bit
/// input).
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cvtsi32_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cvtsi2ss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cvtsi32_ss(a: __m128, b: i32) -> __m128 {
    cvtsi2ss(a, b)
}

/// Alias for [`_mm_cvtsi32_ss`](fn._mm_cvtsi32_ss.html).
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_cvt_si2ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(cvtsi2ss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_cvt_si2ss(a: __m128, b: i32) -> __m128 {
    _mm_cvtsi32_ss(a, b)
}

/// Construct a `__m128` with the lowest element set to `a` and the rest set to
/// zero.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_set_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_set_ss(a: f32) -> __m128 {
    __m128(a, 0.0, 0.0, 0.0)
}

/// Construct a `__m128` with all element set to `a`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_set1_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(shufps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_set1_ps(a: f32) -> __m128 {
    __m128(a, a, a, a)
}

/// Alias for [`_mm_set1_ps`](fn._mm_set1_ps.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_set_ps1)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(shufps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_set_ps1(a: f32) -> __m128 {
    _mm_set1_ps(a)
}

/// Construct a `__m128` from four floating point values highest to lowest.
///
/// Note that `a` will be the highest 32 bits of the result, and `d` the
/// lowest. This matches the standard way of writing bit patterns on x86:
///
/// ```text
///  bit    127 .. 96  95 .. 64  63 .. 32  31 .. 0
///        +---------+---------+---------+---------+
///        |    a    |    b    |    c    |    d    |   result
///        +---------+---------+---------+---------+
/// ```
///
/// Alternatively:
///
/// ```text
/// let v = _mm_set_ps(d, c, b, a);
/// ```
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_set_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(unpcklps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_set_ps(a: f32, b: f32, c: f32, d: f32) -> __m128 {
    __m128(d, c, b, a)
}

/// Construct a `__m128` from four floating point values lowest to highest.
///
/// This matches the memory order of `__m128`, i.e., `a` will be the lowest 32
/// bits of the result, and `d` the highest.
///
/// ```text
/// assert_eq!(__m128::new(a, b, c, d), _mm_setr_ps(a, b, c, d));
/// ```
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_setr_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(all(test, target_arch = "x86_64"), assert_instr(unpcklps))]
// On a 32-bit architecture it just copies the operands from the stack.
#[cfg_attr(all(test, target_arch = "x86"), assert_instr(movaps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_setr_ps(a: f32, b: f32, c: f32, d: f32) -> __m128 {
    __m128(a, b, c, d)
}

/// Construct a `__m128` with all elements initialized to zero.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_setzero_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(xorps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_setzero_ps() -> __m128 {
    __m128(0.0, 0.0, 0.0, 0.0)
}

/// A utility function for creating masks to use with Intel shuffle and permute intrinsics.
#[inline]
#[allow(non_snake_case)]
#[unstable(feature = "stdsimd", issue = "27731")]
pub const fn _MM_SHUFFLE(z: u32, y: u32, x: u32, w: u32) -> u32 {
        (z << 6) | (y << 4) | (x << 2) | w
}

/// Shuffle packed single-precision (32-bit) floating-point elements in `a` and
/// `b` using `mask`.
///
/// The lower half of result takes values from `a` and the higher half from
/// `b`. Mask is split to 2 control bits each to index the element from inputs.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_shuffle_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(shufps, mask = 3))]
#[rustc_args_required_const(2)]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_shuffle_ps(a: __m128, b: __m128, mask: u32) -> __m128 {
    let mask = (mask & 0xFF) as u8;

    macro_rules! shuffle_done {
        ($x01:expr, $x23:expr, $x45:expr, $x67:expr) => {
            simd_shuffle4(a, b, [$x01, $x23, $x45, $x67])
        };
    }
    macro_rules! shuffle_x67 {
        ($x01:expr, $x23:expr, $x45:expr) => {
            match (mask >> 6) & 0b11 {
                0b00 => shuffle_done!($x01, $x23, $x45, 4),
                0b01 => shuffle_done!($x01, $x23, $x45, 5),
                0b10 => shuffle_done!($x01, $x23, $x45, 6),
                _ => shuffle_done!($x01, $x23, $x45, 7),
            }
        };
    }
    macro_rules! shuffle_x45 {
        ($x01:expr, $x23:expr) => {
            match (mask >> 4) & 0b11 {
                0b00 => shuffle_x67!($x01, $x23, 4),
                0b01 => shuffle_x67!($x01, $x23, 5),
                0b10 => shuffle_x67!($x01, $x23, 6),
                _ => shuffle_x67!($x01, $x23, 7),
            }
        };
    }
    macro_rules! shuffle_x23 {
        ($x01:expr) => {
            match (mask >> 2) & 0b11 {
                0b00 => shuffle_x45!($x01, 0),
                0b01 => shuffle_x45!($x01, 1),
                0b10 => shuffle_x45!($x01, 2),
                _ => shuffle_x45!($x01, 3),
            }
        };
    }
    match mask & 0b11 {
        0b00 => shuffle_x23!(0),
        0b01 => shuffle_x23!(1),
        0b10 => shuffle_x23!(2),
        _ => shuffle_x23!(3),
    }
}

/// Unpack and interleave single-precision (32-bit) floating-point elements
/// from the higher half of `a` and `b`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_unpackhi_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(unpckhps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_unpackhi_ps(a: __m128, b: __m128) -> __m128 {
    simd_shuffle4(a, b, [2, 6, 3, 7])
}

/// Unpack and interleave single-precision (32-bit) floating-point elements
/// from the lower half of `a` and `b`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_unpacklo_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(unpcklps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_unpacklo_ps(a: __m128, b: __m128) -> __m128 {
    simd_shuffle4(a, b, [0, 4, 1, 5])
}

/// Combine higher half of `a` and `b`. The highwe half of `b` occupies the
/// lower half of result.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_movehl_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movhlps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_movehl_ps(a: __m128, b: __m128) -> __m128 {
    // TODO; figure why this is a different instruction on Windows?
    simd_shuffle4(a, b, [6, 7, 2, 3])
}

/// Combine lower half of `a` and `b`. The lower half of `b` occupies the
/// higher half of result.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_movelh_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movlhps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_movelh_ps(a: __m128, b: __m128) -> __m128 {
    simd_shuffle4(a, b, [0, 1, 4, 5])
}

/// Return a mask of the most significant bit of each element in `a`.
///
/// The mask is stored in the 4 least significant bits of the return value.
/// All other bits are set to `0`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_movemask_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movmskps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_movemask_ps(a: __m128) -> i32 {
    movmskps(a)
}

/// Set the upper two single-precision floating-point values with 64 bits of
/// data loaded from the address `p`; the lower two values are passed through
/// from `a`.
///
/// This corresponds to the `MOVHPS` / `MOVHPD` / `VMOVHPD` instructions.
///
/// ```rust
/// # #![feature(stdsimd)]
/// # #![cfg_attr(not(dox), no_std)]
/// # #[cfg(not(dox))]
/// # extern crate std as real_std;
/// # #[cfg(not(dox))]
/// # #[macro_use]
/// # extern crate stdsimd as std;
/// #[cfg(target_arch = "x86")]
/// use std::arch::x86::*;
/// #[cfg(target_arch = "x86_64")]
/// use std::arch::x86_64::*;
///
/// #
/// # // The real main function
/// # fn main() {
/// #     if is_x86_feature_detected!("sse") {
/// #         #[target_feature(enable = "sse")]
/// #         unsafe fn worker() {
/// #
/// let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
/// let data: [f32; 4] = [5.0, 6.0, 7.0, 8.0];
/// let r = _mm_loadh_pi(a, data[..].as_ptr() as *const _);
/// // assert_eq!(r, _mm_setr_ps(1.0, 2.0, 5.0, 6.0));
/// #
/// #         }
/// #         unsafe { worker(); }
/// #     }
/// # }
/// ```
#[inline]
#[target_feature(enable = "sse")]
// TODO: generates MOVHPD if the CPU supports SSE2.
// #[cfg_attr(test, assert_instr(movhps))]
#[cfg_attr(all(test, target_arch = "x86_64"), assert_instr(movhpd))]
// 32-bit codegen does not generate `movhps` or `movhpd`, but instead
// `movsd` followed by `unpcklpd` (or `movss'/`unpcklps` if there's no SSE2).
#[cfg_attr(
    all(test, target_arch = "x86", target_feature = "sse2"),
    assert_instr(movlhps)
)]
#[cfg_attr(
    all(test, target_arch = "x86", not(target_feature = "sse2")),
    assert_instr(unpcklps)
)]
// TODO: This function is actually not limited to floats, but that's what
// what matches the C type most closely: (__m128, *const __m64) -> __m128
pub unsafe fn _mm_loadh_pi(a: __m128, p: *const __m64) -> __m128 {
    let q = p as *const f32x2;
    let b: f32x2 = *q;
    let bb = simd_shuffle4(b, b, [0, 1, 0, 1]);
    simd_shuffle4(a, bb, [0, 1, 4, 5])
}

/// Load two floats from `p` into the lower half of a `__m128`. The upper half
/// is copied from the upper half of `a`.
///
/// This corresponds to the `MOVLPS` / `MOVLDP` / `VMOVLDP` instructions.
///
/// ```rust
/// # #![feature(stdsimd)]
/// # #![cfg_attr(not(dox), no_std)]
/// # #[cfg(not(dox))]
/// # extern crate std as real_std;
/// # #[cfg(not(dox))]
/// # #[macro_use]
/// # extern crate stdsimd as std;
/// #[cfg(target_arch = "x86")]
/// use std::arch::x86::*;
/// #[cfg(target_arch = "x86_64")]
/// use std::arch::x86_64::*;
///
/// # // The real main function
/// # fn main() {
/// #     if is_x86_feature_detected!("sse") {
/// #         #[target_feature(enable = "sse")]
/// #         unsafe fn worker() {
/// #
/// let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
/// let data: [f32; 4] = [5.0, 6.0, 7.0, 8.0];
/// let r = _mm_loadh_pi(a, data[..].as_ptr() as *const _);
/// // assert_eq!(r, _mm_setr_ps(5.0, 6.0, 3.0, 4.0));
/// #
/// #         }
/// #         unsafe { worker(); }
/// #     }
/// # }
/// ```
#[inline]
#[target_feature(enable = "sse")]
// TODO: generates MOVLPD if the CPU supports SSE2.
// #[cfg_attr(test, assert_instr(movlps))]
#[cfg_attr(all(test, target_arch = "x86_64"), assert_instr(movlpd))]
// On 32-bit targets with SSE2, it just generates two `movsd`.
#[cfg_attr(
    all(test, target_arch = "x86", target_feature = "sse2"),
    assert_instr(movsd)
)]
// It should really generate "movlps", but oh well...
#[cfg_attr(
    all(test, target_arch = "x86", not(target_feature = "sse2")),
    assert_instr(movss)
)]
// TODO: Like _mm_loadh_pi, this also isn't limited to floats.
pub unsafe fn _mm_loadl_pi(a: __m128, p: *const __m64) -> __m128 {
    let q = p as *const f32x2;
    let b: f32x2 = *q;
    let bb = simd_shuffle4(b, b, [0, 1, 0, 1]);
    simd_shuffle4(a, bb, [4, 5, 2, 3])
}

/// Construct a `__m128` with the lowest element read from `p` and the other
/// elements set to zero.
///
/// This corresponds to instructions `VMOVSS` / `MOVSS`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_load_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_load_ss(p: *const f32) -> __m128 {
    __m128(*p, 0.0, 0.0, 0.0)
}

/// Construct a `__m128` by duplicating the value read from `p` into all
/// elements.
///
/// This corresponds to instructions `VMOVSS` / `MOVSS` followed by some
/// shuffling.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_load1_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_load1_ps(p: *const f32) -> __m128 {
    let a = *p;
    __m128(a, a, a, a)
}

/// Alias for [`_mm_load1_ps`](fn._mm_load1_ps.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_load_ps1)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_load_ps1(p: *const f32) -> __m128 {
    _mm_load1_ps(p)
}

/// Load four `f32` values from *aligned* memory into a `__m128`. If the
/// pointer is not aligned to a 128-bit boundary (16 bytes) a general
/// protection fault will be triggered (fatal program crash).
///
/// Use [`_mm_loadu_ps`](fn._mm_loadu_ps.html) for potentially unaligned
/// memory.
///
/// This corresponds to instructions `VMOVAPS` / `MOVAPS`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_load_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movaps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_load_ps(p: *const f32) -> __m128 {
    *(p as *const __m128)
}

/// Load four `f32` values from memory into a `__m128`. There are no
/// restrictions
/// on memory alignment. For aligned memory
/// [`_mm_load_ps`](fn._mm_load_ps.html)
/// may be faster.
///
/// This corresponds to instructions `VMOVUPS` / `MOVUPS`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_loadu_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movups))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_loadu_ps(p: *const f32) -> __m128 {
    // Note: Using `*p` would require `f32` alignment, but `movups` has no
    // alignment restrictions.
    let mut dst = _mm_undefined_ps();
    ptr::copy_nonoverlapping(
        p as *const u8,
        &mut dst as *mut __m128 as *mut u8,
        mem::size_of::<__m128>(),
    );
    dst
}

/// Load four `f32` values from aligned memory into a `__m128` in reverse
/// order.
///
/// If the pointer is not aligned to a 128-bit boundary (16 bytes) a general
/// protection fault will be triggered (fatal program crash).
///
/// Functionally equivalent to the following code sequence (assuming `p`
/// satisfies the alignment restrictions):
///
/// ```text
/// let a0 = *p;
/// let a1 = *p.offset(1);
/// let a2 = *p.offset(2);
/// let a3 = *p.offset(3);
/// __m128::new(a3, a2, a1, a0)
/// ```
///
/// This corresponds to instructions `VMOVAPS` / `MOVAPS` followed by some
/// shuffling.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_loadr_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movaps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_loadr_ps(p: *const f32) -> __m128 {
    let a = _mm_load_ps(p);
    simd_shuffle4(a, a, [3, 2, 1, 0])
}

/// Store the upper half of `a` (64 bits) into memory.
///
/// This intrinsic corresponds to the `MOVHPS` instruction. The compiler may
/// choose to generate an equivalent sequence of other instructions.
#[inline]
#[target_feature(enable = "sse")]
// On i686 and up LLVM actually generates MOVHPD instead of MOVHPS, that's
// fine.
// On i586 (no SSE2) it just generates plain MOV instructions.
#[cfg_attr(
    all(test, any(target_arch = "x86_64", target_feature = "sse2")),
    assert_instr(movhpd)
)]
pub unsafe fn _mm_storeh_pi(p: *mut __m64, a: __m128) {
    #[cfg(target_arch = "x86")]
    {
        // If this is a `f64x2` then on i586, LLVM generates fldl & fstpl which
        // is just silly
        let a64: u64x2 = mem::transmute(a);
        let a_hi = a64.extract(1);
        *(p as *mut u64) = a_hi;
    }
    #[cfg(target_arch = "x86_64")]
    {
        // If this is a `u64x2` LLVM generates a pshufd + movq, but we really
        // want a a MOVHPD or MOVHPS here.
        let a64: f64x2 = mem::transmute(a);
        let a_hi = a64.extract(1);
        *p = mem::transmute(a_hi);
    }
}

/// Store the lower half of `a` (64 bits) into memory.
///
/// This intrinsic corresponds to the `MOVQ` instruction. The compiler may
/// choose to generate an equivalent sequence of other instructions.
#[inline]
#[target_feature(enable = "sse")]
// On i586 the codegen just generates plane MOVs. No need to test for that.
#[cfg_attr(
    all(test, any(target_arch = "x86_64", target_feature = "sse2")),
    assert_instr(movlps)
)]
pub unsafe fn _mm_storel_pi(p: *mut __m64, a: __m128) {
    #[cfg(target_arch = "x86")]
    {
        // Same as for _mm_storeh_pi: i586 code gen would use floating point
        // stack.
        let a64: u64x2 = mem::transmute(a);
        let a_hi = a64.extract(0);
        *(p as *mut u64) = a_hi;
    }
    #[cfg(target_arch = "x86_64")]
    {
        let a64: f64x2 = mem::transmute(a);
        let a_hi = a64.extract(0);
        *p = mem::transmute(a_hi);
    }
}

/// Store the lowest 32 bit float of `a` into memory.
///
/// This intrinsic corresponds to the `MOVSS` instruction.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_store_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_store_ss(p: *mut f32, a: __m128) {
    *p = simd_extract(a, 0);
}

/// Store the lowest 32 bit float of `a` repeated four times into *aligned*
/// memory.
///
/// If the pointer is not aligned to a 128-bit boundary (16 bytes) a general
/// protection fault will be triggered (fatal program crash).
///
/// Functionally equivalent to the following code sequence (assuming `p`
/// satisfies the alignment restrictions):
///
/// ```text
/// let x = a.extract(0);
/// *p = x;
/// *p.offset(1) = x;
/// *p.offset(2) = x;
/// *p.offset(3) = x;
/// ```
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_store1_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movaps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_store1_ps(p: *mut f32, a: __m128) {
    let b: __m128 = simd_shuffle4(a, a, [0, 0, 0, 0]);
    *(p as *mut __m128) = b;
}

/// Alias for [`_mm_store1_ps`](fn._mm_store1_ps.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_store_ps1)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movaps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_store_ps1(p: *mut f32, a: __m128) {
    _mm_store1_ps(p, a);
}

/// Store four 32-bit floats into *aligned* memory.
///
/// If the pointer is not aligned to a 128-bit boundary (16 bytes) a general
/// protection fault will be triggered (fatal program crash).
///
/// Use [`_mm_storeu_ps`](fn._mm_storeu_ps.html) for potentially unaligned
/// memory.
///
/// This corresponds to instructions `VMOVAPS` / `MOVAPS`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_store_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movaps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_store_ps(p: *mut f32, a: __m128) {
    *(p as *mut __m128) = a;
}

/// Store four 32-bit floats into memory. There are no restrictions on memory
/// alignment. For aligned memory [`_mm_store_ps`](fn._mm_store_ps.html) may be
/// faster.
///
/// This corresponds to instructions `VMOVUPS` / `MOVUPS`.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_storeu_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movups))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_storeu_ps(p: *mut f32, a: __m128) {
    ptr::copy_nonoverlapping(
        &a as *const __m128 as *const u8,
        p as *mut u8,
        mem::size_of::<__m128>(),
    );
}

/// Store four 32-bit floats into *aligned* memory in reverse order.
///
/// If the pointer is not aligned to a 128-bit boundary (16 bytes) a general
/// protection fault will be triggered (fatal program crash).
///
/// Functionally equivalent to the following code sequence (assuming `p`
/// satisfies the alignment restrictions):
///
/// ```text
/// *p = a.extract(3);
/// *p.offset(1) = a.extract(2);
/// *p.offset(2) = a.extract(1);
/// *p.offset(3) = a.extract(0);
/// ```
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_storer_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movaps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_storer_ps(p: *mut f32, a: __m128) {
    let b: __m128 = simd_shuffle4(a, a, [3, 2, 1, 0]);
    *(p as *mut __m128) = b;
}

/// Return a `__m128` with the first component from `b` and the remaining
/// components from `a`.
///
/// In other words for any `a` and `b`:
/// ```text
/// _mm_move_ss(a, b) == a.replace(0, b.extract(0))
/// ```
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_move_ss)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movss))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_move_ss(a: __m128, b: __m128) -> __m128 {
    simd_shuffle4(a, b, [4, 1, 2, 3])
}

/// Perform a serializing operation on all store-to-memory instructions that
/// were issued prior to this instruction.
///
/// Guarantees that every store instruction that precedes, in program order, is
/// globally visible before any store instruction which follows the fence in
/// program order.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_sfence)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(sfence))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_sfence() {
    sfence()
}

/// Get the unsigned 32-bit value of the MXCSR control and status register.
///
/// For more info see [`_mm_setcsr`](fn._mm_setcsr.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_getcsr)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(stmxcsr))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_getcsr() -> u32 {
    let mut result = 0_i32;
    stmxcsr((&mut result) as *mut _ as *mut i8);
    result as u32
}

/// Set the MXCSR register with the 32-bit unsigned integer value.
///
/// This register constrols how SIMD instructions handle floating point
/// operations. Modifying this register only affects the current thread.
///
/// It contains several groups of flags:
///
/// * *Exception flags* report which exceptions occurred since last they were
/// reset.
///
/// * *Masking flags* can be used to mask (ignore) certain exceptions. By
/// default
/// these flags are all set to 1, so all exceptions are masked. When an
/// an exception is masked, the processor simply sets the exception flag and
/// continues the operation. If the exception is unmasked, the flag is also set
/// but additionally an exception handler is invoked.
///
/// * *Rounding mode flags* control the rounding mode of floating point
/// instructions.
///
/// * The *denormals-are-zero mode flag* turns all numbers which would be
/// denormalized (exponent bits are all zeros) into zeros.
///
/// ## Exception Flags
///
/// * `_MM_EXCEPT_INVALID`: An invalid operation was performed (e.g., dividing
///   Infinity by Infinity).
///
/// * `_MM_EXCEPT_DENORM`: An operation attempted to operate on a denormalized
///   number. Mainly this can cause loss of precision.
///
/// * `_MM_EXCEPT_DIV_ZERO`: Division by zero occured.
///
/// * `_MM_EXCEPT_OVERFLOW`: A numeric overflow exception occured, i.e., a
/// result was too large to be represented (e.g., an `f32` with absolute
/// value
///   greater than `2^128`).
///
/// * `_MM_EXCEPT_UNDERFLOW`: A numeric underflow exception occured, i.e., a
/// result was too small to be represented in a normalized way (e.g., an
/// `f32`
///   with absulte value smaller than `2^-126`.)
///
/// * `_MM_EXCEPT_INEXACT`: An inexact-result exception occured (a.k.a.
///   precision exception). This means some precision was lost due to rounding.
///   For example, the fraction `1/3` cannot be represented accurately in a
///   32 or 64 bit float and computing it would cause this exception to be
///   raised. Precision exceptions are very common, so they are usually masked.
///
/// Exception flags can be read and set using the convenience functions
/// `_MM_GET_EXCEPTION_STATE` and `_MM_SET_EXCEPTION_STATE`. For example, to
/// check if an operation caused some overflow:
///
/// ```rust,ignore
/// _MM_SET_EXCEPTION_STATE(0); // clear all exception flags
///                             // perform calculations
/// if _MM_GET_EXCEPTION_STATE() & _MM_EXCEPT_OVERFLOW != 0 {
///     // handle overflow
/// }
/// ```
///
/// ## Masking Flags
///
/// There is one masking flag for each exception flag: `_MM_MASK_INVALID`,
/// `_MM_MASK_DENORM`, `_MM_MASK_DIV_ZERO`, `_MM_MASK_OVERFLOW`,
/// `_MM_MASK_UNDERFLOW`, `_MM_MASK_INEXACT`.
///
/// A single masking bit can be set via
///
/// ```rust,ignore
/// _MM_SET_EXCEPTION_MASK(_MM_MASK_UNDERFLOW);
/// ```
///
/// However, since mask bits are by default all set to 1, it is more common to
/// want to *disable* certain bits. For example, to unmask the underflow
/// exception, use:
///
/// ```rust,ignore
/// _mm_setcsr(_mm_getcsr() & !_MM_MASK_UNDERFLOW); // unmask underflow
/// exception
/// ```
///
/// Warning: an unmasked exception will cause an exception handler to be
/// called.
/// The standard handler will simply terminate the process. So, in this case
/// any underflow exception would terminate the current process with something
/// like `signal: 8, SIGFPE: erroneous arithmetic operation`.
///
/// ## Rounding Mode
///
/// The rounding mode is describe using two bits. It can be read and set using
/// the convenience wrappers `_MM_GET_ROUNDING_MODE()` and
/// `_MM_SET_ROUNDING_MODE(mode)`.
///
/// The rounding modes are:
///
/// * `_MM_ROUND_NEAREST`: (default) Round to closest to the infinite precision
///   value. If two values are equally close, round to even (i.e., least
///   significant bit will be zero).
///
/// * `_MM_ROUND_DOWN`: Round toward negative Infinity.
///
/// * `_MM_ROUND_UP`: Round toward positive Infinity.
///
/// * `_MM_ROUND_TOWARD_ZERO`: Round towards zero (truncate).
///
/// Example:
///
/// ```rust,ignore
/// _MM_SET_ROUNDING_MODE(_MM_ROUND_DOWN)
/// ```
///
/// ## Denormals-are-zero/Flush-to-zero Mode
///
/// If this bit is set, values that would be denormalized will be set to zero
/// instead. This is turned off by default.
///
/// You can read and enable/disable this mode via the helper functions
/// `_MM_GET_FLUSH_ZERO_MODE()` and `_MM_SET_FLUSH_ZERO_MODE()`:
///
/// ```rust,ignore
/// _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF); // turn off (default)
/// _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON); // turn on
/// ```
///
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_setcsr)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(ldmxcsr))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_setcsr(val: u32) {
    ldmxcsr(&val as *const _ as *const i8);
}

/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_EXCEPT_INVALID: u32 = 0x0001;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_EXCEPT_DENORM: u32 = 0x0002;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_EXCEPT_DIV_ZERO: u32 = 0x0004;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_EXCEPT_OVERFLOW: u32 = 0x0008;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_EXCEPT_UNDERFLOW: u32 = 0x0010;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_EXCEPT_INEXACT: u32 = 0x0020;
/// See [`_MM_GET_EXCEPTION_STATE`](fn._MM_GET_EXCEPTION_STATE.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_EXCEPT_MASK: u32 = 0x003f;

/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_MASK_INVALID: u32 = 0x0080;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_MASK_DENORM: u32 = 0x0100;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_MASK_DIV_ZERO: u32 = 0x0200;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_MASK_OVERFLOW: u32 = 0x0400;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_MASK_UNDERFLOW: u32 = 0x0800;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_MASK_INEXACT: u32 = 0x1000;
/// See [`_MM_GET_EXCEPTION_MASK`](fn._MM_GET_EXCEPTION_MASK.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_MASK_MASK: u32 = 0x1f80;

/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_ROUND_NEAREST: u32 = 0x0000;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_ROUND_DOWN: u32 = 0x2000;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_ROUND_UP: u32 = 0x4000;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_ROUND_TOWARD_ZERO: u32 = 0x6000;

/// See [`_MM_GET_ROUNDING_MODE`](fn._MM_GET_ROUNDING_MODE.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_ROUND_MASK: u32 = 0x6000;

/// See [`_MM_GET_FLUSH_ZERO_MODE`](fn._MM_GET_FLUSH_ZERO_MODE.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_FLUSH_ZERO_MASK: u32 = 0x8000;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_FLUSH_ZERO_ON: u32 = 0x8000;
/// See [`_mm_setcsr`](fn._mm_setcsr.html)
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_FLUSH_ZERO_OFF: u32 = 0x0000;

/// See [`_mm_setcsr`](fn._mm_setcsr.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_MM_GET_EXCEPTION_MASK)
#[inline]
#[allow(non_snake_case)]
#[target_feature(enable = "sse")]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _MM_GET_EXCEPTION_MASK() -> u32 {
    _mm_getcsr() & _MM_MASK_MASK
}

/// See [`_mm_setcsr`](fn._mm_setcsr.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_MM_GET_EXCEPTION_STATE)
#[inline]
#[allow(non_snake_case)]
#[target_feature(enable = "sse")]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _MM_GET_EXCEPTION_STATE() -> u32 {
    _mm_getcsr() & _MM_EXCEPT_MASK
}

/// See [`_mm_setcsr`](fn._mm_setcsr.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_MM_GET_FLUSH_ZERO_MODE)
#[inline]
#[allow(non_snake_case)]
#[target_feature(enable = "sse")]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _MM_GET_FLUSH_ZERO_MODE() -> u32 {
    _mm_getcsr() & _MM_FLUSH_ZERO_MASK
}

/// See [`_mm_setcsr`](fn._mm_setcsr.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_MM_GET_ROUNDING_MODE)
#[inline]
#[allow(non_snake_case)]
#[target_feature(enable = "sse")]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _MM_GET_ROUNDING_MODE() -> u32 {
    _mm_getcsr() & _MM_ROUND_MASK
}

/// See [`_mm_setcsr`](fn._mm_setcsr.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_MM_SET_EXCEPTION_MASK)
#[inline]
#[allow(non_snake_case)]
#[target_feature(enable = "sse")]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _MM_SET_EXCEPTION_MASK(x: u32) {
    _mm_setcsr((_mm_getcsr() & !_MM_MASK_MASK) | x)
}

/// See [`_mm_setcsr`](fn._mm_setcsr.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_MM_SET_EXCEPTION_STATE)
#[inline]
#[allow(non_snake_case)]
#[target_feature(enable = "sse")]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _MM_SET_EXCEPTION_STATE(x: u32) {
    _mm_setcsr((_mm_getcsr() & !_MM_EXCEPT_MASK) | x)
}

/// See [`_mm_setcsr`](fn._mm_setcsr.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_MM_SET_FLUSH_ZERO_MODE)
#[inline]
#[allow(non_snake_case)]
#[target_feature(enable = "sse")]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _MM_SET_FLUSH_ZERO_MODE(x: u32) {
    let val = (_mm_getcsr() & !_MM_FLUSH_ZERO_MASK) | x;
    // println!("setting csr={:x}", val);
    _mm_setcsr(val)
}

/// See [`_mm_setcsr`](fn._mm_setcsr.html)
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_MM_SET_ROUNDING_MODE)
#[inline]
#[allow(non_snake_case)]
#[target_feature(enable = "sse")]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _MM_SET_ROUNDING_MODE(x: u32) {
    _mm_setcsr((_mm_getcsr() & !_MM_ROUND_MASK) | x)
}

/// See [`_mm_prefetch`](fn._mm_prefetch.html).
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_HINT_T0: i32 = 3;

/// See [`_mm_prefetch`](fn._mm_prefetch.html).
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_HINT_T1: i32 = 2;

/// See [`_mm_prefetch`](fn._mm_prefetch.html).
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_HINT_T2: i32 = 1;

/// See [`_mm_prefetch`](fn._mm_prefetch.html).
#[stable(feature = "simd_x86", since = "1.27.0")]
pub const _MM_HINT_NTA: i32 = 0;

/// Fetch the cache line that contains address `p` using the given `strategy`.
///
/// The `strategy` must be one of:
///
/// * [`_MM_HINT_T0`](constant._MM_HINT_T0.html): Fetch into all levels of the
///   cache hierachy.
///
/// * [`_MM_HINT_T1`](constant._MM_HINT_T1.html): Fetch into L2 and higher.
///
/// * [`_MM_HINT_T2`](constant._MM_HINT_T2.html): Fetch into L3 and higher or
/// an   implementation-specific choice (e.g., L2 if there is no L3).
///
/// * [`_MM_HINT_NTA`](constant._MM_HINT_NTA.html): Fetch data using the
///   non-temporal access (NTA) hint. It may be a place closer than main memory
///   but outside of the cache hierarchy. This is used to reduce access latency
///   without polluting the cache.
///
/// The actual implementation depends on the particular CPU. This instruction
/// is considered a hint, so the CPU is also free to simply ignore the request.
///
/// The amount of prefetched data depends on the cache line size of the
/// specific CPU, but it will be at least 32 bytes.
///
/// Common caveats:
///
/// * Most modern CPUs already automatically prefetch data based on predicted
///   access patterns.
///
/// * Data is usually not fetched if this would cause a TLB miss or a page
///   fault.
///
/// * Too much prefetching can cause unnecessary cache evictions.
///
/// * Prefetching may also fail if there are not enough memory-subsystem
///   resources (e.g., request buffers).
///
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_prefetch)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(prefetcht0, strategy = _MM_HINT_T0))]
#[cfg_attr(test, assert_instr(prefetcht1, strategy = _MM_HINT_T1))]
#[cfg_attr(test, assert_instr(prefetcht2, strategy = _MM_HINT_T2))]
#[cfg_attr(test, assert_instr(prefetchnta, strategy = _MM_HINT_NTA))]
#[rustc_args_required_const(1)]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_prefetch(p: *const i8, strategy: i32) {
    // The `strategy` must be a compile-time constant, so we use a short form
    // of `constify_imm8!` for now.
    // We use the `llvm.prefetch` instrinsic with `rw` = 0 (read), and
    // `cache type` = 1 (data cache). `locality` is based on our `strategy`.
    macro_rules! pref {
        ($imm8:expr) => {
            match $imm8 {
                0 => prefetch(p, 0, 0, 1),
                1 => prefetch(p, 0, 1, 1),
                2 => prefetch(p, 0, 2, 1),
                _ => prefetch(p, 0, 3, 1),
            }
        };
    }
    pref!(strategy)
}

/// Return vector of type __m128 with undefined elements.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_undefined_ps)
#[inline]
#[target_feature(enable = "sse")]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_undefined_ps() -> __m128 {
    __m128(
        mem::uninitialized(),
        mem::uninitialized(),
        mem::uninitialized(),
        mem::uninitialized(),
    )
}

/// Transpose the 4x4 matrix formed by 4 rows of __m128 in place.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_MM_TRANSPOSE4_PS)
#[inline]
#[allow(non_snake_case)]
#[target_feature(enable = "sse")]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _MM_TRANSPOSE4_PS(
    row0: &mut __m128, row1: &mut __m128, row2: &mut __m128, row3: &mut __m128,
) {
    let tmp0 = _mm_unpacklo_ps(*row0, *row1);
    let tmp2 = _mm_unpacklo_ps(*row2, *row3);
    let tmp1 = _mm_unpackhi_ps(*row0, *row1);
    let tmp3 = _mm_unpackhi_ps(*row2, *row3);

    *row0 = _mm_movelh_ps(tmp0, tmp2);
    *row1 = _mm_movehl_ps(tmp2, tmp0);
    *row2 = _mm_movelh_ps(tmp1, tmp3);
    *row3 = _mm_movehl_ps(tmp3, tmp1);
}

#[allow(improper_ctypes)]
extern "C" {
    #[link_name = "llvm.x86.sse.add.ss"]
    fn addss(a: __m128, b: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.sub.ss"]
    fn subss(a: __m128, b: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.mul.ss"]
    fn mulss(a: __m128, b: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.div.ss"]
    fn divss(a: __m128, b: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.sqrt.ss"]
    fn sqrtss(a: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.sqrt.ps"]
    fn sqrtps(a: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.rcp.ss"]
    fn rcpss(a: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.rcp.ps"]
    fn rcpps(a: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.rsqrt.ss"]
    fn rsqrtss(a: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.rsqrt.ps"]
    fn rsqrtps(a: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.min.ss"]
    fn minss(a: __m128, b: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.min.ps"]
    fn minps(a: __m128, b: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.max.ss"]
    fn maxss(a: __m128, b: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.max.ps"]
    fn maxps(a: __m128, b: __m128) -> __m128;
    #[link_name = "llvm.x86.sse.movmsk.ps"]
    fn movmskps(a: __m128) -> i32;
    #[link_name = "llvm.x86.sse.cmp.ps"]
    fn cmpps(a: __m128, b: __m128, imm8: i8) -> __m128;
    #[link_name = "llvm.x86.sse.comieq.ss"]
    fn comieq_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.comilt.ss"]
    fn comilt_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.comile.ss"]
    fn comile_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.comigt.ss"]
    fn comigt_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.comige.ss"]
    fn comige_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.comineq.ss"]
    fn comineq_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.ucomieq.ss"]
    fn ucomieq_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.ucomilt.ss"]
    fn ucomilt_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.ucomile.ss"]
    fn ucomile_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.ucomigt.ss"]
    fn ucomigt_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.ucomige.ss"]
    fn ucomige_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.ucomineq.ss"]
    fn ucomineq_ss(a: __m128, b: __m128) -> i32;
    #[link_name = "llvm.x86.sse.cvtss2si"]
    fn cvtss2si(a: __m128) -> i32;
    #[link_name = "llvm.x86.sse.cvttss2si"]
    fn cvttss2si(a: __m128) -> i32;
    #[link_name = "llvm.x86.sse.cvtsi2ss"]
    fn cvtsi2ss(a: __m128, b: i32) -> __m128;
    #[link_name = "llvm.x86.sse.sfence"]
    fn sfence();
    #[link_name = "llvm.x86.sse.stmxcsr"]
    fn stmxcsr(p: *mut i8);
    #[link_name = "llvm.x86.sse.ldmxcsr"]
    fn ldmxcsr(p: *const i8);
    #[link_name = "llvm.prefetch"]
    fn prefetch(p: *const i8, rw: i32, loc: i32, ty: i32);
    #[link_name = "llvm.x86.sse.cmp.ss"]
    fn cmpss(a: __m128, b: __m128, imm8: i8) -> __m128;
    #[link_name = "llvm.x86.mmx.movnt.dq"]
    fn movntdq(a: *mut __m64, b: __m64);
    #[link_name = "llvm.x86.sse.cvtpi2ps"]
    fn cvtpi2ps(a: __m128, b: __m64) -> __m128;
    #[link_name = "llvm.x86.mmx.maskmovq"]
    fn maskmovq(a: __m64, mask: __m64, mem_addr: *mut i8);
    #[link_name = "llvm.x86.mmx.pextr.w"]
    fn pextrw(a: __m64, imm8: i32) -> i32;
    #[link_name = "llvm.x86.mmx.pinsr.w"]
    fn pinsrw(a: __m64, d: i32, imm8: i32) -> __m64;
    #[link_name = "llvm.x86.mmx.pmovmskb"]
    fn pmovmskb(a: __m64) -> i32;
    #[link_name = "llvm.x86.sse.pshuf.w"]
    fn pshufw(a: __m64, imm8: i8) -> __m64;
    #[link_name = "llvm.x86.mmx.pmaxs.w"]
    fn pmaxsw(a: __m64, b: __m64) -> __m64;
    #[link_name = "llvm.x86.mmx.pmaxu.b"]
    fn pmaxub(a: __m64, b: __m64) -> __m64;
    #[link_name = "llvm.x86.mmx.pmins.w"]
    fn pminsw(a: __m64, b: __m64) -> __m64;
    #[link_name = "llvm.x86.mmx.pminu.b"]
    fn pminub(a: __m64, b: __m64) -> __m64;
    #[link_name = "llvm.x86.mmx.pmulhu.w"]
    fn pmulhuw(a: __m64, b: __m64) -> __m64;
    #[link_name = "llvm.x86.mmx.pmull.w"]
    fn pmullw(a: __m64, b: __m64) -> __m64;
    #[link_name = "llvm.x86.mmx.pavg.b"]
    fn pavgb(a: __m64, b: __m64) -> __m64;
    #[link_name = "llvm.x86.mmx.pavg.w"]
    fn pavgw(a: __m64, b: __m64) -> __m64;
    #[link_name = "llvm.x86.mmx.psad.bw"]
    fn psadbw(a: __m64, b: __m64) -> __m64;
    #[link_name = "llvm.x86.sse.cvtps2pi"]
    fn cvtps2pi(a: __m128) -> __m64;
    #[link_name = "llvm.x86.sse.cvttps2pi"]
    fn cvttps2pi(a: __m128) -> __m64;
}

/// Stores `a` into the memory at `mem_addr` using a non-temporal memory hint.
///
/// `mem_addr` must be aligned on a 16-byte boundary or a general-protection
/// exception _may_ be generated.
///
/// [Intel's documentation](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_stream_ps)
#[inline]
#[target_feature(enable = "sse")]
#[cfg_attr(test, assert_instr(movntps))]
#[stable(feature = "simd_x86", since = "1.27.0")]
pub unsafe fn _mm_stream_ps(mem_addr: *mut f32, a: __m128) {
    intrinsics::nontemporal_store(mem::transmute(mem_addr), a);
}

/// Store 64-bits of integer data from a into memory using a non-temporal
/// memory hint.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(movntq))]
pub unsafe fn _mm_stream_pi(mem_addr: *mut __m64, a: __m64) {
    movntdq(mem_addr, a)
}

/// Compares the packed 16-bit signed integers of `a` and `b` writing the
/// greatest value into the result.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pmaxsw))]
pub unsafe fn _mm_max_pi16(a: __m64, b: __m64) -> __m64 {
    pmaxsw(a, b)
}

/// Compares the packed 16-bit signed integers of `a` and `b` writing the
/// greatest value into the result.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pmaxsw))]
pub unsafe fn _m_pmaxsw(a: __m64, b: __m64) -> __m64 {
    _mm_max_pi16(a, b)
}

/// Compares the packed 8-bit signed integers of `a` and `b` writing the
/// greatest value into the result.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pmaxub))]
pub unsafe fn _mm_max_pu8(a: __m64, b: __m64) -> __m64 {
    pmaxub(a, b)
}

/// Compares the packed 8-bit signed integers of `a` and `b` writing the
/// greatest value into the result.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pmaxub))]
pub unsafe fn _m_pmaxub(a: __m64, b: __m64) -> __m64 {
    _mm_max_pu8(a, b)
}

/// Compares the packed 16-bit signed integers of `a` and `b` writing the
/// smallest value into the result.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pminsw))]
pub unsafe fn _mm_min_pi16(a: __m64, b: __m64) -> __m64 {
    pminsw(a, b)
}

/// Compares the packed 16-bit signed integers of `a` and `b` writing the
/// smallest value into the result.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pminsw))]
pub unsafe fn _m_pminsw(a: __m64, b: __m64) -> __m64 {
    _mm_min_pi16(a, b)
}

/// Compares the packed 8-bit signed integers of `a` and `b` writing the
/// smallest value into the result.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pminub))]
pub unsafe fn _mm_min_pu8(a: __m64, b: __m64) -> __m64 {
    pminub(a, b)
}

/// Compares the packed 8-bit signed integers of `a` and `b` writing the
/// smallest value into the result.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pminub))]
pub unsafe fn _m_pminub(a: __m64, b: __m64) -> __m64 {
    _mm_min_pu8(a, b)
}

/// Multiplies packed 16-bit unsigned integer values and writes the
/// high-order 16 bits of each 32-bit product to the corresponding bits in
/// the destination.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pmulhuw))]
pub unsafe fn _mm_mulhi_pu16(a: __m64, b: __m64) -> __m64 {
    pmulhuw(a, b)
}

/// Multiplies packed 16-bit integer values and writes the
/// low-order 16 bits of each 32-bit product to the corresponding bits in
/// the destination.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pmullw))]
pub unsafe fn _mm_mullo_pi16(a: __m64, b: __m64) -> __m64 {
    pmullw(a, b)
}

/// Multiplies packed 16-bit unsigned integer values and writes the
/// high-order 16 bits of each 32-bit product to the corresponding bits in
/// the destination.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pmulhuw))]
pub unsafe fn _m_pmulhuw(a: __m64, b: __m64) -> __m64 {
    _mm_mulhi_pu16(a, b)
}

/// Computes the rounded averages of the packed unsigned 8-bit integer
/// values and writes the averages to the corresponding bits in the
/// destination.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pavgb))]
pub unsafe fn _mm_avg_pu8(a: __m64, b: __m64) -> __m64 {
    pavgb(a, b)
}

/// Computes the rounded averages of the packed unsigned 8-bit integer
/// values and writes the averages to the corresponding bits in the
/// destination.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pavgb))]
pub unsafe fn _m_pavgb(a: __m64, b: __m64) -> __m64 {
    _mm_avg_pu8(a, b)
}

/// Computes the rounded averages of the packed unsigned 16-bit integer
/// values and writes the averages to the corresponding bits in the
/// destination.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pavgw))]
pub unsafe fn _mm_avg_pu16(a: __m64, b: __m64) -> __m64 {
    pavgw(a, b)
}

/// Computes the rounded averages of the packed unsigned 16-bit integer
/// values and writes the averages to the corresponding bits in the
/// destination.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pavgw))]
pub unsafe fn _m_pavgw(a: __m64, b: __m64) -> __m64 {
    _mm_avg_pu16(a, b)
}

/// Subtracts the corresponding 8-bit unsigned integer values of the two
/// 64-bit vector operands and computes the absolute value for each of the
/// difference. Then sum of the 8 absolute differences is written to the
/// bits `[15:0]` of the destination; the remaining bits `[63:16]` are cleared.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(psadbw))]
pub unsafe fn _mm_sad_pu8(a: __m64, b: __m64) -> __m64 {
    psadbw(a, b)
}

/// Subtracts the corresponding 8-bit unsigned integer values of the two
/// 64-bit vector operands and computes the absolute value for each of the
/// difference. Then sum of the 8 absolute differences is written to the
/// bits `[15:0]` of the destination; the remaining bits `[63:16]` are cleared.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(psadbw))]
pub unsafe fn _m_psadbw(a: __m64, b: __m64) -> __m64 {
    _mm_sad_pu8(a, b)
}

/// Converts two elements of a 64-bit vector of `[2 x i32]` into two
/// floating point values and writes them to the lower 64-bits of the
/// destination. The remaining higher order elements of the destination are
/// copied from the corresponding elements in the first operand.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvtpi2ps))]
pub unsafe fn _mm_cvtpi32_ps(a: __m128, b: __m64) -> __m128 {
    cvtpi2ps(a, b)
}

/// Converts two elements of a 64-bit vector of `[2 x i32]` into two
/// floating point values and writes them to the lower 64-bits of the
/// destination. The remaining higher order elements of the destination are
/// copied from the corresponding elements in the first operand.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvtpi2ps))]
pub unsafe fn _mm_cvt_pi2ps(a: __m128, b: __m64) -> __m128 {
    _mm_cvtpi32_ps(a, b)
}

/// Converts the lower 4 8-bit values of `a` into a 128-bit vector of 4 `f32`s.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvtpi2ps))]
pub unsafe fn _mm_cvtpi8_ps(a: __m64) -> __m128 {
    let b = _mm_setzero_si64();
    let b = _mm_cmpgt_pi8(b, a);
    let b = _mm_unpacklo_pi8(a, b);
    _mm_cvtpi16_ps(b)
}

/// Converts the lower 4 8-bit values of `a` into a 128-bit vector of 4 `f32`s.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvtpi2ps))]
pub unsafe fn _mm_cvtpu8_ps(a: __m64) -> __m128 {
    let b = _mm_setzero_si64();
    let b = _mm_unpacklo_pi8(a, b);
    _mm_cvtpi16_ps(b)
}

/// Converts a 64-bit vector of `i16`s into a 128-bit vector of 4 `f32`s.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvtpi2ps))]
pub unsafe fn _mm_cvtpi16_ps(a: __m64) -> __m128 {
    let b = _mm_setzero_si64();
    let b = _mm_cmpgt_pi16(b, a);
    let c = _mm_unpackhi_pi16(a, b);
    let r = _mm_setzero_ps();
    let r = cvtpi2ps(r, c);
    let r = _mm_movelh_ps(r, r);
    let c = _mm_unpacklo_pi16(a, b);
    cvtpi2ps(r, c)
}

/// Converts a 64-bit vector of `i16`s into a 128-bit vector of 4 `f32`s.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvtpi2ps))]
pub unsafe fn _mm_cvtpu16_ps(a: __m64) -> __m128 {
    let b = _mm_setzero_si64();
    let c = _mm_unpackhi_pi16(a, b);
    let r = _mm_setzero_ps();
    let r = cvtpi2ps(r, c);
    let r = _mm_movelh_ps(r, r);
    let c = _mm_unpacklo_pi16(a, b);
    cvtpi2ps(r, c)
}

/// Converts the two 32-bit signed integer values from each 64-bit vector
/// operand of `[2 x i32]` into a 128-bit vector of `[4 x float]`.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvtpi2ps))]
pub unsafe fn _mm_cvtpi32x2_ps(a: __m64, b: __m64) -> __m128 {
    let c = _mm_setzero_ps();
    let c = _mm_cvtpi32_ps(c, b);
    let c = _mm_movelh_ps(c, c);
    _mm_cvtpi32_ps(c, a)
}

/// Conditionally copies the values from each 8-bit element in the first
/// 64-bit integer vector operand to the specified memory location, as
/// specified by the most significant bit in the corresponding element in the
/// second 64-bit integer vector operand.
///
/// To minimize caching, the data is flagged as non-temporal
/// (unlikely to be used again soon).
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(maskmovq))]
pub unsafe fn _mm_maskmove_si64(a: __m64, mask: __m64, mem_addr: *mut i8) {
    maskmovq(a, mask, mem_addr)
}

/// Conditionally copies the values from each 8-bit element in the first
/// 64-bit integer vector operand to the specified memory location, as
/// specified by the most significant bit in the corresponding element in the
/// second 64-bit integer vector operand.
///
/// To minimize caching, the data is flagged as non-temporal
/// (unlikely to be used again soon).
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(maskmovq))]
pub unsafe fn _m_maskmovq(a: __m64, mask: __m64, mem_addr: *mut i8) {
    _mm_maskmove_si64(a, mask, mem_addr)
}

/// Extracts 16-bit element from a 64-bit vector of `[4 x i16]` and
/// returns it, as specified by the immediate integer operand.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pextrw, imm2 = 0))]
#[rustc_args_required_const(1)]
pub unsafe fn _mm_extract_pi16(a: __m64, imm2: i32) -> i32 {
    macro_rules! call {
        ($imm2:expr) => {
            pextrw(a, $imm2) as i32
        };
    }
    constify_imm2!(imm2, call)
}

/// Extracts 16-bit element from a 64-bit vector of `[4 x i16]` and
/// returns it, as specified by the immediate integer operand.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pextrw, imm2 = 0))]
#[rustc_args_required_const(1)]
pub unsafe fn _m_pextrw(a: __m64, imm2: i32) -> i32 {
    macro_rules! call {
        ($imm2:expr) => {
            pextrw(a, $imm2) as i32
        };
    }
    constify_imm2!(imm2, call)
}

/// Copies data from the 64-bit vector of `[4 x i16]` to the destination,
/// and inserts the lower 16-bits of an integer operand at the 16-bit offset
/// specified by the immediate operand `n`.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pinsrw, imm2 = 0))]
#[rustc_args_required_const(2)]
pub unsafe fn _mm_insert_pi16(a: __m64, d: i32, imm2: i32) -> __m64 {
    macro_rules! call {
        ($imm2:expr) => {
            pinsrw(a, d, $imm2)
        };
    }
    constify_imm2!(imm2, call)
}

/// Copies data from the 64-bit vector of `[4 x i16]` to the destination,
/// and inserts the lower 16-bits of an integer operand at the 16-bit offset
/// specified by the immediate operand `n`.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pinsrw, imm2 = 0))]
#[rustc_args_required_const(2)]
pub unsafe fn _m_pinsrw(a: __m64, d: i32, imm2: i32) -> __m64 {
    macro_rules! call {
        ($imm2:expr) => {
            pinsrw(a, d, $imm2)
        };
    }
    constify_imm2!(imm2, call)
}

/// Takes the most significant bit from each 8-bit element in a 64-bit
/// integer vector to create a 16-bit mask value. Zero-extends the value to
/// 32-bit integer and writes it to the destination.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pmovmskb))]
pub unsafe fn _mm_movemask_pi8(a: __m64) -> i32 {
    pmovmskb(a)
}

/// Takes the most significant bit from each 8-bit element in a 64-bit
/// integer vector to create a 16-bit mask value. Zero-extends the value to
/// 32-bit integer and writes it to the destination.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pmovmskb))]
pub unsafe fn _m_pmovmskb(a: __m64) -> i32 {
    _mm_movemask_pi8(a)
}

/// Shuffles the 4 16-bit integers from a 64-bit integer vector to the
/// destination, as specified by the immediate value operand.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pshufw, imm8 = 0))]
#[rustc_args_required_const(1)]
pub unsafe fn _mm_shuffle_pi16(a: __m64, imm8: i32) -> __m64 {
    macro_rules! call {
        ($imm8:expr) => {
            pshufw(a, $imm8)
        };
    }
    constify_imm8!(imm8, call)
}

/// Shuffles the 4 16-bit integers from a 64-bit integer vector to the
/// destination, as specified by the immediate value operand.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(pshufw, imm8 = 0))]
#[rustc_args_required_const(1)]
pub unsafe fn _m_pshufw(a: __m64, imm8: i32) -> __m64 {
    macro_rules! call {
        ($imm8:expr) => {
            pshufw(a, $imm8)
        };
    }
    constify_imm8!(imm8, call)
}

/// Convert the two lower packed single-precision (32-bit) floating-point
/// elements in `a` to packed 32-bit integers with truncation.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvttps2pi))]
pub unsafe fn _mm_cvttps_pi32(a: __m128) -> __m64 {
    cvttps2pi(a)
}

/// Convert the two lower packed single-precision (32-bit) floating-point
/// elements in `a` to packed 32-bit integers with truncation.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvttps2pi))]
pub unsafe fn _mm_cvtt_ps2pi(a: __m128) -> __m64 {
    _mm_cvttps_pi32(a)
}

/// Convert the two lower packed single-precision (32-bit) floating-point
/// elements in `a` to packed 32-bit integers.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvtps2pi))]
pub unsafe fn _mm_cvtps_pi32(a: __m128) -> __m64 {
    cvtps2pi(a)
}

/// Convert the two lower packed single-precision (32-bit) floating-point
/// elements in `a` to packed 32-bit integers.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvtps2pi))]
pub unsafe fn _mm_cvt_ps2pi(a: __m128) -> __m64 {
    _mm_cvtps_pi32(a)
}

/// Convert packed single-precision (32-bit) floating-point elements in `a` to
/// packed 16-bit integers.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvtps2pi))]
pub unsafe fn _mm_cvtps_pi16(a: __m128) -> __m64 {
    let b = _mm_cvtps_pi32(a);
    let a = _mm_movehl_ps(a, a);
    let c = _mm_cvtps_pi32(a);
    _mm_packs_pi32(b, c)
}

/// Convert packed single-precision (32-bit) floating-point elements in `a` to
/// packed 8-bit integers, and returns theem in the lower 4 elements of the
/// result.
#[inline]
#[target_feature(enable = "sse,mmx")]
#[cfg_attr(test, assert_instr(cvtps2pi))]
pub unsafe fn _mm_cvtps_pi8(a: __m128) -> __m64 {
    let b = _mm_cvtps_pi16(a);
    let c = _mm_setzero_si64();
    _mm_packs_pi16(b, c)
}

#[cfg(test)]
mod tests {
    use std::f32::NAN;
    use std::mem::transmute;
    use stdsimd_test::simd_test;
    use test::black_box; // Used to inhibit constant-folding.

    use coresimd::simd::*;
    use coresimd::x86::*;

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_add_ps() {
        let a = _mm_setr_ps(-1.0, 5.0, 0.0, -10.0);
        let b = _mm_setr_ps(-100.0, 20.0, 0.0, -5.0);
        let r = _mm_add_ps(a, b);
        assert_eq_m128(r, _mm_setr_ps(-101.0, 25.0, 0.0, -15.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_add_ss() {
        let a = _mm_set_ps(-1.0, 5.0, 0.0, -10.0);
        let b = _mm_set_ps(-100.0, 20.0, 0.0, -5.0);
        let r = _mm_add_ss(a, b);
        assert_eq_m128(r, _mm_set_ps(-1.0, 5.0, 0.0, -15.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_sub_ps() {
        let a = _mm_setr_ps(-1.0, 5.0, 0.0, -10.0);
        let b = _mm_setr_ps(-100.0, 20.0, 0.0, -5.0);
        let r = _mm_sub_ps(a, b);
        assert_eq_m128(r, _mm_setr_ps(99.0, -15.0, 0.0, -5.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_sub_ss() {
        let a = _mm_setr_ps(-1.0, 5.0, 0.0, -10.0);
        let b = _mm_setr_ps(-100.0, 20.0, 0.0, -5.0);
        let r = _mm_sub_ss(a, b);
        assert_eq_m128(r, _mm_setr_ps(99.0, 5.0, 0.0, -10.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_mul_ps() {
        let a = _mm_setr_ps(-1.0, 5.0, 0.0, -10.0);
        let b = _mm_setr_ps(-100.0, 20.0, 0.0, -5.0);
        let r = _mm_mul_ps(a, b);
        assert_eq_m128(r, _mm_setr_ps(100.0, 100.0, 0.0, 50.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_mul_ss() {
        let a = _mm_setr_ps(-1.0, 5.0, 0.0, -10.0);
        let b = _mm_setr_ps(-100.0, 20.0, 0.0, -5.0);
        let r = _mm_mul_ss(a, b);
        assert_eq_m128(r, _mm_setr_ps(100.0, 5.0, 0.0, -10.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_div_ps() {
        let a = _mm_setr_ps(-1.0, 5.0, 2.0, -10.0);
        let b = _mm_setr_ps(-100.0, 20.0, 0.2, -5.0);
        let r = _mm_div_ps(a, b);
        assert_eq_m128(r, _mm_setr_ps(0.01, 0.25, 10.0, 2.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_div_ss() {
        let a = _mm_setr_ps(-1.0, 5.0, 0.0, -10.0);
        let b = _mm_setr_ps(-100.0, 20.0, 0.0, -5.0);
        let r = _mm_div_ss(a, b);
        assert_eq_m128(r, _mm_setr_ps(0.01, 5.0, 0.0, -10.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_sqrt_ss() {
        let a = _mm_setr_ps(4.0, 13.0, 16.0, 100.0);
        let r = _mm_sqrt_ss(a);
        let e = _mm_setr_ps(2.0, 13.0, 16.0, 100.0);
        assert_eq_m128(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_sqrt_ps() {
        let a = _mm_setr_ps(4.0, 13.0, 16.0, 100.0);
        let r = _mm_sqrt_ps(a);
        let e = _mm_setr_ps(2.0, 3.6055512, 4.0, 10.0);
        assert_eq_m128(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_rcp_ss() {
        let a = _mm_setr_ps(4.0, 13.0, 16.0, 100.0);
        let r = _mm_rcp_ss(a);
        let e = _mm_setr_ps(0.24993896, 13.0, 16.0, 100.0);
        assert_eq_m128(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_rcp_ps() {
        let a = _mm_setr_ps(4.0, 13.0, 16.0, 100.0);
        let r = _mm_rcp_ps(a);
        let e = _mm_setr_ps(0.24993896, 0.0769043, 0.06248474, 0.0099983215);
        let rel_err = 0.00048828125;
        for i in 0..4 {
            assert_approx_eq!(get_m128(r, i), get_m128(e, i), 2. * rel_err);
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_rsqrt_ss() {
        let a = _mm_setr_ps(4.0, 13.0, 16.0, 100.0);
        let r = _mm_rsqrt_ss(a);
        let e = _mm_setr_ps(0.49987793, 13.0, 16.0, 100.0);
        let rel_err = 0.00048828125;
        for i in 0..4 {
            assert_approx_eq!(get_m128(r, i), get_m128(e, i), 2. * rel_err);
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_rsqrt_ps() {
        let a = _mm_setr_ps(4.0, 13.0, 16.0, 100.0);
        let r = _mm_rsqrt_ps(a);
        let e = _mm_setr_ps(0.49987793, 0.2772827, 0.24993896, 0.099990845);
        let rel_err = 0.00048828125;
        for i in 0..4 {
            assert_approx_eq!(get_m128(r, i), get_m128(e, i), 2. * rel_err);
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_min_ss() {
        let a = _mm_setr_ps(-1.0, 5.0, 0.0, -10.0);
        let b = _mm_setr_ps(-100.0, 20.0, 0.0, -5.0);
        let r = _mm_min_ss(a, b);
        assert_eq_m128(r, _mm_setr_ps(-100.0, 5.0, 0.0, -10.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_min_ps() {
        let a = _mm_setr_ps(-1.0, 5.0, 0.0, -10.0);
        let b = _mm_setr_ps(-100.0, 20.0, 0.0, -5.0);
        let r = _mm_min_ps(a, b);
        assert_eq_m128(r, _mm_setr_ps(-100.0, 5.0, 0.0, -10.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_max_ss() {
        let a = _mm_setr_ps(-1.0, 5.0, 0.0, -10.0);
        let b = _mm_setr_ps(-100.0, 20.0, 0.0, -5.0);
        let r = _mm_max_ss(a, b);
        assert_eq_m128(r, _mm_setr_ps(-1.0, 5.0, 0.0, -10.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_max_ps() {
        let a = _mm_setr_ps(-1.0, 5.0, 0.0, -10.0);
        let b = _mm_setr_ps(-100.0, 20.0, 0.0, -5.0);
        let r = _mm_max_ps(a, b);
        assert_eq_m128(r, _mm_setr_ps(-1.0, 20.0, 0.0, -5.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_and_ps() {
        let a = transmute(u32x4::splat(0b0011));
        let b = transmute(u32x4::splat(0b0101));
        let r = _mm_and_ps(*black_box(&a), *black_box(&b));
        let e = transmute(u32x4::splat(0b0001));
        assert_eq_m128(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_andnot_ps() {
        let a = transmute(u32x4::splat(0b0011));
        let b = transmute(u32x4::splat(0b0101));
        let r = _mm_andnot_ps(*black_box(&a), *black_box(&b));
        let e = transmute(u32x4::splat(0b0100));
        assert_eq_m128(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_or_ps() {
        let a = transmute(u32x4::splat(0b0011));
        let b = transmute(u32x4::splat(0b0101));
        let r = _mm_or_ps(*black_box(&a), *black_box(&b));
        let e = transmute(u32x4::splat(0b0111));
        assert_eq_m128(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_xor_ps() {
        let a = transmute(u32x4::splat(0b0011));
        let b = transmute(u32x4::splat(0b0101));
        let r = _mm_xor_ps(*black_box(&a), *black_box(&b));
        let e = transmute(u32x4::splat(0b0110));
        assert_eq_m128(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpeq_ss() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(-1.0, 5.0, 6.0, 7.0);
        let r: u32x4 = transmute(_mm_cmpeq_ss(a, b));
        let e: u32x4 = transmute(_mm_setr_ps(transmute(0u32), 2.0, 3.0, 4.0));
        assert_eq!(r, e);

        let b2 = _mm_setr_ps(1.0, 5.0, 6.0, 7.0);
        let r2: u32x4 = transmute(_mm_cmpeq_ss(a, b2));
        let e2: u32x4 =
            transmute(_mm_setr_ps(transmute(0xffffffffu32), 2.0, 3.0, 4.0));
        assert_eq!(r2, e2);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmplt_ss() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(0.0, 5.0, 6.0, 7.0);
        let c = _mm_setr_ps(1.0, 5.0, 6.0, 7.0);
        let d = _mm_setr_ps(2.0, 5.0, 6.0, 7.0);

        let b1 = 0u32; // a.extract(0) < b.extract(0)
        let c1 = 0u32; // a.extract(0) < c.extract(0)
        let d1 = !0u32; // a.extract(0) < d.extract(0)

        let rb: u32x4 = transmute(_mm_cmplt_ss(a, b));
        let eb: u32x4 = transmute(_mm_setr_ps(transmute(b1), 2.0, 3.0, 4.0));
        assert_eq!(rb, eb);

        let rc: u32x4 = transmute(_mm_cmplt_ss(a, c));
        let ec: u32x4 = transmute(_mm_setr_ps(transmute(c1), 2.0, 3.0, 4.0));
        assert_eq!(rc, ec);

        let rd: u32x4 = transmute(_mm_cmplt_ss(a, d));
        let ed: u32x4 = transmute(_mm_setr_ps(transmute(d1), 2.0, 3.0, 4.0));
        assert_eq!(rd, ed);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmple_ss() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(0.0, 5.0, 6.0, 7.0);
        let c = _mm_setr_ps(1.0, 5.0, 6.0, 7.0);
        let d = _mm_setr_ps(2.0, 5.0, 6.0, 7.0);

        let b1 = 0u32; // a.extract(0) <= b.extract(0)
        let c1 = !0u32; // a.extract(0) <= c.extract(0)
        let d1 = !0u32; // a.extract(0) <= d.extract(0)

        let rb: u32x4 = transmute(_mm_cmple_ss(a, b));
        let eb: u32x4 = transmute(_mm_setr_ps(transmute(b1), 2.0, 3.0, 4.0));
        assert_eq!(rb, eb);

        let rc: u32x4 = transmute(_mm_cmple_ss(a, c));
        let ec: u32x4 = transmute(_mm_setr_ps(transmute(c1), 2.0, 3.0, 4.0));
        assert_eq!(rc, ec);

        let rd: u32x4 = transmute(_mm_cmple_ss(a, d));
        let ed: u32x4 = transmute(_mm_setr_ps(transmute(d1), 2.0, 3.0, 4.0));
        assert_eq!(rd, ed);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpgt_ss() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(0.0, 5.0, 6.0, 7.0);
        let c = _mm_setr_ps(1.0, 5.0, 6.0, 7.0);
        let d = _mm_setr_ps(2.0, 5.0, 6.0, 7.0);

        let b1 = !0u32; // a.extract(0) > b.extract(0)
        let c1 = 0u32; // a.extract(0) > c.extract(0)
        let d1 = 0u32; // a.extract(0) > d.extract(0)

        let rb: u32x4 = transmute(_mm_cmpgt_ss(a, b));
        let eb: u32x4 = transmute(_mm_setr_ps(transmute(b1), 2.0, 3.0, 4.0));
        assert_eq!(rb, eb);

        let rc: u32x4 = transmute(_mm_cmpgt_ss(a, c));
        let ec: u32x4 = transmute(_mm_setr_ps(transmute(c1), 2.0, 3.0, 4.0));
        assert_eq!(rc, ec);

        let rd: u32x4 = transmute(_mm_cmpgt_ss(a, d));
        let ed: u32x4 = transmute(_mm_setr_ps(transmute(d1), 2.0, 3.0, 4.0));
        assert_eq!(rd, ed);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpge_ss() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(0.0, 5.0, 6.0, 7.0);
        let c = _mm_setr_ps(1.0, 5.0, 6.0, 7.0);
        let d = _mm_setr_ps(2.0, 5.0, 6.0, 7.0);

        let b1 = !0u32; // a.extract(0) >= b.extract(0)
        let c1 = !0u32; // a.extract(0) >= c.extract(0)
        let d1 = 0u32; // a.extract(0) >= d.extract(0)

        let rb: u32x4 = transmute(_mm_cmpge_ss(a, b));
        let eb: u32x4 = transmute(_mm_setr_ps(transmute(b1), 2.0, 3.0, 4.0));
        assert_eq!(rb, eb);

        let rc: u32x4 = transmute(_mm_cmpge_ss(a, c));
        let ec: u32x4 = transmute(_mm_setr_ps(transmute(c1), 2.0, 3.0, 4.0));
        assert_eq!(rc, ec);

        let rd: u32x4 = transmute(_mm_cmpge_ss(a, d));
        let ed: u32x4 = transmute(_mm_setr_ps(transmute(d1), 2.0, 3.0, 4.0));
        assert_eq!(rd, ed);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpneq_ss() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(0.0, 5.0, 6.0, 7.0);
        let c = _mm_setr_ps(1.0, 5.0, 6.0, 7.0);
        let d = _mm_setr_ps(2.0, 5.0, 6.0, 7.0);

        let b1 = !0u32; // a.extract(0) != b.extract(0)
        let c1 = 0u32; // a.extract(0) != c.extract(0)
        let d1 = !0u32; // a.extract(0) != d.extract(0)

        let rb: u32x4 = transmute(_mm_cmpneq_ss(a, b));
        let eb: u32x4 = transmute(_mm_setr_ps(transmute(b1), 2.0, 3.0, 4.0));
        assert_eq!(rb, eb);

        let rc: u32x4 = transmute(_mm_cmpneq_ss(a, c));
        let ec: u32x4 = transmute(_mm_setr_ps(transmute(c1), 2.0, 3.0, 4.0));
        assert_eq!(rc, ec);

        let rd: u32x4 = transmute(_mm_cmpneq_ss(a, d));
        let ed: u32x4 = transmute(_mm_setr_ps(transmute(d1), 2.0, 3.0, 4.0));
        assert_eq!(rd, ed);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpnlt_ss() {
        // TODO: This test is exactly the same as for _mm_cmpge_ss, but there
        // must be a difference. It may have to do with behavior in the
        // presence of NaNs (signaling or quiet). If so, we should add tests
        // for those.

        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(0.0, 5.0, 6.0, 7.0);
        let c = _mm_setr_ps(1.0, 5.0, 6.0, 7.0);
        let d = _mm_setr_ps(2.0, 5.0, 6.0, 7.0);

        let b1 = !0u32; // a.extract(0) >= b.extract(0)
        let c1 = !0u32; // a.extract(0) >= c.extract(0)
        let d1 = 0u32; // a.extract(0) >= d.extract(0)

        let rb: u32x4 = transmute(_mm_cmpnlt_ss(a, b));
        let eb: u32x4 = transmute(_mm_setr_ps(transmute(b1), 2.0, 3.0, 4.0));
        assert_eq!(rb, eb);

        let rc: u32x4 = transmute(_mm_cmpnlt_ss(a, c));
        let ec: u32x4 = transmute(_mm_setr_ps(transmute(c1), 2.0, 3.0, 4.0));
        assert_eq!(rc, ec);

        let rd: u32x4 = transmute(_mm_cmpnlt_ss(a, d));
        let ed: u32x4 = transmute(_mm_setr_ps(transmute(d1), 2.0, 3.0, 4.0));
        assert_eq!(rd, ed);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpnle_ss() {
        // TODO: This test is exactly the same as for _mm_cmpgt_ss, but there
        // must be a difference. It may have to do with behavior in the
        // presence
        // of NaNs (signaling or quiet). If so, we should add tests for those.

        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(0.0, 5.0, 6.0, 7.0);
        let c = _mm_setr_ps(1.0, 5.0, 6.0, 7.0);
        let d = _mm_setr_ps(2.0, 5.0, 6.0, 7.0);

        let b1 = !0u32; // a.extract(0) > b.extract(0)
        let c1 = 0u32; // a.extract(0) > c.extract(0)
        let d1 = 0u32; // a.extract(0) > d.extract(0)

        let rb: u32x4 = transmute(_mm_cmpnle_ss(a, b));
        let eb: u32x4 = transmute(_mm_setr_ps(transmute(b1), 2.0, 3.0, 4.0));
        assert_eq!(rb, eb);

        let rc: u32x4 = transmute(_mm_cmpnle_ss(a, c));
        let ec: u32x4 = transmute(_mm_setr_ps(transmute(c1), 2.0, 3.0, 4.0));
        assert_eq!(rc, ec);

        let rd: u32x4 = transmute(_mm_cmpnle_ss(a, d));
        let ed: u32x4 = transmute(_mm_setr_ps(transmute(d1), 2.0, 3.0, 4.0));
        assert_eq!(rd, ed);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpngt_ss() {
        // TODO: This test is exactly the same as for _mm_cmple_ss, but there
        // must be a difference. It may have to do with behavior in the
        // presence of NaNs (signaling or quiet). If so, we should add tests
        // for those.

        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(0.0, 5.0, 6.0, 7.0);
        let c = _mm_setr_ps(1.0, 5.0, 6.0, 7.0);
        let d = _mm_setr_ps(2.0, 5.0, 6.0, 7.0);

        let b1 = 0u32; // a.extract(0) <= b.extract(0)
        let c1 = !0u32; // a.extract(0) <= c.extract(0)
        let d1 = !0u32; // a.extract(0) <= d.extract(0)

        let rb: u32x4 = transmute(_mm_cmpngt_ss(a, b));
        let eb: u32x4 = transmute(_mm_setr_ps(transmute(b1), 2.0, 3.0, 4.0));
        assert_eq!(rb, eb);

        let rc: u32x4 = transmute(_mm_cmpngt_ss(a, c));
        let ec: u32x4 = transmute(_mm_setr_ps(transmute(c1), 2.0, 3.0, 4.0));
        assert_eq!(rc, ec);

        let rd: u32x4 = transmute(_mm_cmpngt_ss(a, d));
        let ed: u32x4 = transmute(_mm_setr_ps(transmute(d1), 2.0, 3.0, 4.0));
        assert_eq!(rd, ed);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpnge_ss() {
        // TODO: This test is exactly the same as for _mm_cmplt_ss, but there
        // must be a difference. It may have to do with behavior in the
        // presence of NaNs (signaling or quiet). If so, we should add tests
        // for those.

        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(0.0, 5.0, 6.0, 7.0);
        let c = _mm_setr_ps(1.0, 5.0, 6.0, 7.0);
        let d = _mm_setr_ps(2.0, 5.0, 6.0, 7.0);

        let b1 = 0u32; // a.extract(0) < b.extract(0)
        let c1 = 0u32; // a.extract(0) < c.extract(0)
        let d1 = !0u32; // a.extract(0) < d.extract(0)

        let rb: u32x4 = transmute(_mm_cmpnge_ss(a, b));
        let eb: u32x4 = transmute(_mm_setr_ps(transmute(b1), 2.0, 3.0, 4.0));
        assert_eq!(rb, eb);

        let rc: u32x4 = transmute(_mm_cmpnge_ss(a, c));
        let ec: u32x4 = transmute(_mm_setr_ps(transmute(c1), 2.0, 3.0, 4.0));
        assert_eq!(rc, ec);

        let rd: u32x4 = transmute(_mm_cmpnge_ss(a, d));
        let ed: u32x4 = transmute(_mm_setr_ps(transmute(d1), 2.0, 3.0, 4.0));
        assert_eq!(rd, ed);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpord_ss() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(0.0, 5.0, 6.0, 7.0);
        let c = _mm_setr_ps(NAN, 5.0, 6.0, 7.0);
        let d = _mm_setr_ps(2.0, 5.0, 6.0, 7.0);

        let b1 = !0u32; // a.extract(0) ord b.extract(0)
        let c1 = 0u32; // a.extract(0) ord c.extract(0)
        let d1 = !0u32; // a.extract(0) ord d.extract(0)

        let rb: u32x4 = transmute(_mm_cmpord_ss(a, b));
        let eb: u32x4 = transmute(_mm_setr_ps(transmute(b1), 2.0, 3.0, 4.0));
        assert_eq!(rb, eb);

        let rc: u32x4 = transmute(_mm_cmpord_ss(a, c));
        let ec: u32x4 = transmute(_mm_setr_ps(transmute(c1), 2.0, 3.0, 4.0));
        assert_eq!(rc, ec);

        let rd: u32x4 = transmute(_mm_cmpord_ss(a, d));
        let ed: u32x4 = transmute(_mm_setr_ps(transmute(d1), 2.0, 3.0, 4.0));
        assert_eq!(rd, ed);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpunord_ss() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(0.0, 5.0, 6.0, 7.0);
        let c = _mm_setr_ps(NAN, 5.0, 6.0, 7.0);
        let d = _mm_setr_ps(2.0, 5.0, 6.0, 7.0);

        let b1 = 0u32; // a.extract(0) unord b.extract(0)
        let c1 = !0u32; // a.extract(0) unord c.extract(0)
        let d1 = 0u32; // a.extract(0) unord d.extract(0)

        let rb: u32x4 = transmute(_mm_cmpunord_ss(a, b));
        let eb: u32x4 = transmute(_mm_setr_ps(transmute(b1), 2.0, 3.0, 4.0));
        assert_eq!(rb, eb);

        let rc: u32x4 = transmute(_mm_cmpunord_ss(a, c));
        let ec: u32x4 = transmute(_mm_setr_ps(transmute(c1), 2.0, 3.0, 4.0));
        assert_eq!(rc, ec);

        let rd: u32x4 = transmute(_mm_cmpunord_ss(a, d));
        let ed: u32x4 = transmute(_mm_setr_ps(transmute(d1), 2.0, 3.0, 4.0));
        assert_eq!(rd, ed);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpeq_ps() {
        let a = _mm_setr_ps(10.0, 50.0, 1.0, NAN);
        let b = _mm_setr_ps(15.0, 20.0, 1.0, NAN);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(fls, fls, tru, fls);
        let r: u32x4 = transmute(_mm_cmpeq_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmplt_ps() {
        let a = _mm_setr_ps(10.0, 50.0, 1.0, NAN);
        let b = _mm_setr_ps(15.0, 20.0, 1.0, NAN);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(tru, fls, fls, fls);
        let r: u32x4 = transmute(_mm_cmplt_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmple_ps() {
        let a = _mm_setr_ps(10.0, 50.0, 1.0, 4.0);
        let b = _mm_setr_ps(15.0, 20.0, 1.0, NAN);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(tru, fls, tru, fls);
        let r: u32x4 = transmute(_mm_cmple_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpgt_ps() {
        let a = _mm_setr_ps(10.0, 50.0, 1.0, NAN);
        let b = _mm_setr_ps(15.0, 20.0, 1.0, 42.0);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(fls, tru, fls, fls);
        let r: u32x4 = transmute(_mm_cmpgt_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpge_ps() {
        let a = _mm_setr_ps(10.0, 50.0, 1.0, NAN);
        let b = _mm_setr_ps(15.0, 20.0, 1.0, 42.0);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(fls, tru, tru, fls);
        let r: u32x4 = transmute(_mm_cmpge_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpneq_ps() {
        let a = _mm_setr_ps(10.0, 50.0, 1.0, NAN);
        let b = _mm_setr_ps(15.0, 20.0, 1.0, NAN);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(tru, tru, fls, tru);
        let r: u32x4 = transmute(_mm_cmpneq_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpnlt_ps() {
        let a = _mm_setr_ps(10.0, 50.0, 1.0, NAN);
        let b = _mm_setr_ps(15.0, 20.0, 1.0, 5.0);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(fls, tru, tru, tru);
        let r: u32x4 = transmute(_mm_cmpnlt_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpnle_ps() {
        let a = _mm_setr_ps(10.0, 50.0, 1.0, NAN);
        let b = _mm_setr_ps(15.0, 20.0, 1.0, 5.0);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(fls, tru, fls, tru);
        let r: u32x4 = transmute(_mm_cmpnle_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpngt_ps() {
        let a = _mm_setr_ps(10.0, 50.0, 1.0, NAN);
        let b = _mm_setr_ps(15.0, 20.0, 1.0, 5.0);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(tru, fls, tru, tru);
        let r: u32x4 = transmute(_mm_cmpngt_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpnge_ps() {
        let a = _mm_setr_ps(10.0, 50.0, 1.0, NAN);
        let b = _mm_setr_ps(15.0, 20.0, 1.0, 5.0);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(tru, fls, fls, tru);
        let r: u32x4 = transmute(_mm_cmpnge_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpord_ps() {
        let a = _mm_setr_ps(10.0, 50.0, NAN, NAN);
        let b = _mm_setr_ps(15.0, NAN, 1.0, NAN);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(tru, fls, fls, fls);
        let r: u32x4 = transmute(_mm_cmpord_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cmpunord_ps() {
        let a = _mm_setr_ps(10.0, 50.0, NAN, NAN);
        let b = _mm_setr_ps(15.0, NAN, 1.0, NAN);
        let tru = !0u32;
        let fls = 0u32;

        let e = u32x4::new(fls, tru, tru, tru);
        let r: u32x4 = transmute(_mm_cmpunord_ps(a, b));
        assert_eq!(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_comieq_ss() {
        let aa = &[3.0f32, 12.0, 23.0, NAN];
        let bb = &[3.0f32, 47.5, 1.5, NAN];

        let ee = &[1i32, 0, 0, 0];

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            let r = _mm_comieq_ss(a, b);

            assert_eq!(
                ee[i], r,
                "_mm_comieq_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r, ee[i], i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_comilt_ss() {
        let aa = &[3.0f32, 12.0, 23.0, NAN];
        let bb = &[3.0f32, 47.5, 1.5, NAN];

        let ee = &[0i32, 1, 0, 0];

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            let r = _mm_comilt_ss(a, b);

            assert_eq!(
                ee[i], r,
                "_mm_comilt_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r, ee[i], i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_comile_ss() {
        let aa = &[3.0f32, 12.0, 23.0, NAN];
        let bb = &[3.0f32, 47.5, 1.5, NAN];

        let ee = &[1i32, 1, 0, 0];

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            let r = _mm_comile_ss(a, b);

            assert_eq!(
                ee[i], r,
                "_mm_comile_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r, ee[i], i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_comigt_ss() {
        let aa = &[3.0f32, 12.0, 23.0, NAN];
        let bb = &[3.0f32, 47.5, 1.5, NAN];

        let ee = &[1i32, 0, 1, 0];

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            let r = _mm_comige_ss(a, b);

            assert_eq!(
                ee[i], r,
                "_mm_comige_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r, ee[i], i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_comineq_ss() {
        let aa = &[3.0f32, 12.0, 23.0, NAN];
        let bb = &[3.0f32, 47.5, 1.5, NAN];

        let ee = &[0i32, 1, 1, 1];

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            let r = _mm_comineq_ss(a, b);

            assert_eq!(
                ee[i], r,
                "_mm_comineq_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r, ee[i], i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_ucomieq_ss() {
        let aa = &[3.0f32, 12.0, 23.0, NAN];
        let bb = &[3.0f32, 47.5, 1.5, NAN];

        let ee = &[1i32, 0, 0, 0];

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            let r = _mm_ucomieq_ss(a, b);

            assert_eq!(
                ee[i], r,
                "_mm_ucomieq_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r, ee[i], i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_ucomilt_ss() {
        let aa = &[3.0f32, 12.0, 23.0, NAN];
        let bb = &[3.0f32, 47.5, 1.5, NAN];

        let ee = &[0i32, 1, 0, 0];

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            let r = _mm_ucomilt_ss(a, b);

            assert_eq!(
                ee[i], r,
                "_mm_ucomilt_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r, ee[i], i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_ucomile_ss() {
        let aa = &[3.0f32, 12.0, 23.0, NAN];
        let bb = &[3.0f32, 47.5, 1.5, NAN];

        let ee = &[1i32, 1, 0, 0];

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            let r = _mm_ucomile_ss(a, b);

            assert_eq!(
                ee[i], r,
                "_mm_ucomile_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r, ee[i], i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_ucomigt_ss() {
        let aa = &[3.0f32, 12.0, 23.0, NAN];
        let bb = &[3.0f32, 47.5, 1.5, NAN];

        let ee = &[0i32, 0, 1, 0];

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            let r = _mm_ucomigt_ss(a, b);

            assert_eq!(
                ee[i], r,
                "_mm_ucomigt_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r, ee[i], i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_ucomige_ss() {
        let aa = &[3.0f32, 12.0, 23.0, NAN];
        let bb = &[3.0f32, 47.5, 1.5, NAN];

        let ee = &[1i32, 0, 1, 0];

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            let r = _mm_ucomige_ss(a, b);

            assert_eq!(
                ee[i], r,
                "_mm_ucomige_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r, ee[i], i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_ucomineq_ss() {
        let aa = &[3.0f32, 12.0, 23.0, NAN];
        let bb = &[3.0f32, 47.5, 1.5, NAN];

        let ee = &[0i32, 1, 1, 1];

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            let r = _mm_ucomineq_ss(a, b);

            assert_eq!(
                ee[i], r,
                "_mm_ucomineq_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r, ee[i], i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_comieq_ss_vs_ucomieq_ss() {
        // If one of the arguments is a quiet NaN `comieq_ss` should signal an
        // Invalid Operation Exception while `ucomieq_ss` should not.
        let aa = &[3.0f32, NAN, 23.0, NAN];
        let bb = &[3.0f32, 47.5, NAN, NAN];

        let ee = &[1i32, 0, 0, 0];
        let exc = &[0u32, 1, 1, 1]; // Should comieq_ss signal an exception?

        for i in 0..4 {
            let a = _mm_setr_ps(aa[i], 1.0, 2.0, 3.0);
            let b = _mm_setr_ps(bb[i], 0.0, 2.0, 4.0);

            _MM_SET_EXCEPTION_STATE(0);
            let r1 = _mm_comieq_ss(*black_box(&a), b);
            let s1 = _MM_GET_EXCEPTION_STATE();

            _MM_SET_EXCEPTION_STATE(0);
            let r2 = _mm_ucomieq_ss(*black_box(&a), b);
            let s2 = _MM_GET_EXCEPTION_STATE();

            assert_eq!(
                ee[i], r1,
                "_mm_comeq_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r1, ee[i], i
            );
            assert_eq!(
                ee[i], r2,
                "_mm_ucomeq_ss({:?}, {:?}) = {}, expected: {} (i={})",
                a, b, r2, ee[i], i
            );
            assert_eq!(
                s1,
                exc[i] * _MM_EXCEPT_INVALID,
                "_mm_comieq_ss() set exception flags: {} (i={})",
                s1,
                i
            );
            assert_eq!(
                s2,
                0, // ucomieq_ss should not signal an exception
                "_mm_ucomieq_ss() set exception flags: {} (i={})",
                s2,
                i
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cvtss_si32() {
        let inputs = &[42.0f32, -3.1, 4.0e10, 4.0e-20, NAN, 2147483500.1];
        let result =
            &[42i32, -3, i32::min_value(), 0, i32::min_value(), 2147483520];
        for i in 0..inputs.len() {
            let x = _mm_setr_ps(inputs[i], 1.0, 3.0, 4.0);
            let e = result[i];
            let r = _mm_cvtss_si32(x);
            assert_eq!(
                e, r,
                "TestCase #{} _mm_cvtss_si32({:?}) = {}, expected: {}",
                i, x, r, e
            );
        }
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_cvttss_si32() {
        let inputs = &[
            (42.0f32, 42i32),
            (-31.4, -31),
            (-33.5, -33),
            (-34.5, -34),
            (10.999, 10),
            (-5.99, -5),
            (4.0e10, i32::min_value()),
            (4.0e-10, 0),
            (NAN, i32::min_value()),
            (2147483500.1, 2147483520),
        ];
        for i in 0..inputs.len() {
            let (xi, e) = inputs[i];
            let x = _mm_setr_ps(xi, 1.0, 3.0, 4.0);
            let r = _mm_cvttss_si32(x);
            assert_eq!(
                e, r,
                "TestCase #{} _mm_cvttss_si32({:?}) = {}, expected: {}",
                i, x, r, e
            );
        }
    }

    #[simd_test(enable = "sse")]
    pub unsafe fn test_mm_cvtsi32_ss() {
        let inputs = &[
            (4555i32, 4555.0f32),
            (322223333, 322223330.0),
            (-432, -432.0),
            (-322223333, -322223330.0),
        ];

        for i in 0..inputs.len() {
            let (x, f) = inputs[i];
            let a = _mm_setr_ps(5.0, 6.0, 7.0, 8.0);
            let r = _mm_cvtsi32_ss(a, x);
            let e = _mm_setr_ps(f, 6.0, 7.0, 8.0);
            assert_eq_m128(e, r);
        }
    }

    #[simd_test(enable = "sse")]
    pub unsafe fn test_mm_cvtss_f32() {
        let a = _mm_setr_ps(312.0134, 5.0, 6.0, 7.0);
        assert_eq!(_mm_cvtss_f32(a), 312.0134);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_set_ss() {
        let r = _mm_set_ss(black_box(4.25));
        assert_eq_m128(r, _mm_setr_ps(4.25, 0.0, 0.0, 0.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_set1_ps() {
        let r1 = _mm_set1_ps(black_box(4.25));
        let r2 = _mm_set_ps1(black_box(4.25));
        assert_eq!(get_m128(r1, 0), 4.25);
        assert_eq!(get_m128(r1, 1), 4.25);
        assert_eq!(get_m128(r1, 2), 4.25);
        assert_eq!(get_m128(r1, 3), 4.25);
        assert_eq!(get_m128(r2, 0), 4.25);
        assert_eq!(get_m128(r2, 1), 4.25);
        assert_eq!(get_m128(r2, 2), 4.25);
        assert_eq!(get_m128(r2, 3), 4.25);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_set_ps() {
        let r = _mm_set_ps(
            black_box(1.0),
            black_box(2.0),
            black_box(3.0),
            black_box(4.0),
        );
        assert_eq!(get_m128(r, 0), 4.0);
        assert_eq!(get_m128(r, 1), 3.0);
        assert_eq!(get_m128(r, 2), 2.0);
        assert_eq!(get_m128(r, 3), 1.0);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_setr_ps() {
        let r = _mm_setr_ps(
            black_box(1.0),
            black_box(2.0),
            black_box(3.0),
            black_box(4.0),
        );
        assert_eq_m128(r, _mm_setr_ps(1.0, 2.0, 3.0, 4.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_setzero_ps() {
        let r = *black_box(&_mm_setzero_ps());
        assert_eq_m128(r, _mm_set1_ps(0.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_shuffle() {
        assert_eq!(_MM_SHUFFLE(0, 1, 1, 3), 0b00_01_01_11);
        assert_eq!(_MM_SHUFFLE(3, 1, 1, 0), 0b11_01_01_00);
        assert_eq!(_MM_SHUFFLE(1, 2, 2, 1), 0b01_10_10_01);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_shuffle_ps() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(5.0, 6.0, 7.0, 8.0);
        let r = _mm_shuffle_ps(a, b, 0b00_01_01_11);
        assert_eq_m128(r, _mm_setr_ps(4.0, 2.0, 6.0, 5.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_unpackhi_ps() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(5.0, 6.0, 7.0, 8.0);
        let r = _mm_unpackhi_ps(a, b);
        assert_eq_m128(r, _mm_setr_ps(3.0, 7.0, 4.0, 8.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_unpacklo_ps() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(5.0, 6.0, 7.0, 8.0);
        let r = _mm_unpacklo_ps(a, b);
        assert_eq_m128(r, _mm_setr_ps(1.0, 5.0, 2.0, 6.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_movehl_ps() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(5.0, 6.0, 7.0, 8.0);
        let r = _mm_movehl_ps(a, b);
        assert_eq_m128(r, _mm_setr_ps(7.0, 8.0, 3.0, 4.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_movelh_ps() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(5.0, 6.0, 7.0, 8.0);
        let r = _mm_movelh_ps(a, b);
        assert_eq_m128(r, _mm_setr_ps(1.0, 2.0, 5.0, 6.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_loadh_pi() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let x: [f32; 4] = [5.0, 6.0, 7.0, 8.0];
        let p = x[..].as_ptr();
        let r = _mm_loadh_pi(a, p as *const _);
        assert_eq_m128(r, _mm_setr_ps(1.0, 2.0, 5.0, 6.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_loadl_pi() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let x: [f32; 4] = [5.0, 6.0, 7.0, 8.0];
        let p = x[..].as_ptr();
        let r = _mm_loadl_pi(a, p as *const _);
        assert_eq_m128(r, _mm_setr_ps(5.0, 6.0, 3.0, 4.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_load_ss() {
        let a = 42.0f32;
        let r = _mm_load_ss(&a as *const f32);
        assert_eq_m128(r, _mm_setr_ps(42.0, 0.0, 0.0, 0.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_load1_ps() {
        let a = 42.0f32;
        let r = _mm_load1_ps(&a as *const f32);
        assert_eq_m128(r, _mm_setr_ps(42.0, 42.0, 42.0, 42.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_load_ps() {
        let vals = &[1.0f32, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0];

        let mut p = vals.as_ptr();
        let mut fixup = 0.0f32;

        // Make sure p is aligned, otherwise we might get a
        // (signal: 11, SIGSEGV: invalid memory reference)

        let unalignment = (p as usize) & 0xf;
        if unalignment != 0 {
            let delta = ((16 - unalignment) >> 2) as isize;
            fixup = delta as f32;
            p = p.offset(delta);
        }

        let r = _mm_load_ps(p);
        let e =
            _mm_add_ps(_mm_setr_ps(1.0, 2.0, 3.0, 4.0), _mm_set1_ps(fixup));
        assert_eq_m128(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_loadu_ps() {
        let vals = &[1.0f32, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0];
        let p = vals.as_ptr().offset(3);
        let r = _mm_loadu_ps(black_box(p));
        assert_eq_m128(r, _mm_setr_ps(4.0, 5.0, 6.0, 7.0));
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_loadr_ps() {
        let vals = &[1.0f32, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0];

        let mut p = vals.as_ptr();
        let mut fixup = 0.0f32;

        // Make sure p is aligned, otherwise we might get a
        // (signal: 11, SIGSEGV: invalid memory reference)

        let unalignment = (p as usize) & 0xf;
        if unalignment != 0 {
            let delta = ((16 - unalignment) >> 2) as isize;
            fixup = delta as f32;
            p = p.offset(delta);
        }

        let r = _mm_loadr_ps(p);
        let e =
            _mm_add_ps(_mm_setr_ps(4.0, 3.0, 2.0, 1.0), _mm_set1_ps(fixup));
        assert_eq_m128(r, e);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_storeh_pi() {
        let mut vals = [0.0f32; 8];
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        _mm_storeh_pi(vals.as_mut_ptr() as *mut _, a);

        assert_eq!(vals[0], 3.0);
        assert_eq!(vals[1], 4.0);
        assert_eq!(vals[2], 0.0);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_storel_pi() {
        let mut vals = [0.0f32; 8];
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        _mm_storel_pi(vals.as_mut_ptr() as *mut _, a);

        assert_eq!(vals[0], 1.0);
        assert_eq!(vals[1], 2.0);
        assert_eq!(vals[2], 0.0);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_store_ss() {
        let mut vals = [0.0f32; 8];
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        _mm_store_ss(vals.as_mut_ptr().offset(1), a);

        assert_eq!(vals[0], 0.0);
        assert_eq!(vals[1], 1.0);
        assert_eq!(vals[2], 0.0);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_store1_ps() {
        let mut vals = [0.0f32; 8];
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);

        let mut ofs = 0;
        let mut p = vals.as_mut_ptr();

        if (p as usize) & 0xf != 0 {
            ofs = (16 - (p as usize) & 0xf) >> 2;
            p = p.offset(ofs as isize);
        }

        _mm_store1_ps(p, *black_box(&a));

        if ofs > 0 {
            assert_eq!(vals[ofs - 1], 0.0);
        }
        assert_eq!(vals[ofs + 0], 1.0);
        assert_eq!(vals[ofs + 1], 1.0);
        assert_eq!(vals[ofs + 2], 1.0);
        assert_eq!(vals[ofs + 3], 1.0);
        assert_eq!(vals[ofs + 4], 0.0);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_store_ps() {
        let mut vals = [0.0f32; 8];
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);

        let mut ofs = 0;
        let mut p = vals.as_mut_ptr();

        // Align p to 16-byte boundary
        if (p as usize) & 0xf != 0 {
            ofs = (16 - (p as usize) & 0xf) >> 2;
            p = p.offset(ofs as isize);
        }

        _mm_store_ps(p, *black_box(&a));

        if ofs > 0 {
            assert_eq!(vals[ofs - 1], 0.0);
        }
        assert_eq!(vals[ofs + 0], 1.0);
        assert_eq!(vals[ofs + 1], 2.0);
        assert_eq!(vals[ofs + 2], 3.0);
        assert_eq!(vals[ofs + 3], 4.0);
        assert_eq!(vals[ofs + 4], 0.0);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_storer_ps() {
        let mut vals = [0.0f32; 8];
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);

        let mut ofs = 0;
        let mut p = vals.as_mut_ptr();

        // Align p to 16-byte boundary
        if (p as usize) & 0xf != 0 {
            ofs = (16 - (p as usize) & 0xf) >> 2;
            p = p.offset(ofs as isize);
        }

        _mm_storer_ps(p, *black_box(&a));

        if ofs > 0 {
            assert_eq!(vals[ofs - 1], 0.0);
        }
        assert_eq!(vals[ofs + 0], 4.0);
        assert_eq!(vals[ofs + 1], 3.0);
        assert_eq!(vals[ofs + 2], 2.0);
        assert_eq!(vals[ofs + 3], 1.0);
        assert_eq!(vals[ofs + 4], 0.0);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_storeu_ps() {
        let mut vals = [0.0f32; 8];
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);

        let mut ofs = 0;
        let mut p = vals.as_mut_ptr();

        // Make sure p is *not* aligned to 16-byte boundary
        if (p as usize) & 0xf == 0 {
            ofs = 1;
            p = p.offset(1);
        }

        _mm_storeu_ps(p, *black_box(&a));

        if ofs > 0 {
            assert_eq!(vals[ofs - 1], 0.0);
        }
        assert_eq!(vals[ofs + 0], 1.0);
        assert_eq!(vals[ofs + 1], 2.0);
        assert_eq!(vals[ofs + 2], 3.0);
        assert_eq!(vals[ofs + 3], 4.0);
        assert_eq!(vals[ofs + 4], 0.0);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_move_ss() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let b = _mm_setr_ps(5.0, 6.0, 7.0, 8.0);

        let r = _mm_move_ss(a, b);
        let e = _mm_setr_ps(5.0, 2.0, 3.0, 4.0);
        assert_eq_m128(e, r);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_movemask_ps() {
        let r = _mm_movemask_ps(_mm_setr_ps(-1.0, 5.0, -5.0, 0.0));
        assert_eq!(r, 0b0101);

        let r = _mm_movemask_ps(_mm_setr_ps(-1.0, -5.0, -5.0, 0.0));
        assert_eq!(r, 0b0111);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_sfence() {
        _mm_sfence();
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_getcsr_setcsr_1() {
        let saved_csr = _mm_getcsr();

        let a = _mm_setr_ps(1.1e-36, 0.0, 0.0, 1.0);
        let b = _mm_setr_ps(0.001, 0.0, 0.0, 1.0);

        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
        let r = _mm_mul_ps(*black_box(&a), *black_box(&b));

        _mm_setcsr(saved_csr);

        let exp = _mm_setr_ps(0.0, 0.0, 0.0, 1.0);
        assert_eq_m128(r, exp); // first component is a denormalized f32
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_getcsr_setcsr_2() {
        // Same as _mm_setcsr_1 test, but with opposite flag value.

        let saved_csr = _mm_getcsr();

        let a = _mm_setr_ps(1.1e-36, 0.0, 0.0, 1.0);
        let b = _mm_setr_ps(0.001, 0.0, 0.0, 1.0);

        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF);
        let r = _mm_mul_ps(*black_box(&a), *black_box(&b));

        _mm_setcsr(saved_csr);

        let exp = _mm_setr_ps(1.1e-39, 0.0, 0.0, 1.0);
        assert_eq_m128(r, exp); // first component is a denormalized f32
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_getcsr_setcsr_underflow() {
        _MM_SET_EXCEPTION_STATE(0);

        let a = _mm_setr_ps(1.1e-36, 0.0, 0.0, 1.0);
        let b = _mm_setr_ps(1e-5, 0.0, 0.0, 1.0);

        assert_eq!(_MM_GET_EXCEPTION_STATE(), 0); // just to be sure

        let r = _mm_mul_ps(*black_box(&a), *black_box(&b));

        let exp = _mm_setr_ps(1.1e-41, 0.0, 0.0, 1.0);
        assert_eq_m128(r, exp);

        let underflow = _MM_GET_EXCEPTION_STATE() & _MM_EXCEPT_UNDERFLOW != 0;
        assert_eq!(underflow, true);
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_MM_TRANSPOSE4_PS() {
        let mut a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let mut b = _mm_setr_ps(5.0, 6.0, 7.0, 8.0);
        let mut c = _mm_setr_ps(9.0, 10.0, 11.0, 12.0);
        let mut d = _mm_setr_ps(13.0, 14.0, 15.0, 16.0);

        _MM_TRANSPOSE4_PS(&mut a, &mut b, &mut c, &mut d);

        assert_eq_m128(a, _mm_setr_ps(1.0, 5.0, 9.0, 13.0));
        assert_eq_m128(b, _mm_setr_ps(2.0, 6.0, 10.0, 14.0));
        assert_eq_m128(c, _mm_setr_ps(3.0, 7.0, 11.0, 15.0));
        assert_eq_m128(d, _mm_setr_ps(4.0, 8.0, 12.0, 16.0));
    }

    #[repr(align(16))]
    struct Memory {
        pub data: [f32; 4],
    }

    #[simd_test(enable = "sse")]
    unsafe fn test_mm_stream_ps() {
        let a = _mm_set1_ps(7.0);
        let mut mem = Memory { data: [-1.0; 4] };

        _mm_stream_ps(&mut mem.data[0] as *mut f32, a);
        for i in 0..4 {
            assert_eq!(mem.data[i], get_m128(a, i));
        }
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_stream_pi() {
        let a = transmute(i8x8::new(0, 0, 0, 0, 0, 0, 0, 7));
        let mut mem =
            ::std::boxed::Box::<__m64>::new(transmute(i8x8::splat(1)));
        _mm_stream_pi(&mut *mem as *mut _ as *mut _, a);
        assert_eq_m64(a, *mem);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_max_pi16() {
        let a = _mm_setr_pi16(-1, 6, -3, 8);
        let b = _mm_setr_pi16(5, -2, 7, -4);
        let r = _mm_setr_pi16(5, 6, 7, 8);

        assert_eq_m64(r, _mm_max_pi16(a, b));
        assert_eq_m64(r, _m_pmaxsw(a, b));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_max_pu8() {
        let a = _mm_setr_pi8(2, 6, 3, 8, 2, 6, 3, 8);
        let b = _mm_setr_pi8(5, 2, 7, 4, 5, 2, 7, 4);
        let r = _mm_setr_pi8(5, 6, 7, 8, 5, 6, 7, 8);

        assert_eq_m64(r, _mm_max_pu8(a, b));
        assert_eq_m64(r, _m_pmaxub(a, b));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_min_pi16() {
        let a = _mm_setr_pi16(-1, 6, -3, 8);
        let b = _mm_setr_pi16(5, -2, 7, -4);
        let r = _mm_setr_pi16(-1, -2, -3, -4);

        assert_eq_m64(r, _mm_min_pi16(a, b));
        assert_eq_m64(r, _m_pminsw(a, b));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_min_pu8() {
        let a = _mm_setr_pi8(2, 6, 3, 8, 2, 6, 3, 8);
        let b = _mm_setr_pi8(5, 2, 7, 4, 5, 2, 7, 4);
        let r = _mm_setr_pi8(2, 2, 3, 4, 2, 2, 3, 4);

        assert_eq_m64(r, _mm_min_pu8(a, b));
        assert_eq_m64(r, _m_pminub(a, b));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_mulhi_pu16() {
        let (a, b) = (_mm_set1_pi16(1000), _mm_set1_pi16(1001));
        let r = _mm_mulhi_pu16(a, b);
        assert_eq_m64(r, _mm_set1_pi16(15));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_mullo_pi16() {
        let (a, b) = (_mm_set1_pi16(1000), _mm_set1_pi16(1001));
        let r = _mm_mullo_pi16(a, b);
        assert_eq_m64(r, _mm_set1_pi16(17960));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_m_pmulhuw() {
        let (a, b) = (_mm_set1_pi16(1000), _mm_set1_pi16(1001));
        let r = _m_pmulhuw(a, b);
        assert_eq_m64(r, _mm_set1_pi16(15));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_avg_pu8() {
        let (a, b) = (_mm_set1_pi8(3), _mm_set1_pi8(9));
        let r = _mm_avg_pu8(a, b);
        assert_eq_m64(r, _mm_set1_pi8(6));

        let r = _m_pavgb(a, b);
        assert_eq_m64(r, _mm_set1_pi8(6));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_avg_pu16() {
        let (a, b) = (_mm_set1_pi16(3), _mm_set1_pi16(9));
        let r = _mm_avg_pu16(a, b);
        assert_eq_m64(r, _mm_set1_pi16(6));

        let r = _m_pavgw(a, b);
        assert_eq_m64(r, _mm_set1_pi16(6));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_sad_pu8() {
        #[cfg_attr(rustfmt, rustfmt_skip)]
        let a = _mm_setr_pi8(
            255u8 as i8, 254u8 as i8, 253u8 as i8, 252u8 as i8,
            1, 2, 3, 4,
        );
        let b = _mm_setr_pi8(0, 0, 0, 0, 2, 1, 2, 1);
        let r = _mm_sad_pu8(a, b);
        assert_eq_m64(r, _mm_setr_pi16(1020, 0, 0, 0));

        let r = _m_psadbw(a, b);
        assert_eq_m64(r, _mm_setr_pi16(1020, 0, 0, 0));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_cvtpi32_ps() {
        let a = _mm_setr_ps(0., 0., 3., 4.);
        let b = _mm_setr_pi32(1, 2);
        let expected = _mm_setr_ps(1., 2., 3., 4.);
        let r = _mm_cvtpi32_ps(a, b);
        assert_eq_m128(r, expected);

        let r = _mm_cvt_pi2ps(a, b);
        assert_eq_m128(r, expected);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_cvtpi16_ps() {
        let a = _mm_setr_pi16(1, 2, 3, 4);
        let expected = _mm_setr_ps(1., 2., 3., 4.);
        let r = _mm_cvtpi16_ps(a);
        assert_eq_m128(r, expected);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_cvtpu16_ps() {
        let a = _mm_setr_pi16(1, 2, 3, 4);
        let expected = _mm_setr_ps(1., 2., 3., 4.);
        let r = _mm_cvtpu16_ps(a);
        assert_eq_m128(r, expected);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_cvtpi8_ps() {
        let a = _mm_setr_pi8(1, 2, 3, 4, 5, 6, 7, 8);
        let expected = _mm_setr_ps(1., 2., 3., 4.);
        let r = _mm_cvtpi8_ps(a);
        assert_eq_m128(r, expected);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_cvtpu8_ps() {
        let a = _mm_setr_pi8(1, 2, 3, 4, 5, 6, 7, 8);
        let expected = _mm_setr_ps(1., 2., 3., 4.);
        let r = _mm_cvtpu8_ps(a);
        assert_eq_m128(r, expected);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_cvtpi32x2_ps() {
        let a = _mm_setr_pi32(1, 2);
        let b = _mm_setr_pi32(3, 4);
        let expected = _mm_setr_ps(1., 2., 3., 4.);
        let r = _mm_cvtpi32x2_ps(a, b);
        assert_eq_m128(r, expected);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_maskmove_si64() {
        let a = _mm_set1_pi8(9);
        let mask = _mm_setr_pi8(0, 0, 0x80u8 as i8, 0, 0, 0, 0, 0);
        let mut r = _mm_set1_pi8(0);
        _mm_maskmove_si64(a, mask, &mut r as *mut _ as *mut i8);
        let e = _mm_setr_pi8(0, 0, 9, 0, 0, 0, 0, 0);
        assert_eq_m64(r, e);

        let mut r = _mm_set1_pi8(0);
        _m_maskmovq(a, mask, &mut r as *mut _ as *mut i8);
        assert_eq_m64(r, e);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_extract_pi16() {
        let a = _mm_setr_pi16(1, 2, 3, 4);
        let r = _mm_extract_pi16(a, 0);
        assert_eq!(r, 1);
        let r = _mm_extract_pi16(a, 1);
        assert_eq!(r, 2);

        let r = _m_pextrw(a, 1);
        assert_eq!(r, 2);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_insert_pi16() {
        let a = _mm_setr_pi16(1, 2, 3, 4);
        let r = _mm_insert_pi16(a, 0, 0b0);
        let expected = _mm_setr_pi16(0, 2, 3, 4);
        assert_eq_m64(r, expected);
        let r = _mm_insert_pi16(a, 0, 0b10);
        let expected = _mm_setr_pi16(1, 2, 0, 4);
        assert_eq_m64(r, expected);

        let r = _m_pinsrw(a, 0, 0b10);
        assert_eq_m64(r, expected);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_movemask_pi8() {
        let a =
            _mm_setr_pi16(0b1000_0000, 0b0100_0000, 0b1000_0000, 0b0100_0000);
        let r = _mm_movemask_pi8(a);
        assert_eq!(r, 0b10001);

        let r = _m_pmovmskb(a);
        assert_eq!(r, 0b10001);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_shuffle_pi16() {
        let a = _mm_setr_pi16(1, 2, 3, 4);
        let r = _mm_shuffle_pi16(a, 0b00_01_01_11);
        let expected = _mm_setr_pi16(4, 2, 2, 1);
        assert_eq_m64(r, expected);

        let r = _m_pshufw(a, 0b00_01_01_11);
        assert_eq_m64(r, expected);
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_cvtps_pi32() {
        let a = _mm_setr_ps(1.0, 2.0, 3.0, 4.0);
        let r = _mm_setr_pi32(1, 2);

        assert_eq_m64(r, _mm_cvtps_pi32(a));
        assert_eq_m64(r, _mm_cvt_ps2pi(a));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_cvttps_pi32() {
        let a = _mm_setr_ps(7.0, 2.0, 3.0, 4.0);
        let r = _mm_setr_pi32(7, 2);

        assert_eq_m64(r, _mm_cvttps_pi32(a));
        assert_eq_m64(r, _mm_cvtt_ps2pi(a));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_cvtps_pi16() {
        let a = _mm_setr_ps(7.0, 2.0, 3.0, 4.0);
        let r = _mm_setr_pi16(7, 2, 3, 4);
        assert_eq_m64(r, _mm_cvtps_pi16(a));
    }

    #[simd_test(enable = "sse,mmx")]
    unsafe fn test_mm_cvtps_pi8() {
        let a = _mm_setr_ps(7.0, 2.0, 3.0, 4.0);
        let r = _mm_setr_pi8(7, 2, 3, 4, 0, 0, 0, 0);
        assert_eq_m64(r, _mm_cvtps_pi8(a));
    }
}
