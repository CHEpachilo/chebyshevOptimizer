void chebCosSum (int size, const double* inputCoeffs, double* outputCoeffs) {
	for (int i = 0; i < size; i++) {
		outputCoeffs[i] = 0;
	}
	double* prevprevC = new double [size]();
	double* prevC = new double [size]();
	double* C = new double [size]();

	prevprevC[0] = 1.;
	prevC[1] = 1.;
	outputCoeffs[0] = inputCoeffs[0];
	outputCoeffs[1] = inputCoeffs[1];

	for (int i = 2; i < size; i++) {
		for (int j = 1; j < size; j++) {
			C[j] = prevC[j - 1] * 2. - prevprevC[j];
		}
		C[0] = -prevprevC[0];
		for (int j = 0; j < size; j++) {
			outputCoeffs [j] += C[j] * inputCoeffs[i];
			prevprevC[j] = prevC[j];
			prevC[j] = C[j];
		}
	}

	delete[] prevprevC;
	delete[] prevC;
	delete[] C;
	return;
}
