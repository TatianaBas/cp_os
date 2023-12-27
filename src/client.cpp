#include "funcs.h"
#include <thread>

//функция приёма сообщений (для потока)
void func(int fd_recv, string login){
    while (true){
        string reply = c_recieve(fd_recv);
        //cout << reply << "\n";
        cout.flush();
        cout << login << ">";
        cout.flush();
    }
}


int main(){
    //подключение к входному FIFO сервера
    int fd_send = open("input", O_WRONLY);
    if (fd_send == -1)  {
        cout << "ERROR: MAIN FIFO WAS NOT OPENED\n";
        exit(1);
    }

    cout << "Insert login or chat name: ";
    string login;

    //подключение к персональному именованному пайпу
    int fd_recv = -1;
    while (fd_recv == -1)  {
        cin >> login;
        fd_recv = open(login.c_str(), O_RDONLY);
        if (fd_recv == -1) {
            cout << "Wrong login!\nInsert your login: ";
        }
    };

    
    string adressee, message;
    cout << "You have successfully signed!\n\n";

    //запуск потока принятия сообщений от сервера
    thread thr_recieve(func, fd_recv, login);

    cout << "USAGE: <recipient's login or chat name> <your message>\n";
    cout << "\tcreate <chat's name> <user's quantity> <names of users>\n";
    cout << "\tEXAMPLE: create family_chat 3 mama papa kot\n\n";
    cout << "\tquit - completion of work\n\n"; 
        
    while (true) {
        cout << login <<"> ";
        cin >> adressee;

        if (adressee == "quit"){ //quit client
            break;
        }
        if (adressee == "create"){ //create new chat
            string cur;
            cin >> cur; 
            message += cur + "$";

            int n; cin >> n;
            while (n > 0){
                cin >> cur;
                message += cur + "$";
                n--;
            }
        }
        else{ 
            getline(cin, message);
        }
        
        c_send(fd_send, login, adressee, message);
        
    }
    thr_recieve.detach();
}