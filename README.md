# 基于QT的AI桌面壁纸应用

- [x] 文本输入->提示词生成->图片生成
- [ ] 自定义大模型
- [ ] 语音交互

## Usage:

`OpenAIAPIClient.cpp`, 设置语言模型
```cpp
OpenAIAPIClient::OpenAIAPIClient(QObject *parent) : OpenAIBaseClient(parent) {
    setApiUrl("https://internlm-chat.intern-ai.org.cn/puyu/api/v1/chat/completions");//https://api.openai.com/v1/chat/completions  https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions
    setModel("internlm2.5-latest");//gpt-4o   qwen-plus

    // Connect the finished signal of QNetworkAccessManager to handleNetworkReply
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &OpenAIAPIClient::handleNetworkReply);
}
```

`ImageGenerator.cpp`，设置图像生成模型
```cpp
ImageGenerator::ImageGenerator(QObject *parent)
    : OpenAIBaseClient(parent), m_downloadPath(QDir::currentPath()), 
    m_outputFileName("generated_image.jpg") { // Default file name
    setModel("stabilityai/stable-diffusion-3-5-large");//dall-e-3
    setApiUrl("https://api.siliconflow.cn/v1/images/generations");//https://api.openai.com/v1/images/generations
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &ImageGenerator::handleImageGenerationReply);
}
```

`ChatClientWindow.cpp`, 设置api-key
```cpp
QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
QString apiKey = "xxxxxxx";//env.value("OPENAI_API_KEY");

if (apiKey.isEmpty()) {
  qWarning() << "Error: OPENAI_API_KEY environment variable is not set.";
}

// Set default API key for the image generator
chatClient->setApiKey(apiKey); // Replace with your OpenAI API key
QString apiKey2 = "sk-xxxxxx";
imageGenerator->setApiKey(apiKey2);
```

QT下模型api调用参考:
https://github.com/muman613/qt-openai
