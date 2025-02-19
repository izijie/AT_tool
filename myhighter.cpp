#include "myhighter.h"
#include <QSyntaxHighlighter>

Myhighter::Myhighter(QTextDocument* parent)
    :QSyntaxHighlighter{parent}
{
    setupHighlightingRules();
}

void Myhighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

}

void Myhighter::setupHighlightingRules()
{
    HighlightingRule rule;

    // 关键字高亮规则
    keywordFormat.setForeground(Qt::darkGreen);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\OK\\b" << "\\bWIFI CONNECTED\\b" << "\\bready\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

   //其他高亮
    commentFormat.setForeground(Qt::darkRed);
    keywordFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("\\ERROR\\b");
    rule.format = commentFormat;
    highlightingRules.append(rule);


    commentFormat.setForeground(Qt::darkRed);
    keywordFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("\\FAIL\\b");
    rule.format = commentFormat;
    highlightingRules.append(rule);

    commentFormat.setForeground(Qt::darkRed);
    keywordFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("\\WIFI DISCONNECT\\b");
    rule.format = commentFormat;
    highlightingRules.append(rule);

    commentFormat.setForeground(Qt::darkRed);
    keywordFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("\\DISCONNECT\\b");
    rule.format = commentFormat;
    highlightingRules.append(rule);
}
