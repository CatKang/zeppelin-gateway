#include "src/s3_cmds/zgw_s3_object.h"

#include "slash/include/env.h"
#include "src/zgwstore/zgw_define.h"

bool HeadObjectCmd::DoInitial() {

  return TryAuth();
}

void HeadObjectCmd::DoAndResponse(pink::HttpResponse* resp) {
  if (http_ret_code_ == 200) {
    Status s = store_->GetObject(user_name_, bucket_name_, object_name_,
                                 &object_);
    if (s.ok()) {
      http_ret_code_ = 200;
      resp->SetHeaders("ETag", "\"" + object_.etag + "\"");
      resp->SetHeaders("Last-Modified", http_nowtime(object_.last_modified));
    } else {
      if (s.ToString().find("Bucket Doesn't Belong To This User") !=
          std::string::npos) {
        http_ret_code_ = 404;
      } else if (s.ToString().find("Object Not Found") != std::string::npos) {
        http_ret_code_ = 404;
      }
    }
  }

  if (http_ret_code_ == 200) {
    resp->SetContentLength(object_.size);
  } else {
    resp->SetContentLength(0);
  }
  resp->SetStatusCode(http_ret_code_);
}

int HeadObjectCmd::DoResponseBody(char* buf, size_t max_size) {
  // HEAD needn't response data
  return -2;
}