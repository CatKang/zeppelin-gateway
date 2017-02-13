#include "zgw_conf.h"

#define READCONFINT(reader, attr, value) \
  reader.GetConfInt(std::string(#attr), &value)
#define READCONFBOOL(reader, attr, value) \
  reader.GetConfBool(std::string(#attr), &value)
#define READCONFSTR(reader, attr, value) \
  reader.GetConfStr(std::string(#attr), &value)
#define READCONFSTRVEC(reader, attr, value) \
  reader.GetConfStrVec(std::string(#attr), &value)

#define READCONF(reader, attr, value, type) \
  ret = READCONF##type(reader, attr, value); \
  if (!ret) printf("%s not set,use default\n", #attr)

ZgwConf::ZgwConf() {
  pthread_rwlock_init(&rwlock_, NULL);
  local_ip_ = std::string("127.0.0.1");
  local_port_ = 9999;
  timeout_ = 100;
  log_path_ = std::string("./log/");
  daemonize_ = false;
  pid_file_ = std::string("./pid");
  lock_file_ = std::string("./lock");
}

ZgwConf::~ZgwConf() {
  pthread_rwlock_destroy(&rwlock_);
}

void ZgwConf::Dump() const {
  fprintf(stderr, "    Config.local_ip    : %s\n", local_ip_.c_str());
  fprintf(stderr, "    Config.local_port  : %d\n", local_port_);
  fprintf(stderr, "    Config.log_path    : %s\n", log_path_.c_str());
  fprintf(stderr, "    Config.daemonize    : %s\n", daemonize_? "true":"false");
}

int ZgwConf::Load(const std::string& path) {
  slash::BaseConf conf_reader(path);
  int res = conf_reader.LoadConf();
  if (res != 0) {
    return res;
  }

  bool ret = false;
  READCONF(conf_reader, local_ip, local_ip_, STR);
  READCONF(conf_reader, local_port, local_port_, INT);
  READCONF(conf_reader, log_path, log_path_, STR);
  READCONF(conf_reader, daemonize, daemonize_, BOOL);
  std::string lock_path = log_path_;
  if (lock_path.back() != '/') {
    lock_path.append("/");
  }
  pid_file_ = lock_path + "pid";
  lock_file_ = lock_path + "lock";
  return res;
}
