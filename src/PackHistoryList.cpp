#include "PackHistoryList.hpp"

#include <cstring>

PackHistoryList::PackHistoryList()
    : head(nullptr)
{
}

PackHistoryList::PackHistoryList(const PackHistoryList& other)
    : head(nullptr)
{
    copyFrom(other);
}

PackHistoryList& PackHistoryList::operator=(const PackHistoryList& other)
{
    if (this != &other)
    {
        clear();
        copyFrom(other);
    }
    return *this;
}

PackHistoryList::~PackHistoryList()
{
    clear();
}

void PackHistoryList::addEntry(const std::string& packName)
{
    HistoryNode* node = new HistoryNode{copyText(packName), head};
    head = node;
}

std::vector<std::string> PackHistoryList::getRecentEntries(int maxEntries) const
{
    std::vector<std::string> entries;
    const HistoryNode* currentNode = head;
    while (currentNode != nullptr && static_cast<int>(entries.size()) < maxEntries)
    {
        entries.emplace_back(currentNode->packName);
        currentNode = currentNode->next;
    }
    return entries;
}

void PackHistoryList::clear()
{
    HistoryNode* currentNode = head;
    while (currentNode != nullptr)
    {
        HistoryNode* nextNode = currentNode->next;
        delete[] currentNode->packName;
        delete currentNode;
        currentNode = nextNode;
    }
    head = nullptr;
}

void PackHistoryList::copyFrom(const PackHistoryList& other)
{
    std::vector<std::string> entries = other.getRecentEntries(1000);
    for (auto entryIterator = entries.rbegin(); entryIterator != entries.rend(); ++entryIterator)
    {
        addEntry(*entryIterator);
    }
}

char* PackHistoryList::copyText(const std::string& text)
{
    char* copiedText = new char[text.size() + 1];
    std::strcpy(copiedText, text.c_str());
    return copiedText;
}
