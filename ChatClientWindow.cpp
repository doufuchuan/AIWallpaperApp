#include "ChatClientWindow.h"
#include <QDebug>
#include <QProcessEnvironment>
#include <QDir>
#include <QPixmap>
#include "windows.h"
ChatClientWindow::ChatClientWindow(QWidget *parent)
    : QWidget(parent),
    promptEdit(new QTextEdit(this)),
    responseEdit(new QTextEdit(this)),
    sendButton(new QPushButton("发送", this)),
    chatClient(new OpenAIAPIClient(this)),
    //图像生成
    fileNameEdit(new QLineEdit(this)),
    // descriptionEdit(new QLineEdit(this)),
    // generateButton(new QPushButton("生成", this)),
    imageLabel(new QLabel(this)),
    imageGenerator(new ImageGenerator(this)),
    setWallpaperButton(new QPushButton("设置为壁纸", this)){

    // Set up the layout
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setColumnStretch(0,1);
    mainLayout->setColumnStretch(1,5);
    // Prompt entry
    QLabel *promptLabel = new QLabel("壁纸主题:", this);
    mainLayout->addWidget(promptLabel,0,0);
    mainLayout->addWidget(promptEdit,1,0);

    // Send button
    mainLayout->addWidget(sendButton,2,0);

    // Response output
    QLabel *responseLabel = new QLabel("结果:", this);
    mainLayout->addWidget(responseLabel,3,0);
    responseEdit->setReadOnly(true);
    mainLayout->addWidget(responseEdit,4,0);

    // Input fields layout
    // QHBoxLayout *inputLayout = new QHBoxLayout();
    mainLayout->addWidget(new QLabel("File Name:", this),5,0);
    mainLayout->addWidget(fileNameEdit,6,0);
    // mainLayout->addWidget(new QLabel("Description:", this),2,1);
    // mainLayout->addWidget(descriptionEdit,3,1);

    // mainLayout->addLayout(inputLayout,0,1);

    // Generate button
    // mainLayout->addWidget(generateButton,4,1);
    mainLayout->addWidget(setWallpaperButton, 6, 1);
    // Image display area
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setText("Generated image will appear here.");
    imageLabel->setStyleSheet("QLabel { background-color : lightgray; }");
    mainLayout->addWidget(imageLabel,0,1,7,1);

    // connect(generateButton, &QPushButton::clicked, this, &ChatClientWindow::onGenerateButtonClicked);

    // Connect signals from the OpenAIImageGenerator
    connect(imageGenerator, &ImageGenerator::imageGenerated, this, &ChatClientWindow::onImageGenerated);
    connect(imageGenerator, &ImageGenerator::errorOccurred, this, &ChatClientWindow::onErrorOccurred);

    // Connect the send button to the chat client
    connect(sendButton, &QPushButton::clicked, this, &ChatClientWindow::onSendButtonClicked);

    // Connect signals from the chat client
    connect(chatClient, &OpenAIAPIClient::responseReceived, this, &ChatClientWindow::onResponseReceived);
    connect(chatClient, &OpenAIAPIClient::responseReceived, this, &ChatClientWindow::onResponseReceivedForImageGeneration);
    connect(chatClient, &OpenAIAPIClient::errorOccurred, this, &ChatClientWindow::onErrorOccurred);

    // Connect the set wallpaper button
    connect(setWallpaperButton, &QPushButton::clicked, this, &ChatClientWindow::onSetWallpaperButtonClicked);

    // Retrieve the API key from the environment variable
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString apiKey = "";//env.value("OPENAI_API_KEY");sk-2f9a7fa7e08a4808a3a85a5603943d5a

    if (apiKey.isEmpty()) {
        qWarning() << "Error: OPENAI_API_KEY environment variable is not set.";
    }

    // Set default API key for the image generator
    chatClient->setApiKey(apiKey); // Replace with your OpenAI API key
    QString apiKey2 = "";
    imageGenerator->setApiKey(apiKey2);
}

void ChatClientWindow::onSendButtonClicked() {
    QString prompt = promptEdit->toPlainText().trimmed();
    QString Sprompt = "请生成stable diffuison提示词用以生成壁纸，主题为：";
    if (prompt.isEmpty()) {
        qCritical() << "Prompt cannot be empty.";
        return;
    }

    // Send the prompt to the chat client
    chatClient->sendMessage(Sprompt + prompt + "。只需输出提示词：");
    sendButton->setEnabled(false); // Disable the button to prevent multiple requests
    // responseEdit->setText("Waiting for response...");
}

void ChatClientWindow::onResponseReceived(const QString &response) {
    qDebug() << "Response received:" << response;

    // Set Markdown content to the document
    responseEdit->document()->setMarkdown(response);
    sendButton->setEnabled(true); // Re-enable the button
}

void ChatClientWindow::onResponseReceivedForImageGeneration(const QString &response) {
    qDebug() << "Response received for image generation:" << response;

    // Use the response as the description for image generation
    QString description = response;
    QString fileName = fileNameEdit->text().trimmed();
    if (fileName.isEmpty()) {
        fileName = "generated_image.png";
    }
    // Configure the image generator
    imageGenerator->setOutputFileName(fileName); // Default file name
    imageGenerator->setDownloadPath(QDir::currentPath());

    // Start generating the image
    imageGenerator->generateImage(description);
    // generateButton->setEnabled(false); // Disable button to prevent multiple requests
    imageLabel->setText("Generating image...");
}

void ChatClientWindow::onGenerateButtonClicked() {
    QString fileName = fileNameEdit->text().trimmed();
    QString description = descriptionEdit->text().trimmed();

    if (fileName.isEmpty() || description.isEmpty()) {
        qCritical() << "File name or description cannot be empty.";
        return;
    }

    // Configure the image generator
    imageGenerator->setOutputFileName(fileName);
    imageGenerator->setDownloadPath(QDir::currentPath());

    // Start generating the image
    imageGenerator->generateImage(description);
    // generateButton->setEnabled(false); // Disable button to prevent multiple requests
    imageLabel->setText("Generating image...");
}

void ChatClientWindow::onImageGenerated(const QString &filePath) {
    qDebug() << "Image successfully generated at:" << filePath;

    // Load and display the image
    QPixmap pixmap(filePath);
    if (!pixmap.isNull()) {
        imageLabel->setPixmap(pixmap);
        currentImagePath = filePath; // Store the path of the current image
        setWallpaperButton->setEnabled(true); // Enable the set wallpaper button
    } else {
        imageLabel->setText("Failed to load image.");

        setWallpaperButton->setEnabled(false);
    }

    // generateButton->setEnabled(true); // Re-enable the button
}

// 设置壁纸
void ChatClientWindow::onSetWallpaperButtonClicked() {
    if (currentImagePath.isEmpty()) {
        qCritical() << "No image generated to set as wallpaper.";
        return;
    }

    // Set the image as the desktop wallpaper
    if (SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)currentImagePath.toStdWString().c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE)) {
        qDebug() << "Wallpaper set successfully.";
    } else {
        qCritical() << "Failed to set wallpaper.";
    }
}

void ChatClientWindow::onErrorOccurred(const QString &error) {
    qCritical() << "Error occurred:" << error;

    // Display the error message
    responseEdit->setText("Error: " + error);
    sendButton->setEnabled(true); // Re-enable the button
    imageLabel->setText("Error: " + error);
    // generateButton->setEnabled(true);
    setWallpaperButton->setEnabled(false);
}
