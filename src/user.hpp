#include "../lib/utils.hpp"
#include "../lib/bpt.hpp"
#include "../lib/map.hpp"
#include "../lib/exceptions.hpp"

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
  BPlusTree<my_string<20>, User> UserMap;
  sjtu::map<my_string<20>, int> LoginState;
 public:
  // 1 for successful operation and 0 for unsuccessful
  bool add_user(const std::string &cur,
                const std::string &username,
                const std::string &password,
                const std::string &name,
                const std::string &mailAddr,
                int privilege) {
    int real_privilege = privilege;
    my_string<20> cur_name(cur), todo(username);
    bool super_admin = false;
    if (UserMap.empty()) {
      real_privilege = 10;
    } else {
      auto target = UserMap.find(todo);
      if (target.GetPrivilege() != -1 || LoginState.find(cur_name) == LoginState.end()
          || privilege >= LoginState[cur_name]) {
        throw sjtu::exception();
      }
      User to_add(username, password, name, mailAddr, privilege);
      UserMap.insert(username, to_add);
    }

  }

  bool login(const std::string &username, const std::string &password) {
    my_string<20> todo(username);
    if (LoginState.find(todo) != LoginState.end()) {
      return false;
    }
    User res = UserMap.find(todo);
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
    my_string<20> todo(username);
    auto temp = LoginState.find(todo);
    if (temp != LoginState.end()) {
      LoginState.erase(temp);
      return true;
    } else {
      return false;
    }
  }

  std::string query_profile(const std::string &cur, const std::string &username) {
    my_string<20> cur_name(cur), todo(username);
    if (LoginState.find(cur_name) == LoginState.end()) {
      throw sjtu::exception();
    }
    User target = UserMap.find(todo);
    if (target.GetPrivilege() == -1 || LoginState[cur_name] <= target.GetPrivilege() && cur != username) {
      throw sjtu::exception();
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
    my_string<20> cur_name(cur), todo(username);
    if (LoginState.find(cur_name) == LoginState.end()) {
      throw sjtu::exception();
    }
    User target = UserMap.find(todo), original = target;
    if (target.GetPrivilege() == -1 || LoginState[cur_name] <= target.GetPrivilege() && cur != username) {
      throw sjtu::exception();
    }
    if (new_privilege != -1) {
      if (new_privilege >= LoginState[cur_name]) {
        throw sjtu::exception();
      }
      target.ChangePrivilege(new_privilege);
      if (LoginState.find(todo) != LoginState.end()) {
        LoginState[todo] = new_privilege;
      }
    }
    if (!new_password.empty()) target.ChangePassword(new_password);
    if (!new_name.empty()) target.ChangeName(new_name);
    if (!new_mailAddr.empty()) target.ChangeMail(new_mailAddr);
    UserMap.modify(todo, original, target);
    std::string ans =
        username + ' ' + target.GetName() + ' ' + target.GetMailAddr() + ' ' + std::to_string(target.GetPrivilege());
    return ans;
  }
};