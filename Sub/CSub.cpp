#include "CSub.h"

#include "ISubListener.h"

#include <algorithm>

namespace renaud
{
	CSub::CSub()
	{

	}

	int CSub::Init(const std::string& rstrQueueManager, const std::string& rstrTopic)
	{
		MQLONG   CompCode;
		MQLONG   CReason;

		MQHCONN  Hcon;

		MQCONN(const_cast<char*>(rstrQueueManager.c_str()),						/* queue manager                  */
			&Hcon,																/* connection handle              */
			&CompCode,															/* completion code                */
			&CReason);															/* reason code                    */

																				/* report reason and stop if it failed     */
		if (CompCode == MQCC_FAILED)
			return ((int)CReason);

		MQSD     sd = { MQSD_DEFAULT };
		sd.Options = MQSO_CREATE
			| MQSO_NON_DURABLE
			| MQSO_FAIL_IF_QUIESCING
			| MQSO_MANAGED;
		sd.ObjectString.VSPtr = rstrTopic.c_str();
		sd.ObjectString.VSLength = (MQLONG)rstrTopic.size();

		MQHOBJ   Hobj = MQHO_NONE;
		MQHOBJ   Hsub = MQHO_NONE;
		MQLONG   S_CompCode;
		MQLONG   Reason;
		MQSUB(Hcon,                       /* connection handle            */
			&sd,                        /* object descriptor for queue  */
			&Hobj,                      /* object handle (output)       */
			&Hsub,                      /* object handle (output)       */
			&S_CompCode,                /* completion code              */
			&Reason);                   /* reason code                  */

		/* report reason, if any; stop if failed      */
		if (Reason != MQRC_NONE)
			return Reason;

		if (S_CompCode == MQCC_FAILED)
			return -1;

		// !! TODO : Lancer un thread ici pour ne pas bloquer le init et recevoir les events
		// à remonter via NotifyListeners(message)
		// et virer l'attente de 30 secs

		MQGMO gmo = { MQGMO_DEFAULT };   /* get message options           */
		gmo.Options = MQGMO_WAIT         /* wait for new messages       */
			| MQGMO_NO_SYNCPOINT /* no transaction              */
			| MQGMO_CONVERT;     /* convert if necessary        */

		gmo.WaitInterval = 30000;        /* 30 second limit for waiting   */

		//MQGET...

		return -1;
	}

	int CSub::Close()
	{
		return -1;
	}

	void CSub::AddListener(ISubListener* pListener)
	{
		m_vecListeners.push_back(pListener);
	}

	void CSub::RemoveListener(ISubListener* pListener)
	{
		m_vecListeners.erase(std::remove(m_vecListeners.begin(), m_vecListeners.end(), pListener), m_vecListeners.end());
	}

	void CSub::NotifyListeners(const std::string& rstrMessages)
	{
		for (TVecListeners::iterator it = m_vecListeners.begin(); it != m_vecListeners.end(); ++it)
			(*it)->OnMessage("message", rstrMessages);
	}
}