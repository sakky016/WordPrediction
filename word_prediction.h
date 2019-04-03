#ifndef _WORD_PREDICTION_H_
#define _WORD_PREDICTION_H_

#include<stdio.h>
#include<iostream>
#include<map>
#include<string>
#include<unordered_set>
#include<vector>

using namespace std;

//-----------------------------------------------------------------------------
// Word Prediction Class
//-----------------------------------------------------------------------------
class WordPrediction
{
private:
    unordered_set<string>            m_dictMap;
    size_t                           m_maxSuggestions;
    size_t                           m_minCharsForSuggestion;
    string                           m_dictFilename;
    bool                             m_dictionaryLoaded;

public:
    WordPrediction(const string & dictFileName, size_t minCharsForSuggestion, size_t maxSuggestions);
    ~WordPrediction();
    bool Initialize();
    size_t LoadDictionaryFile();
    map<int, string> SuggestWords(const string & word);
};

#endif
