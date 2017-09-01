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
		typedef std::vector<ISubListener*> TVecListeners;
		TVecListeners m_vecListeners;

		void NotifyListeners(const std::string& rstrMessages);
	};
}

#define SUB CSub::GetInstance()

#endif // C_SUB_H