#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Upotreba:" << endl;
        cout << "\t" << argv[0] << " <csv_datoteka_sa_studentima>" << endl;
        cout << endl;
        return 1;
    }
    string line;
    ifstream input_file(argv[1]);
    if (!input_file.is_open()) {
        cout << "Nije bilo moguće otvoriti " << argv[1] << endl;
        return 2;
    }
    vector<string> index_numbers;

    while (getline(input_file, line)) {
        index_numbers.push_back(line.substr(0, 9));
    }
    input_file.close();

    default_random_engine engine(
        system_clock::now().time_since_epoch().count());

    uniform_int_distribution<int> distribution(0, index_numbers.size() - 1);

    auto dice = bind(distribution, engine);

    for (auto i = 0; i < 50; i++) {
        cout << index_numbers[dice()] << endl;
    }

    return 0;
}
