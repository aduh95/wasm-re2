#include "../../basement/re2.h"
#include "../../basement/filtered_re2.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>

extern "C"
{
    char *getStringPtr(const std::string &inputString)
    {
        char *stringPtr = new char[inputString.length() + 1];
        strcpy(stringPtr, inputString.c_str());
        return stringPtr;
    }

    int getQtyOfCapturingGroups(char *inputRegex)
    {
        return re2::RE2(inputRegex).NumberOfCapturingGroups() + 1;
    }

    int getQtyOfMatchedGroups(char *inputString, char *inputRegex, char *flag)
    {
        if (*flag != 'g')
            return 1;
        re2::StringPiece sp(inputString);
        re2::RE2 regex(inputRegex);

        int n = getQtyOfCapturingGroups(inputRegex);
        std::vector<re2::StringPiece> groups(n);

        int count = 0,
            lastIndex = 0;

        while (regex.Match(sp, lastIndex, sp.size(), re2::RE2::UNANCHORED, &(groups[0]), groups.size()))
        {
            lastIndex += groups[0].data() - sp.data() + groups[0].size() - lastIndex;
            count++;
        }

        return count;
    }

    char *getStringPtrFromMatrix(char ***stringArray, int raw, int columns)
    {
        return stringArray[raw][columns];
    }

    bool check(char *text, char *regex)
    {
        return re2::RE2::PartialMatch(text, regex);
    }

    char *replace(char *text, char *regex, char *rewrite, char *flag)
    {
        std::string replacedString = text;
        if (*flag == 'g')
            re2::RE2::GlobalReplace(&replacedString, regex, rewrite);
        else
            re2::RE2::Replace(&replacedString, regex, rewrite);
        return getStringPtr(replacedString);
    }

    void *validate(char *regexp)
    {
        re2::RE2 regex(regexp);
        return regex.ok() ? nullptr : getStringPtr(regex.error());
    }

    void *exec(char *inputString, char *inputRegex, char *flag)
    {
        re2::StringPiece sp(inputString);
        re2::RE2 regex(inputRegex);

        int raw = getQtyOfCapturingGroups(inputRegex);
        std::vector<re2::StringPiece> groups(raw);

        if (!check(inputString, inputRegex))
            return nullptr;

        int columns = getQtyOfMatchedGroups(inputString, inputRegex, flag);
        char ***result = new char **[columns];
        int count = 0,
            lastIndex = 0;

        while (regex.Match(sp, lastIndex, sp.size(), re2::RE2::UNANCHORED, &(groups[0]), groups.size()))
        {
            result[count] = new char *[raw];
            for (int i = 0; i < raw; ++i)
            {
                const size_t size = groups[i].size();
                result[count][i] = new char[size + 1];
                groups[i].copy(result[count][i], size);
                result[count][i][size] = '\0';
            }

            if (*flag != 'g')
                break;

            lastIndex += groups[0].data() - sp.data() + groups[0].size() - lastIndex;
            count++;
        }
        return result;
    }
}
