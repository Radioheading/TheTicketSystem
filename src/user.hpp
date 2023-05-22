#ifndef USER_HPP_
#define USER_HPP_

#include "../lib/utils.hpp"
#include "../lib/bpt.hpp"
#include "../lib/map.hpp"
#include "../lib/exceptions.hpp"

using namespace Lee;

class User {
 private:
  my_string<20> username;
  my_string<30> password;
  my_string<20> name;
  my_string<30> mailAddr;
  int privilege = -1; // to check the default constructor
 public:
  User(const std::string &_username = "",
       const std::string &_password = "",
       const std::string &_name = "",
       const std::string &_mailAddr = "", int _privilege = -1) : username(_username),
                                                                 password(_password),
                                                                 name(_name),
                                                                 mailAddr(_mailAddr),
                                                                 privilege(_privilege) {};
  void ChangePassword(const std::string &new_password) {
    password = new_password;
  }

  void ChangeName(const std::string &new_name) {
    name = new_name;
  }

  void ChangeMail(const std::string &new_mail) {
    mailAddr = new_mail;
  }

  void ChangePrivilege(int new_privilege) {
    privilege = new_privilege;
  }

  [[nodiscard]] std::string GetUsername() const {
    return (std::string) username.info;
  }

  [[nodiscard]] int GetPrivilege() const {
    return privilege;
  }

  [[nodiscard]] std::string GetPassword() const {
    return (std::string) password.info;
  }

  [[nodiscard]] std::string GetName() const {
    return (std::string) name.info;
  }

  [[nodiscard]] std::string GetMailAddr() const {
    return (std::string) mailAddr.info;
  }

  friend bool operator<(const User &cmp_1, const User &cmp_2) {
    return cmp_1.username < cmp_2.username;
  }

  friend bool operator==(const User &cmp_1, const User &cmp_2) {
    return cmp_1.username == cmp_2.username;
  }

  friend bool operator!=(const User &cmp_1, const User &cmp_2) {
    return !(cmp_1.username == cmp_2.username);
  }
};

class UserSystem {
 private:
  BPlusTree<size_t, User> UserMap;
  map<std::string, int> LoginState;
 public:
  // 1 for successful operation and 0 for unsuccessful
  UserSystem(const std::string &name_1, const std::string &name_2) : UserMap(name_1, name_2) {}

  bool check_user(const std::string &username) {
    return LoginState.find(username) != LoginState.end();
  }

  bool add_user(const std::string &cur,
                const std::string &username,
                const std::string &password,
                const std::string &name,
                const std::string &mailAddr,
                int privilege) {
    int real_privilege = privilege;
    size_t user_key = MyHash(username);
    if (UserMap.empty()) {
      real_privilege = 10;
    } else {
      auto target = UserMap.find(user_key);
      if (target.GetPrivilege() != -1 || LoginState.find(cur) == LoginState.end()
          || privilege >= LoginState[cur]) {
        return false;
      }
    }
    User to_add(username, password, name, mailAddr, real_privilege);
    UserMap.insert(user_key, to_add);
    return true;
  }

  bool login(const std::string &username, const std::string &password) {
    size_t user_key = MyHash(username);
    if (LoginState.find(username) != LoginState.end()) {
      return false;
    }
    User res = UserMap.find(user_key);
    if (res.GetPrivilege() == -1) {
      return false;
    }
    if (res.GetPassword() != password) {
      return false;
    }
    LoginState[username] = res.GetPrivilege();
    return true;
  }

  bool logout(const std::string &username) {
    auto temp = LoginState.find(username);
    if (temp != LoginState.end()) {
      LoginState.erase(temp);
      return true;
    } else {
      return false;
    }
  }

  std::string query_profile(const std::string &cur, const std::string &username) {
    size_t user_key = MyHash(username);
    if (LoginState.count(cur) == 0) {
      return "-1";
    }
    User target = UserMap.find(user_key);
    if (target.GetPrivilege() == -1 || (LoginState[cur] <= target.GetPrivilege() && cur != username)) {
      return "-1";
    }
    std::string ans =
        username + ' ' + target.GetName() + ' ' + target.GetMailAddr() + ' ' + std::to_string(target.GetPrivilege());
    return ans;
  }

  std::string modify_profile(const std::string &cur,
                             const std::string &username,
                             const std::string &new_password,
                             const std::string &new_name,
                             const std::string &new_mailAddr,
                             int new_privilege) {
    size_t user_key = MyHash(username);
    if (LoginState.find(cur) == LoginState.end()) {
      return "-1";
    }
    User target = UserMap.find(user_key);
    if (LoginState.find(cur) == LoginState.end() || target.GetPrivilege() == -1 || LoginState[cur] <= target.GetPrivilege() && cur != username || new_privilege >= LoginState[cur]) {
      return "-1";
    }
    if (new_privilege != -1) {
      target.ChangePrivilege(new_privilege);
      if (LoginState.find(username) != LoginState.end()) {
        LoginState[username] = new_privilege;
      }
    }
    if (!new_password.empty()) target.ChangePassword(new_password);
    if (!new_name.empty()) target.ChangeName(new_name);
    if (!new_mailAddr.empty()) target.ChangeMail(new_mailAddr);
    UserMap.modify(user_key, target);
    std::string ans =
        username + ' ' + target.GetName() + ' ' + target.GetMailAddr() + ' ' + std::to_string(target.GetPrivilege());
    return ans;
  }

  void clean() {
    UserMap.clear();
    LoginState.clear();
  }
};
#endif