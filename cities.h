#ifndef CITIES_H
#define CITIES_H

#include "types.h"

#define CITY_DATA_FILE "D:\\Dropbox\\Projects\\cities_socket_gl\\Debug\\cities.dat"


// namespace to operate with cities game logic
namespace Cities
{
    bool findCity(const string_v& cities, string cityName);
    string getRandomCity(const string_v& cities);

    string_v byFirstChar(const string_v& cities, char ch);
    string_v byLastChar(const string_v& cities, char ch);

    bool initData(const string& dataFile, string_v& cities);
    void citySort(string_v& cities);
};

#endif // CITIES_H
