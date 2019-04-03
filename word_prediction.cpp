#include "word_prediction.h"
#include<fstream>

//----------------------------------------------------------------------------------------------
// @name                            : WordPrediction
//
// @description                     : Constructor
//
// param @dictFileName              : Dictionary file to be used for fetching word list 
// param @minCharsForSuggestion     : Minimum characters required so as to suggest words for completion
// param @maxSuggestions            : Maximum number of words to suggest. 
//
// @returns                         : Nothing
//----------------------------------------------------------------------------------------------
WordPrediction::WordPrediction(const string & dictFileName, size_t minCharsForSuggestion, size_t maxSuggestions)
{
    m_dictFilename = dictFileName;
    m_maxSuggestions = maxSuggestions;
    m_minCharsForSuggestion = minCharsForSuggestion;
    m_dictionaryLoaded = false;
}

//----------------------------------------------------------------------------------------------
// @name                            : WordPrediction
//
// @description                     : Destructor
//
// @returns                         : Nothing
//----------------------------------------------------------------------------------------------
WordPrediction::~WordPrediction()
{

}

//----------------------------------------------------------------------------------------------
// @name                            : Initialize
//
// @description                     : This function loads the provided Dictionary file
//
// @returns                         : true if system initialized successfully, else false
//----------------------------------------------------------------------------------------------
bool WordPrediction::Initialize()
{
    printf("Loading Dictionary [ %s ]...", m_dictFilename.c_str());
    size_t wordsLoaded = LoadDictionaryFile();
    if (wordsLoaded)
    {
        m_dictionaryLoaded = true;
        printf("[ DONE ]\n");
        printf("Found %ld words in dictionary\n", wordsLoaded);
        printf("Min input characters for suggestion : %ld\n", m_minCharsForSuggestion);
        printf("Max suggestions                     : %ld\n", m_maxSuggestions);
        printf("\n");
    }
    else
    {
        printf("[ FAILED ]\n");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------
// @name                            : LoadDictionaryFile
//
// @description                     : Opens and reads the dictionary file. Stores the entries
//                                    in an unordered_set. 
//
// @returns                         : Count of words loaded from the dictionary file
//----------------------------------------------------------------------------------------------
size_t WordPrediction::LoadDictionaryFile()
{
    ifstream fileStream;
    fileStream.open(m_dictFilename, ios::in);
    if (!fileStream)
    {
        printf("File [ %s ] NOT found!\n", m_dictFilename.c_str());
        return false;
    }

    // Read the file
    std::string line;
    long int lineNumber = 0;
    while (getline(fileStream, line))
    {
        m_dictMap.insert(line);
        lineNumber++;
    }

    // Close the file
    fileStream.close();

    return m_dictMap.size();
}

//----------------------------------------------------------------------------------------------
// @name                            : SuggestWords
//
// @description                     : This function compares the characters in the input word
//                                    with that of words in the dictionary. Using the comparison
//                                    probable completion words are stored in a vector.
//
// param @word                      : Word for which suggestions have to be found.
//
// @returns                         : A vector of suggested words.
//----------------------------------------------------------------------------------------------
map<int, string> WordPrediction::SuggestWords(const string & word)
{
    map<int, string> suggestedWordsMap;
    int suggestionIndex = 1;

    // If dictionary is not loaded, return.
    if (m_dictionaryLoaded == false)
    {
        printf("Dictionary not initialized! System cannot provide suggestions.\n");
        return suggestedWordsMap;
    }

    // Check if we have required number of characters in the word to provide suggestion.
    if (word.size() < m_minCharsForSuggestion)
    {
        return suggestedWordsMap;
    }

    //printf("\nSuggestions for %s?\n", word.c_str());
    
    for (auto it = m_dictMap.begin(); it != m_dictMap.end(); it++)
    {
        string dictWord = *it;
        if (0 == dictWord.compare(0, word.size(), word))
        {
            suggestedWordsMap[suggestionIndex] = dictWord;
            suggestionIndex++;
        }

        // If we already have maximum suggestions as per configuration, do not
        // look for more.
        if (suggestedWordsMap.size() >= m_maxSuggestions)
        {
            break;
        }
    }

    return suggestedWordsMap;
}