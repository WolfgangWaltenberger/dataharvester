#ifndef SystemWriter_H
#define SystemWriter_H

#include <string>

namespace dataharvester {
class SystemWriter {
public:
  /** \class SystemWriter
   *  A ''high-level'' dataharvesting class, based on the standard Writer,
   *  that collects useful system information.
   *  Currently only the 'currentTime' is POSIX compliant,
   *  the rest is linux only.
   */
  SystemWriter ( const std::string & filename = "system.txt",
                 bool comments=false );

  static const int CpuTime=1, Memory=2, MachineName=4, KernelName=8, CurrentTime=16,
                   LsbRelease=32;
  static const int Everything = CpuTime | Memory | MachineName | KernelName |
    CurrentTime | LsbRelease;

  static std::string currentTime(); // "Wed Feb 14 11:08:42 CET 2007"
  static std::string machineName(); // "centurion.shacknet.nu"
  static std::string kernelName(); //  "2.6.17-2-686"
  static std::string lsbRelease(); //  "Debian GNU/Linux testing (jessie)"
  static double cpuMHz(); // "3074.274"
  static int memory(); // "2049236" (2GB)

  /**
   *  Save what information into what tuple?
   */
  void save ( int what = Everything, const std::string & tuplename="System" );

  /** Shortcut for saving "CurrentTime" into "System", under a "Time" column.
   */
  void timestamp( const std::string & ntuplename="System", 
                  const std::string & columnname="Time" );
private:
  std::string theFilename;
  bool theComments;
};
}

#endif
