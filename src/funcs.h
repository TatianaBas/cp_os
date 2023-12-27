#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <string>
#include <vector>

using namespace std;

//отправка сообщения от клиента серверу
void c_send(int fd_send, string login, string user, string message) {
    string text = login + "$" + user + "$" + message;

    int k = text.size();
    write(fd_send, &k, sizeof(k));

    char c_message[k];
    for (int i = 0; i < k; ++i) {
        c_message[i] = text[i];
    }

    write(fd_send, c_message, k);
}

//получения сообщения клиентом от сервера
string c_recieve(int fd_recieve) {
    int size;
    read(fd_recieve, &size, sizeof(size));

    char s_message[size];
    read(fd_recieve, s_message, size);

    string recv;
    for (int i = 0; i < size; ++i) {
        if (s_message[i] == '$') {
            recv = recv + ": ";
        }
        else {
            recv.push_back(s_message[i]);
        }
    }

    cout << recv << endl;
    return recv;
}

//отправка сообщения от сервера клиенту
void s_send(int fd, string message) {
    string text = message;
    int k = text.size();
    write(fd, &k, sizeof(k));
    char s_message[k];
    for (int i = 0; i < k; ++i) {
        s_message[i] = text[i];
    }
    write(fd, s_message, k);
}

//получение сообщения сервером от клиента
string s_recieve(int fd) {
    int size;
    read(fd, &size, sizeof(size));

    char c_message[size];
    read(fd, c_message, size);

    string recv;
    for (int i = 0; i < size; ++i) {
        recv.push_back(c_message[i]);
    }

    cout << recv << endl;
    return recv;
}

//-----------Парсинг сообщения-----------

//поиск в сообщении отправителя
string find_sender(string message){
    string login;
    int i = 0;
    while (message[i] != '$') {
        login.push_back(message[i]);
        ++i;
    }
    return login;
}

//поиск в сообщении получателя
string find_recipient(string message) {
    string text;
    int i = 0;
    while (message[i] != '$') { ++i; }  ++i;
    while (message[i] != '$') {
        text.push_back(message[i]);
        ++i;
    }
    return text;
}

string find_text(string message) {
    string text;
    int i = 0;
    while (message[i] != '$') { ++i; }  ++i;
    while (message[i] != '$') { ++i; }  ++i;
    while (i < message.size()) {
        text.push_back(message[i]);
        ++i;
    }
    return text;
}

//поиск в сообщении информации для отправки получателю - текст + отправитель
string find_message_info(string message){
    string res, sender, mess;
    int i = 0;
    while (message[i] != '$') {
        sender.push_back(message[i]);
        ++i;
    }  ++i;
    while (message[i] != '$') { ++i; }
    while (i < message.size()) {
        mess.push_back(message[i]);
        ++i;
    }
    res = sender + mess;
    return res;
}