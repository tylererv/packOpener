#include "PackDatabase.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

static vector<string> splitLine(const string &line, char delimiter) {
  vector<string> parts;
  string part;
  stringstream lineStream(line);
  while (getline(lineStream, part, delimiter)) {
    parts.push_back(part);
  }
  return parts;
}
unordered_map<Rarity, int> parseOdds(const string &oddsText) {
  unordered_map<Rarity, int> rarityOdds;
  const vector<string> oddsParts = splitLine(oddsText, ',');
  for (const string &oddsPart : oddsParts) {
    const vector<string> keyValue = splitLine(oddsPart, ':');
    if (keyValue.size() == 2) {
      rarityOdds[rarityFromString(keyValue[0])] = stoi(keyValue[1]);
    }
  }
  return rarityOdds;
}

bool PackDatabase::loadFromFile(const string &filePath) {
  packs.clear();

  ifstream inputFile(filePath);
  if (!inputFile) {
    cerr << "Could not open pack data: " << filePath << '\n';
    return false;
  }

  string line;
  int lineNumber = 0;
  while (getline(inputFile, line)) {
    ++lineNumber;
    if (line.empty() || line[0] == '#') {
      continue;
    }

    try {
      const vector<string> fields = splitLine(line, '|');
      if (fields.size() < 6) {
        cerr << "Skipping malformed pack row " << lineNumber << '\n';
        continue;
      }

      packs.emplace_back(fields[0], fields[1], stoi(fields[2]), stoi(fields[3]),
                         parseOdds(fields[4]), splitLine(fields[5], ','));
    } catch (const exception &error) {
      cerr << "Skipping pack row " << lineNumber << ": " << error.what()
           << '\n';
    }
  }

  return !packs.empty();
}

const Pack *PackDatabase::findPack(const string &packId) const {
  for (const Pack &pack : packs) {
    if (pack.getId() == packId) {
      return &pack;
    }
  }
  return nullptr;
}

const vector<Pack> &PackDatabase::getPacks() const { return packs; }

bool PackDatabase::isLoaded() const { return !packs.empty(); }
