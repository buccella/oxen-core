// Copyright (c) 2018-2020, The Loki Project
// Copyright (c) 2014-2019, The Monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers
//
#pragma once

#include <cstdio>
#include <memory>
#include <optional>
#include <system_error>

#ifdef _WIN32
#include "windows.h"
#endif


/**
 * Various utilities for dealing with files/directories.
 */

namespace tools {

  //! Functional class for closing C file handles.
  struct close_file
  {
    void operator()(std::FILE* handle) const noexcept
    {
      if (handle)
      {
        std::fclose(handle);
      }
    }
  };

  //! A file restricted to process owner AND process. Deletes file on destruction.
  class private_file {
    std::unique_ptr<std::FILE, close_file> m_handle;
    std::string m_filename;

    private_file(std::FILE* handle, std::string&& filename) noexcept;
  public:

    //! `handle() == nullptr && filename.empty()`.
    private_file() noexcept;

    /*! \return File only readable by owner and only used by this process
      OR `private_file{}` on error. */
    static private_file create(std::string filename);

    private_file(private_file&&) = default;
    private_file& operator=(private_file&&) = default;

    //! Deletes `filename()` and closes `handle()`.
    ~private_file() noexcept;

    std::FILE* handle() const noexcept { return m_handle.get(); }
    const std::string& filename() const noexcept { return m_filename; }
  };

  class file_locker
  {
  public:
    file_locker(const std::string &filename);
    ~file_locker();
    bool locked() const;
  private:
#ifdef WIN32
    HANDLE m_fd;
#else
    int m_fd;
#endif
  };

  /*! \brief Returns the default data directory.
   *
   * \details Windows < Vista: C:\\Documents and Settings\\Username\\Application Data\\CRYPTONOTE_NAME
   *
   * Windows >= Vista: C:\\Users\\Username\\AppData\\Roaming\\CRYPTONOTE_NAME
   *
   * Mac: ~/Library/Application Support/CRYPTONOTE_NAME
   *
   * Unix: ~/.CRYPTONOTE_NAME
   */
  std::string get_default_data_dir();

#ifdef WIN32
  /**
   * @brief 
   *
   * @param nfolder
   * @param iscreate
   *
   * @return 
   */
  std::string get_special_folder_path(int nfolder, bool iscreate);
#endif

  /*! \brief creates directories for a path
   *
   *  wrapper around boost::filesyste::create_directories.  
   *  (ensure-directory-exists): greenspun's tenth rule in action!
   */
  bool create_directories_if_necessary(const std::string& path);
  /*! \brief std::rename wrapper for nix and something strange for windows.
   */
  std::error_code replace_file(const std::string& old_name, const std::string& new_name);

  void set_strict_default_file_permissions(bool strict);

  std::optional<bool> is_hdd(const char *path);

  void closefrom(int fd);

}
