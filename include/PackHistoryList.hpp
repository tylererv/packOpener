#pragma once

#include <string>
#include <vector>

class PackHistoryList
{
public:
    PackHistoryList();
    PackHistoryList(const PackHistoryList& other);
    PackHistoryList& operator=(const PackHistoryList& other);
    ~PackHistoryList();

    void addEntry(const std::string& packName);
    std::vector<std::string> getRecentEntries(int maxEntries = 6) const;
    void clear();

private:
    struct HistoryNode
    {
        char* packName;
        HistoryNode* next;
    };

    void copyFrom(const PackHistoryList& other);
    static char* copyText(const std::string& text);

    HistoryNode* head;
};
