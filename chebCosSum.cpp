#ifdef __AVX2__
#include <immintrin.h>
#endif

#ifndef size_t // Clang is weird
#include <cstdlib>
#endif

void chebCosSum(size_t size, const double* inputCoeffs, double* outputCoeffs) {
	for (size_t i = 0; i < size; i++) {
		outputCoeffs[i] = 0;
	}
	outputCoeffs[0] = inputCoeffs[0];
	outputCoeffs[1] = inputCoeffs[1];

	size_t buf_size = (size + 3) & ~3; // Rounding up size to the next multiple of 4 (32 / sizeof(double))
#ifdef _MSC_VER //msvc specific aligned alloc
	alignas(32) double* buffer = (double*)_aligned_malloc(buf_size * 3 * sizeof(double), 32);
#else
	alignas(32) double* buffer = (double*)aligned_alloc(32, buf_size * 3 * sizeof(double));
#endif
	alignas(32) double* prevprevC = buffer;
	alignas(32) double* prevC = &buffer[buf_size];
	alignas(32) double* C = &buffer[buf_size * 2];
	for (size_t i = 0; i < buf_size * 3; i++) {
		buffer[i] = 0.0;
	}

	prevprevC[0] = 1.0;
	prevC[1] = 1.0;

	for (size_t i = 2; i < size; i++) {
		// Making Chebyshev calculations step
		size_t j = 1;

#ifdef __AVX2__ // AVX2 + FMA implementation (FMA is assumed to be supported with AVX2)
		if (size >= 4) {
			for (; j <= size - 4; j += 4) {
				__m256d prevC_vec = _mm256_load_pd(&prevC[j - 1]);
				__m256d prevprevC_vec = _mm256_loadu_pd(&prevprevC[j]);
				__m256d two_vec = _mm256_set1_pd(2.0);
				// Perform the Chebyshev calculation: C[j] = 2 * prevC[j-1] - prevprevC[j]
				__m256d result = _mm256_fmadd_pd(
						two_vec,
						prevC_vec,
						_mm256_sub_pd(_mm256_setzero_pd(), prevprevC_vec));
				_mm256_storeu_pd(&C[j], result);
			}
		}
#endif

		// Fallback (scalar) implementation for remaining elements or when AVX2 is not enabled
		for (; j < size; j++) {
			C[j] = prevC[j - 1] * 2.0 - prevprevC[j];
		}
		C[0] = -prevprevC[0];

		j = 0;

#ifdef __AVX2__ // AVX2 + FMA implementation (FMA is assumed to be supported with AVX2)
		if (size >= 4) {
			__m256d output_vec = _mm256_loadu_pd(&outputCoeffs[j]);  // Load once before the loop
			for (; j <= size - 4; j += 4) {
				__m256d C_vec = _mm256_load_pd(&C[j]);
				__m256d input_vec = _mm256_set1_pd(inputCoeffs[i]);
				// outputCoeffs[j] += C[j] * inputCoeffs[i]
				output_vec = _mm256_fmadd_pd(C_vec, input_vec, output_vec);
				_mm256_storeu_pd(&outputCoeffs[j], output_vec);
				output_vec = _mm256_setzero_pd();
			}
		}
#endif

		// Fallback (scalar) implementation for remaining elements or when AVX2 is not enabled
		for (; j < size; j++) {
			outputCoeffs[j] += C[j] * inputCoeffs[i];
		}

		// Revolve buffer
		double* temp = prevprevC;
		prevprevC = prevC;
		prevC = C;
		C = temp;
	}
#ifdef _MSC_VER //msvc specific aligned alloc
	_aligned_free(buffer);
#else
	free(buffer);
#endif
	return;
}