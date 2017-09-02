#ifndef C_SUB_H
#define C_SUB_H

#include "../CSingleton.h"

#include <cmqc.h>

#include <string>
#include <vector>

namespace renaud
{
	class ISubListener;
	class CSub : public CSingleton<CSub>
	{
	SINGLETON_DECLARE(CSub)

	public:
		CSub();
		int Init(const std::string& rstrQueueManager, const std::string& rstrTopic);
		int Close();

		void AddListener(ISubListener* pListener);
		void RemoveListener(ISubListener* pListener);
		
	private:
		static void* Loop(void* ptr);

		typedef std::vector<ISubListener*> TVecListeners;
		TVecListeners m_vecListeners;

		void NotifyListeners(const std::string& rstrMessages);

		MQHCONN m_Hcon;
		MQHOBJ m_Hsub;
		MQHOBJ m_Hobj;

		pthread_t m_thread;
		bool m_bStop;
	};
}

#define SUB CSub::GetInstance()

#endif // C_SUB_H