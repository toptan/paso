#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]) {
    default_random_engine index_year_engine(
        system_clock::now().time_since_epoch().count());
    default_random_engine index_number_engine(
        system_clock::now().time_since_epoch().count());

    uniform_int_distribution<int> index_year_distribution(1996, 2015);
    uniform_int_distribution<int> index_number_distribution(1, 600);

    auto index_year_dice = bind(index_year_distribution, index_year_engine);
    auto index_number_dice =
        bind(index_number_distribution, index_number_engine);

    for (auto i = 0; i < 50; i++) {
        auto index_year = index_year_dice();
        auto index_number = index_number_dice();
        cout << index_year << "/" << setfill('0') << setw(4) << index_number
             << endl;
    }

    return 0;
}
