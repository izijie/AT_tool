#ifndef MYHIGHTER_H
#define MYHIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>

class Myhighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
         Myhighter(QTextDocument* parent);
protected:
    void highlightBlock(const QString &text) override;

private:
    void setupHighlightingRules();

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;

signals:
};

#endif // MYHIGHTER_H
