#ifndef I_SUB_LISTENER_H
#define I_SUB_ISTENER_H

#include <string>

namespace renaud
{
	class ISubListener
	{
	friend class CSub;

	protected:
		virtual void OnMessage(const std::string& rstrEvent, const std::string& rstrMessage) = 0;
	};
}

#endif // I_SUB_LISTENER_H
