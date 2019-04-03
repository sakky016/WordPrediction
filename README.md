# WordPrediction
Provides functionality for word auto completion using a specified dictionary. The autocomplete functionality is separate thread that monitors the characters entered. Once some characters are entered (configurable by MIN_INPUT_CHARS_FOR_SUGGESTIONS_TO_DISPLAY) and certain time has elapsed (configurable by PREDICTION_WAIT_DURATION_MILLISEC), the suggestions are displayed on the console. The number of suggestions that should be displayed is configurable by MAX_SUGGESTIONS_TO_DISPLAY. The prediction wait duration makes sure that if a user is typing known characters, he is not bugged by suggestions continuously.  

Once a suggestion is displayed, user can choose the desired word using number index. As soon as the number is entered, the corresponding word is saved to resultString.

Refer console_logs.txt for summary of a sample execution.
