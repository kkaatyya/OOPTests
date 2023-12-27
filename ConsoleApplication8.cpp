
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

using namespace std;

class Answer
{
public:
    Answer(char option, const string& text, bool isCorrect) : option(option), text(text), isCorrect(isCorrect) {}

    char getOption() const
    {
        return option;
    }

    const string& getText() const
    {
        return text;
    }

    bool isCorrectAnswer() const
    {
        return isCorrect;
    }

private:
    char option;
    string text;
    bool isCorrect;
};

class Question
{
public:
    Question(const string& questionText) : questionText(questionText) {}

    const string& getQuestionText() const
    {
        return questionText;
    }

    virtual void addAnswer(char option, const string& text, bool isCorrect)
    {
        answers.push_back(Answer(option, text, isCorrect));
    }

    const vector<Answer>& getAnswers() const
    {
        return answers;
    }

    virtual void display() const
    {
        cout << "Question: " << getQuestionText() << endl;
        cout << "Answers:" << endl;

        for (const auto& answer : answers)
        {
            cout << "- " << answer.getOption() << ". " << answer.getText() << endl;
        }
    }

    bool checkAnswer(char userAnswer) const
    {
        auto it = find_if(answers.begin(), answers.end(),
            [userAnswer](const Answer& answer) { return answer.getOption() == userAnswer; });

        return it != answers.end() && it->isCorrectAnswer();
    }

    virtual ~Question() {}

private:
    string questionText;
    vector<Answer> answers;
};

class SingleChoiceQuestion : public Question
{
public:
    using Question::Question;

    void display() const override
    {
        Question::display();
        cout << "Type: Single Choice" << endl;
    }
};

class MultipleChoiceQuestion : public Question
{
public:
    using Question::Question;

    void display() const override
    {
        Question::display();
        cout << "Type: Multiple Choice" << endl;
    }

    bool checkAnswer(const vector<char>& userAnswers) const
    {
        // Check if the number of selected options matches the number of correct options
        if (userAnswers.size() != countCorrectAnswers())
            return false;

        // Check if all correct options are selected
        return all_of(userAnswers.begin(), userAnswers.end(), [this](char userAnswer) {
            return isCorrectAnswer(userAnswer);
            });
    }

private:
    size_t countCorrectAnswers() const
    {
        return count_if(getAnswers().begin(), getAnswers().end(), [](const Answer& answer) {
            return answer.isCorrectAnswer();
            });
    }

    bool isCorrectAnswer(char userAnswer) const
    {
        auto it = find_if(getAnswers().begin(), getAnswers().end(),
            [userAnswer](const Answer& answer) { return answer.getOption() == userAnswer; });

        return it != getAnswers().end() && it->isCorrectAnswer();
    }
};

class HalfCorrectChoiceQuestion : public Question
{
public:
    using Question::Question;

    void display() const override
    {
        Question::display();
        cout << "Type: Half Correct Choice" << endl;
    }

    bool checkAnswer(const vector<char>& userAnswers) const
    {
        size_t numCorrectAnswers = countCorrectAnswers();
        size_t minCorrectAnswersRequired = numCorrectAnswers / 2 + numCorrectAnswers % 2;

        // Check if the number of selected options is greater than or equal to half of the correct options
        if (userAnswers.size() < minCorrectAnswersRequired)
            return false;

        // Check if at least half of the correct options are selected
        size_t correctAnswersSelected = count_if(userAnswers.begin(), userAnswers.end(), [this](char userAnswer) {
            return isCorrectAnswer(userAnswer);
            });

        return correctAnswersSelected >= minCorrectAnswersRequired;
    }

private:
    size_t countCorrectAnswers() const
    {
        return count_if(getAnswers().begin(), getAnswers().end(), [](const Answer& answer) {
            return answer.isCorrectAnswer();
            });
    }

    bool isCorrectAnswer(char userAnswer) const
    {
        auto it = find_if(getAnswers().begin(), getAnswers().end(),
            [userAnswer](const Answer& answer) { return answer.getOption() == userAnswer; });

        return it != getAnswers().end() && it->isCorrectAnswer();
    }
};

