#pragma once
#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"
#include "BitmapFont.h"
#include <string>
#include <vector>

struct QuizQuestion {
    std::string question;
    std::vector<std::string> options; // 4 options A, B, C, D
    int correctIndex;                 // 0-3
    std::string daveComment;          // Crazy Dave's fun comment
};

enum class QuizState {
    Rules,
    Playing,
    AnswerFeedback,
    Summary
};

class QuizMenu {
public:
    QuizMenu(Resources& res);
    ~QuizMenu() = default;

    void update(float dt, bool& showQuiz);
    void draw();

private:
    void loadQuestions();
    void startQuiz();
    void selectAnswer(int optionIndex);
    void nextQuestion();
    void drawWrappedText(const BitmapFont& font, const std::string& text, float x, float y, float maxW, float scale, Color color, float lineSpacing = 24.0f) const;

    Resources& m_res;
    BitmapFont m_fontTitle;       // HouseofTerror28
    BitmapFont m_fontQuestion;    // BrianneTod16
    BitmapFont m_fontOption;      // DwarvenTodcraft24
    BitmapFont m_fontStats;       // ContinuumBold14

    Reanimation m_crazyDave;
    Texture2D m_bg;
    Texture2D m_coinBank;
    Texture2D m_closeBtn;
    Texture2D m_closeBtnHl;

    QuizState m_state = QuizState::Rules;
    std::vector<QuizQuestion> m_allQuestions;
    std::vector<QuizQuestion> m_currentRoundQuestions;
    int m_currentQuestionIndex = 0;
    int m_score = 0;
    int m_coinsEarned = 0;
    int m_totalPlayerCoins = 5000;
    int m_selectedAnswer = -1;
    bool m_isCorrect = false;
    float m_feedbackTimer = 0.0f;

    // Floating text feedback (e.g. "+$250!")
    struct FloatingText {
        std::string text;
        Vector2 pos;
        float alpha;
        Color color;
    };
    std::vector<FloatingText> m_floatingTexts;
};
