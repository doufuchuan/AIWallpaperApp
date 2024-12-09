#ifndef CHATCLIENTWINDOW_H
#define CHATCLIENTWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include "OpenAIAPIClient.h"
#include "ImageGenerator.h"

class ChatClientWindow : public QWidget {
    Q_OBJECT

public:
    explicit ChatClientWindow(QWidget *parent = nullptr);

private slots:
    void onSendButtonClicked();
    void onResponseReceived(const QString &response);
    void onErrorOccurred(const QString &error);

    //for 图像生成
    void onResponseReceivedForImageGeneration(const QString &response);
    void onGenerateButtonClicked();
    void onImageGenerated(const QString &filePath);

    //壁纸
    void onSetWallpaperButtonClicked();

private:
    QTextEdit *promptEdit;      // Input for chat prompt
    QTextEdit *responseEdit;    // Output for chat response
    QPushButton *sendButton;    // Button to send prompt
    OpenAIAPIClient *chatClient; // OpenAI chat client

    //图像生成
    QLineEdit *fileNameEdit;
    QLineEdit *descriptionEdit;
    QPushButton *generateButton;
    QLabel *imageLabel;
    ImageGenerator *imageGenerator;

    //设置壁纸
    QPushButton *setWallpaperButton;
    QString currentImagePath;
};

#endif // CHATCLIENTWINDOW_H
