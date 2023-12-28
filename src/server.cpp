#include "funcs.h"
#include <asm-generic/errno-base.h>
#include <atomic>
#include <csignal>
#include <unistd.h>

int in(vector<string> logins, string str) {
  for (int i = 0; i < logins.size(); ++i) {
    if (logins[i] == str)
      return i;
  }
  return -1;
}

int in_chat(vector<vector<string>> chats, string str) {
  for (int i = 0; i < chats.size(); ++i) {
    if (chats[i][0] == str)
      return i;
  }
  return -1;
}

int main() {
  vector<string> logins;
  vector<vector<string>> chats;

  cout << "Enter number of users\n";
  int n;
  cin >> n;
  cout << "\nEnter all user's logins.\n";

  while (n > 0) {
    string login;
    cin >> login;
    if (login == "") {
      break;
    }
    if (in(logins, login) == -1) {
      logins.push_back(login);
    } else {
      cout << "Login already exists!\n\n";
    }
    n--;
  }

  cout << "Instant messaging system is started!\n";

  // создание и открытие входного FIFO
  if (mkfifo("input", 0777) == -1) {
    if (errno == EEXIST) {
      cout << "MAIN INPUT FIFO WAS ALREADY CREATED" << endl;
    } else {
      cout << "MAIN INPUT FIFO WAS NOT CREATED";
      exit(1);
    }
  }
  int fd_recv = open("input", O_RDONLY);
  if (fd_recv == -1) {
    cout << "INPUT FIFO WAS NOT OPENED";
    exit(1);
  }

  // создание и открытие выходных FIFO для всех логинов
  for (int i = 0; i < logins.size(); ++i) {
    if (mkfifo(logins[i].c_str(), 0777) == -1) {
      if (errno == EEXIST) {
        cout << "USER FIFO WAS ALREADY CREATED" << endl;
      } else {
        cout << "FIFO WAS NOT CREATED";
        exit(1);
      }
    }
  }

  int fd_l[logins.size()];
  for (int i = 0; i < logins.size(); ++i) {
    fd_l[i] = open(logins[i].c_str(), O_WRONLY);
    if (fd_l[i] == -1) {
      cout << "FIFO login WAS NOT OPENED";
      exit(1);
    }
  }

  // обработка сообщений, полученных от клиентов
  while (true) {
    string message;
    message = s_recieve(fd_recv); // читаем из input файла
    // cout << "message  " << message << endl;

    string f_sender = find_sender(message);
    string f_recipient = find_recipient(message);
    string f_message_info = find_message_info(message);

    int sender = in(logins, f_sender);

    if (f_recipient == "create") { // processing a request to create a chat
      vector<string> v_cur;
      string tmp = find_text(message);

      string s_cur;
      for (int i = 0; i < tmp.size(); i++) {
        if (tmp[i] == '$') {
          v_cur.push_back(s_cur);
          cout << s_cur;
          s_cur = "";
        } else {
          s_cur += tmp[i];
        }
      }

      bool correct_usernames = true;
      for (int i = 1; i < v_cur.size();
           i++) { // check for correct usernames in chat
        if (in(logins, v_cur[i]) == -1) {
          correct_usernames = false;
        }
      }

      if (correct_usernames) {
        chats.push_back(v_cur);
        s_send(fd_l[sender], "Chat created successfully!");
      } else {
        s_send(fd_l[sender], "Login does not exists!");
      }
    } else { // send messenges
      int recipient;
      if (in(logins, f_recipient) != -1) { // to users
        recipient = in(logins, f_recipient);
        s_send(fd_l[recipient], f_message_info);
      } else if (in_chat(chats, f_recipient) != -1) { // to chats
        bool sender_can_write = false;
        int chat_num = in_chat(chats, f_recipient);

        for (int i = 1; i < chats[chat_num].size();
             i++) { // checking whether the sender is in the chat
          string rec_name = chats[chat_num][i];
          if (f_sender == rec_name) {
            sender_can_write = true;
          }
        }

        if (sender_can_write) { // success check
          for (int i = 1; i < chats[chat_num].size(); i++) {
            string rec_name = chats[chat_num][i];

            int rec = in(logins, rec_name);

            if (sender != rec) {
              s_send(fd_l[rec], f_message_info);
            }
          }
        } else { // unsuccess
          s_send(fd_l[sender], "You can't write to this chat!");
        }

      } else { // wrong login or chat name
        s_send(fd_l[sender], "Login does not exists!");
      }
    }
  }
};