class OpenEndedQuestion : public Question
{
public:
    using Question::Question;

    void display() const override
    {
        Question::display();
        cout << "Type: Open Ended" << endl;
    }

    bool checkAnswer(const string& userAnswer) const
    {
        // Compare user's answer with a correct answer (case-sensitive)
        return userAnswer == correctAnswer;
    }

    // Set the correct answer for the open-ended question
    void setCorrectAnswer(const string& correctAnswer)
    {
        this->correctAnswer = correctAnswer;
    }

private:
    string correctAnswer;
};

class ChronologyQuestion : public Question
{
public:
    using Question::Question;

    void display() const override
    {
        Question::display();
        cout << "Type: Chronology" << endl;
    }

    bool checkAnswer(const vector<char>& userAnswers) const
    {
        // Check if the user's answers match the correct chronological order
        return userAnswers == correctOrder;
    }

    // Set the correct chronological order for the question
    void setCorrectOrder(const vector<char>& correctOrder)
    {
        this->correctOrder = correctOrder;
    }

private:
    vector<char> correctOrder;
};

class Pair
{
public:
    Pair(char leftOption, const string& left, char rightOption, const string& right) : leftOption(leftOption), left(left), rightOption(rightOption), right(right) {}

    char getLeftOption() const
    {
        return leftOption;
    }

    const string& getLeft() const
    {
        return left;
    }

    char getRightOption() const
    {
        return rightOption;
    }

    const string& getRight() const
    {
        return right;
    }

private:
    char leftOption;
    string left;
    char rightOption;
    string right;
};

class MatchingQuestion : public Question
{
public:
    using Question::Question;

    void addPair(char leftOption, const string& left, char rightOption, const string& right)
    {
        pairs.push_back(Pair(leftOption, left, rightOption, right));
    }

    const vector<Pair>& getPairs() const
    {
        return pairs;
    }

    void display() const override
    {
        Question::display();
        cout << "Type: Matching" << endl;
        cout << "Pairs:" << endl;

        for (const auto& pair : pairs)
        {
            cout << "- " << pair.getLeftOption() << ". " << pair.getLeft() << " <-> " << pair.getRightOption() << ". " << pair.getRight() << endl;
        }
    }

    bool checkAnswer(const vector<Pair>& userPairs) const
    {
        // Check if the user's pairs match all correct pairs
        return userPairs.size() == pairs.size() && all_of(userPairs.begin(), userPairs.end(), [this](const Pair& userPair) {
            return isCorrectPair(userPair);
            });
    }

private:
    bool isCorrectPair(const Pair& userPair) const
    {
        auto it = find_if(pairs.begin(), pairs.end(),
            [userPair](const auto& pair)
            {
                return pair.getLeftOption() == userPair.getLeftOption() && pair.getRightOption() == userPair.getRightOption();
            });

        return it != pairs.end();
    }

    vector<Pair> pairs;
};

