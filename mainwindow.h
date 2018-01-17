#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>



enum MessageType {
    USUAL_MESSAGE, // текст
    PERSON_ONLINE, // онлайн
    WHO_IS_ONLINE, // Запрос о статусе пользователей
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();
    void saveToLogFile(QString str);

private:
    Ui::MainWindow* ui;

    QUdpSocket* socket;

    void UdpChat(QString nick, int port);

    void send(QString str, MessageType type);

    void log(QString s);

private slots:
    void read();
    void on_enterChatButton_clicked();
    void on_sendButton_clicked();

    void on_messageEdit_returnPressed();
    void refreshOnlineList();

};

#endif // MAINWINDOW_H
