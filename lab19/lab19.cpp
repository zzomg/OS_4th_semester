#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

void lookUpPattern(string pattern)
{
    DIR *dir;
    struct dirent *dirp;

    map<string, int> matches;
    map<string, int> entries;
    // string = name, int = seek_pos

    if ((dir = opendir(".")) == NULL) {
        perror("Couldn't open '.'");
        return;
    }

    // read up all files in the current directory 
    dirp = readdir(dir);
    while (dirp != NULL)
    {
        entries[dirp->d_name] = 0;
        dirp = readdir(dir);
    }

    // cout << "-----------------------------" << endl;
    // for(auto entry : entries)
    // {
    //     cout << "Entry: "<< entry.first << endl;
    //     cout << "Seek_pos: " << entry.second << endl;
    // }
    
    bool star = false;
    char c;
    int pattern_len = pattern.length();

    for (int i = 0; i < pattern_len; ++i)
    {
        string word;
        int seek_pos;

        c = pattern[i];
        // cout << "-----------------------------" << endl;
        // cout << "pattern:" << c << endl;
        // cout << "Star: " << star << endl;

        switch (c)
        {
        case '*':
            star = true;
            for (auto& entry: entries)
            {
                word = entry.first;
                seek_pos = entry.second;

                matches[word] = seek_pos;
            }
            break;

        case '?':
            for (auto& entry: entries)
            {
                word = entry.first;
                seek_pos = entry.second;

                if (i == pattern_len - 1)
                {
                    if (word.length() == seek_pos + 1) 
                    {
                        matches[word] = 0;
                        // just end it here, we no longer need seeking position
                    }
                }
                else 
                {
                    if (word.length() > seek_pos)
                    {
                        matches[word] = seek_pos + 1;
                    }
                }
            }
            if (star)
                star = false;
            break;

        default:
            for (auto& entry : entries) 
            {
                word = entry.first;
                seek_pos = entry.second;

                if (star) 
                {
                   int found;

                    for (found = seek_pos; found < word.length(); ++found) 
                    {
                        if (word[found] == c)
                        {
                            break;
                        }
                    }

                    if (found < word.length()) 
                    {
                        matches[word] = found + 1; // new seek_pos
                    }
                }
                else
                {
                    if (i == pattern_len - 1)
                    {
                        if(word[seek_pos] == c && word.length() == seek_pos + 1)
                        {
                            matches[word] = seek_pos + 1;
                        }
                        
                    }
                    else 
                    {
                        if(word[seek_pos] == c )
                        {
                            matches[word] = seek_pos + 1;
                        }
                    }
                }
            }
            star = false;
            break;
        }

        entries.clear();
        for (auto match : matches)
        {
            entries[match.first] = match.second;
        }
        matches.clear();

        // for(auto entry : entries)
        // {
        //     cout << "Entry: "<< entry.first << endl;
        //     cout << "Seek_pos: " << entry.second << endl;
        // }
    }

    cout << "Matches found:" << endl;
    cout << "\n";

    for(auto entry : entries)
    {
        cout << entry.first << endl;
    }
}

int main(int argc, char* argv[])
{
    string pattern;

    cout << "Enter desirable pattern:" << endl;
    cin >> pattern;

    lookUpPattern(pattern);

    return 0;
}
