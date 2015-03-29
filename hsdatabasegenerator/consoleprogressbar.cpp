#include "consoleprogressbar.h"
#include <string>
#include <iostream>

ConsoleProgressBar* ConsoleProgressBar::mInstance = NULL;

ConsoleProgressBar::ConsoleProgressBar(QObject *parent) : QObject(parent)
{
    mProgressBars.fill('.', 100);
    std::cout << "Overall progression:\n"
              << mProgressBars.toStdString() << " 0%";
}

ConsoleProgressBar* ConsoleProgressBar::Instance()
{
    if (mInstance == NULL)
        mInstance = new ConsoleProgressBar();
    return mInstance;
}

ConsoleProgressBar::~ConsoleProgressBar()
{
}

void ConsoleProgressBar::update(int progress)
{
    if (progress > 100)
        return;

    for (int i = progress - 1; i >= 0; i -= 1) {
        if (mProgressBars.at(i) == '|')
            break;

        mProgressBars[i] = '|';
    }

    std::cout << "\r"; // carriage return back to beginning of line
    std::cout << mProgressBars.toStdString() << " " << progress << "%";
}
