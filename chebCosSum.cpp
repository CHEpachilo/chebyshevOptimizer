void chebCosSum (int size, const double* inputCoeffs, double* outputCoeffs) {
	for (int i = 0; i < size; i++) {
		outputCoeffs[i] = 0;
	}
	double* buffer = new double [size * 3]();
	double* prevprevC = buffer;
	double* prevC = &buffer[size];
	double* C = &buffer[size * 2];
	
	prevprevC[0] = 1.;
	prevC[1] = 1.;
	outputCoeffs[0] = inputCoeffs[0];
	outputCoeffs[1] = inputCoeffs[1];

	for (int i = 2; i < size; i++) {
		// Making Chebyshev calculations step
		// T[n+1](x) = 2x*T[n](x) - T[n-1](x)
		for (int j = 1; j < size; j++) {
			C[j] = prevC[j - 1] * 2. - prevprevC[j];
		}
		C[0] = -prevprevC[0];

		for (int j = 0; j < size; j++) {
			outputCoeffs [j] += C[j] * inputCoeffs[i];
		}
		//revolve buffer
		double* temp = prevprevC;
		prevprevC = prevC;
		prevC = C;
		C = temp;
	}

	delete [] buffer;
	return;
}
