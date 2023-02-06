#include <fstream>
#include <iostream>

#include <string>
#include <sstream>
#include <filesystem>
#include <functional>


namespace fs = std::filesystem;

inline std::string strip_edge(const std::string src, char to_strip)
{
    int index1 = 0, index2 = src.length();

    for (; index1 < src.length(); index1++)
        if (src[index1] != to_strip)
            break;

    for (; index2 < src.length(); --index2)
        if (src[index2] != to_strip)
            break;

    return src.substr(index1, (index2 - index1 - 1));
}

inline std::string replace_first_in(const std::string& src, const std::string to_replace, const std::string with)
{
    int lent = to_replace.length(); 

    for (int i = 0; i < src.length(); i++)
    {
        if (src.substr(i, lent) == to_replace)
        {
            return src.substr(0, i) + with + src.substr(i + lent, src.length() - (i + lent));
        }
    }
    return src;
}


inline std::string replace_all_in(const std::string& src, const std::string to_replace, const std::string with )
{
    std::string hold = src;

    auto h = replace_first_in(hold, to_replace, with);
    if (h != hold)
    {
        hold = h;
        hold = replace_all_in(hold, to_replace, with);
    }
    return hold;
}


template <class Type>
inline std::string to_string(const Type v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}


/*

int main()
{

    fs::current_path(fs::temp_directory_path());
    fs::create_directories("sandbox/a/b");
    std::ofstream("sandbox/file1.txt");
    fs::create_symlink("a", "sandbox/syma");

    // Iterate over the `std::filesystem::directory_entry` elements explicitly
    for (const fs::directory_entry& dir_entry :
        fs::recursive_directory_iterator("sandbox"))
    {
        std::cout << dir_entry << '\n';
    }
    std::cout << "-----------------------------\n";
    // Iterate over the `std::filesystem::directory_entry` elements using `auto`
    for (auto const& dir_entry : fs::recursive_directory_iterator("sandbox"))
    {
        std::cout << dir_entry << '\n';
    }

    fs::remove_all("sandbox");
}*/