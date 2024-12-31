#include <iostream>
#include <cstdlib>

#include <cmath>
#include <numbers>
constexpr double pi = std::numbers::pi;

#include <chrono>
#define now() std::chrono::steady_clock::now()

#include "..\chebCosSum.cpp"
#define sample_count 192000 * 2 * 60 * 10 //10 minutes, 2 channels, 192000 samplerate

inline double BH4 (double x) {
	// https://www.mathworks.com/matlabcentral/mlc-downloads/downloads/submissions/46092/versions/3/previews/coswin.m/index.html
	// % 4 Term Blackman-Harris window, 92 dB
	return 0.358750287312166
		- 0.4882901074726 * std::cos(2.0 * pi * x)
		+ 0.141279712970519 * std::cos(4.0 * pi * x)
		- 0.011679892244715 * std::cos(6.0 * pi * x);
}

inline double optimizedBH4 (double x) {
	const double c = std::cos(2.0 * pi * x);
	return (((0.0467195689788600030
				* c + 0.2825594259410380005)
				* c + 0.4532504307384550013)
				* c + 0.2174705743416470161);
}

int main (int argc, char ** argv) {
	if (!argc) {
		std::cout << "Hello world!" << std::endl;
	}
	else if (argc == 1) {
		std::cout << "Starting classic BH4 benchmark:" << std::endl << "(est. time about 1 min)" << std::endl;
		auto start = now();
		for (double i = 0.0; i < sample_count; i++) {
			BH4(i);
		}
		auto end = now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		std::cout << elapsed_seconds << std::endl;
		std::cout << "Starting optimized BH4 benchmark:" << std::endl << "(est. time about 1 min)" << std::endl;
		start = now();
		for (double i = 0.0; i < sample_count; i++) {
			optimizedBH4(i);
		}
		end = now();
		elapsed_seconds = end - start;
		std::cout << elapsed_seconds << std::endl;
	}
	else {
		std::cout << argc << std::endl;
		for (int i = 0; i < argc; i++)
			std::cout << argv[i] << std::endl;
	}
	std::system("Pause");
	return 0;
}
