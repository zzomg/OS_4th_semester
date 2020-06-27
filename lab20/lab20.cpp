#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

bool checkIfDir(string filename)
{
    struct stat stbuf;

    if (stat(filename.c_str(), &stbuf) == -1) {
        perror("Can not obtain file information");
        exit(1);
    }

    switch(stbuf.st_mode & S_IFMT) {
        case S_IFDIR:
            return true;
        default:
            return false;
    }
}

vector<string> lookUpPattern(string dir_name, string pattern)
{
    DIR *dir;
    struct dirent *dirp;

    map<string, int> matches;
    map<string, int> entries;
    // string = name, int = seek_pos

    vector<string> matching_entries;

    if ((dir = opendir(dir_name.c_str())) == NULL) {
        cout << "Couldn't open " <<  dir_name << endl;
        exit(1);
    }

    // read up all files in the current directory
    dirp = readdir(dir);
    while (dirp != NULL)
    {
        if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
        {
            entries[dirp->d_name] = 0;
        }
        dirp = readdir(dir);
    }

    bool star = false;
    char c;
    int pattern_len = pattern.length();

    for (int i = 0; i < pattern_len; ++i)
    {
        string word;
        int seek_pos;

        c = pattern[i];

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
                        if(word[seek_pos] == c)
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
    }

    for(auto entry : entries)
    {
        matching_entries.push_back(entry.first);
    }

    return matching_entries;
}

string parsePattern(string patt, vector<string>& patterns)
{
    string delimiter = "/";
    string start_dir;

    int start = 0;
    int end = patt.find(delimiter);
    if(end != 0) // relative path
    {
        start_dir = "."; // starts searching from current directory
    }
    else // absolute path
    {
        start_dir = "/";
    }
    string token;

    while (end != string::npos)
    {
        token = patt.substr(start, end - start);
        if(!token.empty())
        {
            patterns.push_back(token);
        }
        start = end + delimiter.length();
        end = patt.find(delimiter, start);
    }

    token = patt.substr(start, end - start);
    if(!token.empty())
    {
        patterns.push_back(token);
    }

    return start_dir;
}

int count = 0;

void lookUpDirs(string dir, vector<string>& patterns, int max_count)
{
    vector<string> matches;
    if (checkIfDir(dir))
    {
        matches = lookUpPattern(dir, patterns[count]);
    }

    for (string match : matches) {
        string l_dir = dir + "/" + match;
        ++count;
        if(count != max_count)
        {
            lookUpDirs(l_dir, patterns, max_count);
        }
        else
        {
            cout << l_dir.substr(1) << endl;
        }
        --count;
    }
}

int main(int argc, char* argv[])
{
    string pattern;
    vector<string> patterns;
    vector<string> matches;

    string dir;

    cout << "Enter desirable pattern:" << endl;
    cin >> pattern;

    dir = parsePattern(pattern, patterns);
    int max_count = patterns.size();

    lookUpDirs(dir, patterns, max_count);

    return 0;
}
