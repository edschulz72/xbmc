#pragma once

#if defined(HAS_VIDONME)

#include "IHTTPRequestHandler.h"

#include "utils/StdString.h"

class CHTTPFileHandler : public IHTTPRequestHandler
{
public:
  CHTTPFileHandler() { };

  virtual IHTTPRequestHandler* GetInstance() { return new CHTTPFileHandler(); }
  virtual bool CheckHTTPRequest(const HTTPRequest &request);
  virtual int HandleHTTPRequest(const HTTPRequest &request);

  virtual std::string GetHTTPResponseFile() const { return m_path; }

  virtual int GetPriority() const { return 2; }

private:
  CStdString m_path;
};
#endif