int main()
{
    // Create questions
    SingleChoiceQuestion singleChoiceQuestion("What is the capital of France?");
    singleChoiceQuestion.addAnswer('A', "Berlin", false);
    singleChoiceQuestion.addAnswer('B', "Madrid", false);
    singleChoiceQuestion.addAnswer('C', "Paris", true);

    MultipleChoiceQuestion multipleChoiceQuestion("Which of the following are fruits?");
    multipleChoiceQuestion.addAnswer('A', "Carrot", false);
    multipleChoiceQuestion.addAnswer('B', "Apple", true);
    multipleChoiceQuestion.addAnswer('C', "Tomato", false);
    multipleChoiceQuestion.addAnswer('D', "Banana", true);

    HalfCorrectChoiceQuestion halfCorrectChoiceQuestion("Select half of the correct options");
    halfCorrectChoiceQuestion.addAnswer('A', "Option A", true);
    halfCorrectChoiceQuestion.addAnswer('B', "Option B", false);
    halfCorrectChoiceQuestion.addAnswer('C', "Option C", true);

    OpenEndedQuestion openEndedQuestion("What is the largest planet in our solar system?");
    openEndedQuestion.setCorrectAnswer("Jupiter");

    ChronologyQuestion chronologyQuestion("Put the following events in chronological order");
    chronologyQuestion.addAnswer('A', "Beginning", false);
    chronologyQuestion.addAnswer('B', "Growth", false);
    chronologyQuestion.addAnswer('C', "Culmination", true);
    chronologyQuestion.addAnswer('D', "End", false);

    // Set the correct chronological order
    chronologyQuestion.setCorrectOrder({ 'A', 'B', 'C', 'D' });

    MatchingQuestion matchingQuestion("Match the following items");
    matchingQuestion.addPair('A', "Item A", '1', "Option 1");
    matchingQuestion.addPair('B', "Item B", '2', "Option 2");
    matchingQuestion.addPair('C', "Item C", '3', "Option 3");


    // Store questions in a vector
    vector<Question*> questions = { &singleChoiceQuestion, &multipleChoiceQuestion, &halfCorrectChoiceQuestion, &openEndedQuestion, &chronologyQuestion, &matchingQuestion };

    // Display and answer each question
    for (auto question : questions)
    {
        question->display();

        if (auto singleChoice = dynamic_cast<SingleChoiceQuestion*>(question))
        {
            char userAnswer;
            cout << "Enter your answer (A, B, or C): ";
            cin >> userAnswer;
            cout << "Result: " << (singleChoice->checkAnswer(userAnswer) ? "Correct" : "Incorrect") << endl;
        }
        else if (auto multipleChoice = dynamic_cast<MultipleChoiceQuestion*>(question))
        {
            vector<char> userAnswers;
            cout << "Enter your answers (e.g., BD): ";
            string userAnswerString;
            cin >> userAnswerString;
            for (char c : userAnswerString)
            {
                userAnswers.push_back(c);
            }
            cout << "Result: " << (multipleChoice->checkAnswer(userAnswers) ? "Correct" : "Incorrect") << endl;
        }
        else if (auto halfCorrectChoice = dynamic_cast<HalfCorrectChoiceQuestion*>(question))
        {
            vector<char> userAnswers;
            cout << "Enter your answers (e.g., AC): ";
            string userAnswerString;
            cin >> userAnswerString;
            for (char c : userAnswerString)
            {
                userAnswers.push_back(c);
            }
            cout << "Result: " << (halfCorrectChoice->checkAnswer(userAnswers) ? "Correct" : "Incorrect") << endl;
        }

        else if (auto openEnded = dynamic_cast<OpenEndedQuestion*>(question))
        {
            string userAnswer;
            cout << "Enter your answer: ";
            cin.ignore();
            getline(cin, userAnswer);
            cout << "Result: " << (openEnded->checkAnswer(userAnswer) ? "Correct" : "Incorrect") << endl;
        }

        else if (auto chronology = dynamic_cast<ChronologyQuestion*>(question))
        {
            vector<char> userAnswers;
            cout << "Enter your answers in chronological order (e.g., ABCD): ";
            string userAnswerString;
            cin >> userAnswerString;
            for (char c : userAnswerString)
            {
                userAnswers.push_back(c);
            }
            cout << "Result: " << (chronology->checkAnswer(userAnswers) ? "Correct" : "Incorrect") << endl;
        }

        else if (auto matching = dynamic_cast<MatchingQuestion*>(question))
        {
            vector<Pair> userPairs;
            cout << "Enter your answers (e.g., A1 B2 C3): ";

            for (const auto& pair : matching->getPairs())
            {
                char leftOption, rightOption;
                cin >> leftOption >> rightOption;
                userPairs.push_back(Pair(leftOption, "", rightOption, ""));
            }

            cout << "Result: " << (matching->checkAnswer(userPairs) ? "Correct" : "Incorrect") << endl;
        }

        cout << endl;
    }

    return 0;
}

