#if defined(HAS_VIDONME)

#pragma once
#include "boost/shared_ptr.hpp"
#include "guilib/GUIWindowManager.h"
#include "dialogs/GUIDialogBusy.h"
#include "threads/SingleLock.h"
#include "utils/log.h"
#include "utils/Job.h"
#include "utils/JobManager.h"
#include "Application.h"

class MethodBase
{
public:
  virtual ~MethodBase(){}
  virtual bool MethodCall() = 0;
  virtual float GetProgress(/*OUT*/ std::string& text)
  { 
    text = "";
    return 0.0f; 
  }
  virtual bool Cancel() { return true; }
};

class MethodPtr
{
public:
  MethodPtr(MethodBase* const method)
    : m_method(method)
  {
  }
  ~MethodPtr()
  {
    delete m_method;
  }
public:
  bool MethodCall()
  {
    return m_method->MethodCall();
  }
  float GetProgress(/*OUT*/ std::string& text)
  { 
    return m_method->GetProgress(text);
  }
  bool Cancel() 
  { 
    return m_method->Cancel();
  }
private:
  MethodBase* m_method;
};


class MethodHolder
{
private:
    struct CResult
    {
      CResult()
        : m_event(true)
        , m_result(false)
      {

      }
      CEvent m_event;
      bool   m_result;
    };

    struct MethodJob: CJob
    {
        MethodJob(const boost::shared_ptr<MethodPtr>& method
            , const boost::shared_ptr<CResult>& result)
            : m_method(method)
            , m_result(result)
        {
          // NULL
        }
        virtual bool DoWork()
        {
            m_result->m_result = m_method->MethodCall();
            m_result->m_event.Set();
            return m_result->m_result;
        }
        boost::shared_ptr<MethodPtr>   m_method;
        boost::shared_ptr<CResult>  m_result;
    };

public:
    MethodHolder(const boost::shared_ptr<MethodPtr>& method)
      : m_result(new CResult())
    {
        m_id = CJobManager::GetInstance().AddJob(new MethodJob(method, m_result)
            , NULL
            , CJob::PRIORITY_HIGH);
    }
    ~MethodHolder()
    {
        CJobManager::GetInstance().CancelJob(m_id);
    }

    bool Wait(unsigned int timeout)
    {
        return m_result->m_event.WaitMSec(timeout);
    }

    bool GetResults()
    {
        /* if it was not finished or failed, return failure */
        if(!m_result->m_event.WaitMSec(0) || !m_result->m_result)
        {
            return false;
        }

        return true;
    }
    unsigned int  m_id;
    boost::shared_ptr<CResult>   m_result;
};

bool SynchroMethodCall( const boost::shared_ptr<MethodPtr> method);

#endif //if defined(HAS_VIDONME)
