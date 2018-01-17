#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QTime>
#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInterface>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->portNumEdit->setValidator( new QIntValidator(1000, 60000, this) );
    QHostInfo hostInfo;
    ui->nicknameEdit->setText(hostInfo.localHostName());
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::UdpChat(QString nick, int port) {



    socket = new QUdpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(read()));

    port = (ui->portNumEdit->text().toInt());

    if  ( !socket->bind(QHostAddress::Any, port) )  {
    log(QString("УПСС !!! Порт %1 уже открыт. Введите другое значение ").arg(port));
    return;
    }

    log(QString("Чат открыт на порту %1").arg(port));
    send(nick + " - в чате", USUAL_MESSAGE);

    QTime now = QTime::currentTime();
    QString str = now.toString("hh:mm:ss") /*nowStr*/ + " " +  ui->nicknameEdit->text();
    send(str, PERSON_ONLINE);

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),  this, SLOT(refreshOnlineList()));
    timer->start(15000);
}

void MainWindow::on_enterChatButton_clicked() {
    QString nick = ui->nicknameEdit->text();
    UdpChat(nick, ui->portNumEdit->text().toInt());

    ui->sendButton->setEnabled(true);
    ui->nicknameEdit->setEnabled(false);
}


void MainWindow::send(QString str, MessageType type) {

    if(type == USUAL_MESSAGE)
        log(QString("send: %1 %2").arg(str).arg(type));

    QByteArray data;

    QDataStream out(&data, QIODevice::WriteOnly);
    out << qint8(type); // Тип
    out << str; // сообщение
    socket->writeDatagram(data, QHostAddress::Broadcast, ui->portNumEdit->text().toInt() );
}



void MainWindow::read() {
    while (socket->hasPendingDatagrams()) {

        QByteArray buf;

        buf.resize(socket->pendingDatagramSize());
        QHostAddress* address = new QHostAddress();
        socket->readDatagram(buf.data(), buf.size(), address);

        log(QString("Message from IP: %1 size: %2").arg(address->toString()).arg(buf.size()));
        delete address;

        QDataStream in(&buf, QIODevice::ReadOnly);

        qint8 type = 0;
        in >> type;

        QString str;
        in >> str;

        if(str.length() == 0)
            return;

        log(QString("read>> %1 %2").arg(str).arg(type));


        if (type == USUAL_MESSAGE) {
            ui->plainTextEdit->appendPlainText(str);
        } else if (type == PERSON_ONLINE) {

            ui->onlineList->addItem(str);

        } else if (type == WHO_IS_ONLINE) {

            QTime now = QTime::currentTime();
            QString nowStr = now.toString("hh:mm:ss");
            send(nowStr + " " + ui->nicknameEdit->text(),PERSON_ONLINE);
        }
    }
}

void MainWindow::on_sendButton_clicked() {
    send(ui->nicknameEdit->text() + " : " + ui->messageEdit->text(),USUAL_MESSAGE);
    ui->messageEdit->clear();
    QTime now = QTime::currentTime();
    QString nowStr = now.toString("hh:mm:ss");
    send(nowStr + " " + ui->nicknameEdit->text(), PERSON_ONLINE);
}

void MainWindow::on_messageEdit_returnPressed() {
    on_sendButton_clicked();
}

void MainWindow::refreshOnlineList() {
    // Удаляем тех, от кого давно не было сообщений
    for(int i = 0; i < ui->onlineList->count(); ++i) {
        // Получаем i-ую запись из списка
        QListWidgetItem* item = ui->onlineList->item(i);
        //
        QString str = item->text();
        QStringList list = str.split(" ");
        QString dateStr = list.at(0);


        QTime time = QTime::fromString(dateStr, "hh:mm:ss");
        QTime now = QTime::currentTime();
        int diff = time.msecsTo(now);


        if(diff > 10000) {
            //if(nickLst != nick){
            ui->onlineList->takeItem(i);

        }
    }

    //send("Who is online?", WHO_IS_ONLINE);
}

void MainWindow::log(QString s) {
    ui->log->append(s);
}
