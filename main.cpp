#include <condition_variable>
#include <conio.h>
#include <mutex>
#include<thread>
#include<Windows.h>
#include "word_prediction.h"

//----------------------------------------------------------------------------------------------
// Configurations
//----------------------------------------------------------------------------------------------
const string DICTIONARY_FILENAME                        = "words_alpha.txt";
const size_t MAX_SUGGESTIONS_TO_DISPLAY                 = 5;
const size_t MIN_INPUT_CHARS_FOR_SUGGESTIONS_TO_DISPLAY = 3;
const int    PREDICTION_WAIT_DURATION_MILLISEC          = 500;

//----------------------------------------------------------------------------------------------
// Globals used by Word Predictor thread
//----------------------------------------------------------------------------------------------
map<int, string> g_suggestedWords;
string g_inputString;
bool g_doPrediction = false;
bool g_startInputPrompt = false;
mutex g_mtx;
long long g_lastEnteredTimestamp;


long long getCurrentTimestampInMilliseconds()
{
    long long ts_us = chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::
                      now().time_since_epoch()).count();
    return ts_us;
}

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

//----------------------------------------------------------------------------------------------
// @name                    : WordPredictionThread
//
// @description             : This thread displays suggestion when triggered.
//
// @returns                 : Nothing
//----------------------------------------------------------------------------------------------
void WordPredictionThread()
{
    WordPrediction wordPredictor(DICTIONARY_FILENAME,
                                 MIN_INPUT_CHARS_FOR_SUGGESTIONS_TO_DISPLAY,
                                 MAX_SUGGESTIONS_TO_DISPLAY);

    // Initialize the module to load words from the dictionary.
    if (wordPredictor.Initialize())
    {
        g_startInputPrompt = true;
    }

    // Loop continues indefinitely
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        long long currentTimestamp = getCurrentTimestampInMilliseconds();

        if (g_doPrediction == true &&
            g_inputString.size() > 0 &&
            currentTimestamp - g_lastEnteredTimestamp >= PREDICTION_WAIT_DURATION_MILLISEC)
        {
            // Find list of of auto-completion words as per dictionary. Clear the existing
            // suggestions first.
            g_mtx.lock();
            g_suggestedWords = wordPredictor.SuggestWords(g_inputString);
            DisplaySuggestedWords(g_suggestedWords);
            g_doPrediction = false;
            g_mtx.unlock();

            // Displays the prompt and echo back the already entered characters, 
            // because the prompt now has moved because of preivous operations.
            if (g_suggestedWords.size())
            {
                printf("\nInput>> ");
                printf("%s", g_inputString.c_str());
            }
        }
    }
}

//----------------------------------------------------------------------------------------------
// @name                    : FindSuggestedStringFromIndex
//
// @description             : Fetches the suggested word as per the index specified by user.
//
// param @suggestedWords    : vector of strings of suggested words
// param @indexChar         : Choice entered by user
//
// @returns                 : string corresonding to indexChar
//----------------------------------------------------------------------------------------------
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

    // Spawn thread for word prediction
    thread WordPredictionThreadId = thread(WordPredictionThread);

    while (!g_startInputPrompt)
    {
        // Wait till Word Predicttor has initialized
        Sleep(100);
    }

    // Main routing which processes fetching of input from user
    while (1 /* Continue indefinitely */)
    {
        printf("\nInput>> ");

        while (1 /* Continue till newline is received */)
        {
            // _getche() is used because:
            // 1) It echoes the character received on screen
            // 2) It does not require newline to store the input
            char ch = _getche();
            
            // Record the timestamp of entry of character
            g_lastEnteredTimestamp = getCurrentTimestampInMilliseconds();

            if (ch == '\r')
            {
               // g_mtx.lock();
                g_doPrediction = false;
                resultString = g_inputString;
                g_inputString.clear();
                g_suggestedWords.clear();
               // g_mtx.unlock();
                break;
            }

            // If entered value is a digit and we have a list of suggestions available,
            // then use this digit as selection for the suggested words.
            if (isdigit(ch))
            {
                resultString = FindSuggestedStringFromIndex(g_suggestedWords, ch);
                if (resultString == "")
                {
                    printf("\nIncorrect selection!\n");
                }
                else
                {
                    // We've found a valid suggestion.
                    g_inputString.clear();
                    break;
                }
            }

            // Form the string with this character appended to existing characters
            if (isalpha(ch))
            {
                g_inputString.push_back(ch);
                g_suggestedWords.clear();
            }

            // Notify the thread that a string is ready for prediction
            g_doPrediction = true;

        }// Processed 1 word

        printf("\n** Final string: %s\n", resultString.c_str());
    }// Continuous input

    // Wait for joining
    WordPredictionThreadId.join();

    return 0;
}