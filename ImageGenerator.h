#ifndef IMAGEGENERATOR_H
#define IMAGEGENERATOR_H

#include "OpenAIBaseClient.h"

class ImageGenerator : public OpenAIBaseClient {
    Q_OBJECT

public:
    explicit ImageGenerator(QObject *parent = nullptr);

    void generateImage(const QString &prompt);

    // New setters and getters
    void setDownloadPath(const QString &path);
    QString downloadPath() const;

    void setOutputFileName(const QString &fileName);
    QString outputFileName() const;

signals:
    void imageGenerated(const QString &filePath);
    void errorOccurred(const QString &errorString);

private slots:
    void handleImageGenerationReply(QNetworkReply *reply);

protected:
    QString m_downloadPath;   // Directory to save the downloaded image
    QString m_outputFileName; // File name for the downloaded image
};

#endif // IMAGEGENERATOR_H
