#include "word_prediction.h"
#include <conio.h>

//----------------------------------------------------------------------------------------------
// Configurations
//----------------------------------------------------------------------------------------------
const string DICTIONARY_FILENAME = "words_alpha.txt";
const size_t MAX_SUGGESTIONS_TO_DISPLAY = 5;
const size_t MIN_INPUT_CHARS_FOR_SUGGESTIONS_TO_DISPLAY = 3;

//----------------------------------------------------------------------------------------------
// @name                    : DisplaySuggestedWords
//
// @description             : Displays the suggested words
//
// param @suggestedWords    : vector of strings of suggested words
//
// @returns                 : Nothing
//----------------------------------------------------------------------------------------------
void DisplaySuggestedWords(const map<int, string> & suggestedWords)
{
    
    if (suggestedWords.size() == 0)
    {
        return;
    }

    printf("\nSuggestions: ");
    for (auto it = suggestedWords.begin(); it != suggestedWords.end(); it++)
    {
        printf("%d) %s  ", it->first, it->second.c_str());
    }
    cout << endl;
}

string FindSuggestedStringFromIndex(const map<int, string> & suggestedWords, char indexChar)
{
    string result;
    int index = indexChar - '0';
    if (index < 0 || index > 9)
    {
        return result;
    }

    auto it = suggestedWords.find(index);
    if (it != suggestedWords.end())
    {
        return it->second;
    }

    return result;
}

//----------------------------------------------------------------------------------------------
// M A I N
//----------------------------------------------------------------------------------------------
int main()
{
    string resultString;
    WordPrediction wordPredictor(DICTIONARY_FILENAME, 
                                 MIN_INPUT_CHARS_FOR_SUGGESTIONS_TO_DISPLAY, 
                                 MAX_SUGGESTIONS_TO_DISPLAY);
    wordPredictor.Initialize();

    while (1 /* Continue indefinitely */)
    {
        printf("\nInput>> ");
        string inputString;
        map<int, string> suggestedWords;

        while (1 /* Continue till newline is received */)
        {
            // _getche() is used because:
            // 1) It echoes the character received on screen
            // 2) It does not require newline to store the input
            char ch = _getche();
            if (ch == '\r')
            {
                resultString = inputString;
                break;
            }

            // If entered value is a digit and we have a list of suggestions available,
            // then use this digit as selection for the suggested words.
            if (isdigit(ch))
            {
                resultString = FindSuggestedStringFromIndex(suggestedWords, ch);
                if (resultString == "")
                {
                    printf("\nIncorrect selection!\n");
                }
                else
                {
                    // We've found a valid suggestion.
                    break;
                }
            }

            // Form the string with this character appended to existing characters
            if (isalpha(ch))
            {
                inputString.push_back(ch);
                suggestedWords.clear();
            }

            // Find list of of auto-completion words as per dictionary. Clear the existing
            // suggestions first.
            suggestedWords = wordPredictor.SuggestWords(inputString);
            DisplaySuggestedWords(suggestedWords);

            // Displays the prompt and echo back the already entered characters, 
            // because the prompt now has moved because of preivous operations.
            if (suggestedWords.size())
            {
                printf("\nInput>> ");
                printf("%s", inputString.c_str());
            }
        }// Processed 1 word

        printf("\n** Final string: %s\n", resultString.c_str());
    }// Continuous input

    return 0;
}