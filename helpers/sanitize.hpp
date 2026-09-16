#include <string>
std::string sanitize_number(const std::string input)
{
    unsigned int pos = 0;
    std::string result = "";
    while(pos <= input.size() && ((input[pos] >= '0' && input[pos]<='9') || input[pos] =='.' || input[pos] =='-'))
    {
        result += input[pos];
        pos++;
    }
    return result;
}
