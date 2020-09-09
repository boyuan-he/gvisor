// Copyright 2020 The gVisor Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <errno.h>
#include <fcntl.h>
#include <linux/fuse.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "test/fuse/linux/fuse_base.h"
#include "test/util/fuse_util.h"
#include "test/util/test_util.h"

namespace gvisor {
namespace testing {

namespace {

class LinkTest : public FuseTest {
 protected:
  const std::string target_file_ = "target_file_";
  const std::string link_name_ = "link_name_";
  const mode_t perm_ = S_IRWXU | S_IRWXG | S_IRWXO;
};

TEST_F(LinkTest, CreateLink) {
  const std::string target_file_path =
      JoinPath(mount_point_.path().c_str(), target_file_);
  const std::string link_path =
      JoinPath(mount_point_.path().c_str(), link_name_);

  SetServerInodeLookup(target_file_, S_IFREG | perm_);
  struct fuse_out_header out_header = {
      .len = sizeof(struct fuse_out_header) + sizeof(struct fuse_entry_out),
  };
  struct fuse_entry_out out_payload = DefaultEntryOut(S_IFREG | perm_, 2);
  auto iov_out = FuseGenerateIovecs(out_header, out_payload);
  SetServerResponse(FUSE_LINK, iov_out);

  ASSERT_THAT(link(target_file_path.c_str(), link_path.c_str()),
              SyscallSucceeds());
  struct fuse_in_header in_header;
  struct fuse_link_in in_payload;
  std::vector<char> actual_link_name(link_name_.length() + 1);
  auto iov_in = FuseGenerateIovecs(in_header, in_payload, actual_link_name);

  GetServerActualRequest(iov_in);
  EXPECT_EQ(in_header.len,
            sizeof(in_header) + sizeof(in_payload) + link_name_.length() + 1);
  EXPECT_EQ(in_header.opcode, FUSE_LINK);
  EXPECT_EQ(in_payload.oldnodeid, 2);
}

TEST_F(LinkTest, FileTypeError) {
  const std::string target_file_path =
      JoinPath(mount_point_.path().c_str(), target_file_);
  const std::string link_path =
      JoinPath(mount_point_.path().c_str(), link_name_);

  SetServerInodeLookup(target_file_, S_IFREG | perm_);
  struct fuse_out_header out_header = {
      .len = sizeof(struct fuse_out_header) + sizeof(struct fuse_entry_out),
  };
  struct fuse_entry_out out_payload = DefaultEntryOut(S_IFLNK | perm_, 5);
  auto iov_out = FuseGenerateIovecs(out_header, out_payload);
  SetServerResponse(FUSE_LINK, iov_out);

  ASSERT_THAT(link(target_file_path.c_str(), link_path.c_str()),
              SyscallFailsWithErrno(EIO));
  SkipServerActualRequest();
}

}  // namespace

}  // namespace testing
}  // namespace gvisor
