#include "cities.h"
#include <algorithm>
#include <fstream>

using std::ifstream;

bool Cities::findCity(const string_v& cities, string cityName)
{
    string ctStr(cityName);
    transform(ctStr.begin(), ctStr.end(), ctStr.begin(), ::tolower);
    return binary_search(cities.begin(),cities.end(),ctStr);
}


string Cities::getRandomCity(const string_v& cities)
{
    int randomIndex = rand() % cities.size() - 1;
    return cities.at(randomIndex);
}

string_v Cities::byFirstChar(const string_v &cities, char ch)
{
    string_v result;
    ch = tolower(ch);

    auto pos = find_if(cities.begin(),
                       cities.end(),
                       [&ch](const string ct)
                       { return ct.at(0) == ch;}
    );
    if (pos != cities.end())
    {
        while(pos->at(0) == ch)
        {
            result.push_back(*pos);
            pos++;
        }
    }
    return result;
}

string_v Cities::byLastChar(const string_v& cities, char ch)
{
    string_v result;
    ch = tolower(ch);
    auto pos = cities.begin();

    while (pos != cities.end())
    {
        pos = find_if(pos,
                      cities.end(),
                      [&ch](const string ct)
                      { return ct.back() == ch;});

        if (pos != cities.end())
            result.push_back(*pos);
        else
            break;

        pos++;
    }

    return result;
}


/*ostream& operator <<(ostream& out, Cities& ct)
{
    for(auto& city : ct.cities)
        out << city << endl;

    return out;
}*/

bool Cities::initData(const string &dataFile, string_v &cities)
{

    if (!cities.empty())
        cities.clear();

    ifstream infile(dataFile);

    if(!infile.is_open())
        return false;

    string line;

    while (std::getline(infile, line))
    {
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);
        cities.push_back(line);
    }

    return true;
}

void Cities::citySort(string_v& cities)
{
    std::sort(cities.begin(),cities.end());
}

