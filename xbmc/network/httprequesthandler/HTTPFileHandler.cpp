#if defined(HAS_VIDONME)
#include "HTTPFileHandler.h"
#include "network/WebServer.h"
#include "URL.h"
#include "filesystem/ImageFile.h"

using namespace std;

bool CHTTPFileHandler::CheckHTTPRequest(const HTTPRequest &request)
{
  return (request.url.find("/file/") == 0);
}

int CHTTPFileHandler::HandleHTTPRequest(const HTTPRequest &request)
{
	if (request.url.size() > 6)
	{
		m_path = request.url.substr(6);
		CURL::Decode(m_path);

		if (XFILE::CFile::Exists(m_path) )
    {
      m_responseCode = MHD_HTTP_OK;
      m_responseType = HTTPFileDownload;
    }
    else
    {
      m_responseCode = MHD_HTTP_NOT_FOUND;
      m_responseType = HTTPError;
    }
  }
  else
  {
    m_responseCode = MHD_HTTP_BAD_REQUEST;
    m_responseType = HTTPError;
  }

  return MHD_YES;
}
#endif