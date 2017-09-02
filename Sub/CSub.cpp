#include "CSub.h"

#include "ISubListener.h"

#include <algorithm>
#include <string.h>
#include <pthread.h>

namespace renaud
{
	SINGLETON_IMPL(CSub)

	CSub::CSub()
		: m_Hcon(0),
		m_Hsub(MQHO_NONE),
		m_Hobj(MQHO_NONE)
	{

	}

	void* CSub::Loop(void* ptr)
	{
		CSub* pSub = (CSub*)ptr;

		MQGMO gmo = { MQGMO_DEFAULT };   /* get message options           */
		gmo.Options = MQGMO_WAIT         /* wait for new messages       */
			| MQGMO_NO_SYNCPOINT /* no transaction              */
			| MQGMO_CONVERT;     /* convert if necessary        */

		gmo.WaitInterval = 1000;        /* 1 second limit for waiting   */

		MQBYTE   buffer[101];            /* message buffer                */
		MQMD     md = { MQMD_DEFAULT };    /* Message Descriptor            */
		
		MQLONG   CompCode = 0;
		MQLONG   Reason;
		while (!pSub->m_bStop && CompCode != MQCC_FAILED)
		{
			MQLONG buflen = sizeof(buffer) - 1; /* buffer size available for GET   */

												/****************************************************************/
												/* The following two statements are not required if the MQGMO   */
												/* version is set to MQGMO_VERSION_2 and and gmo.MatchOptions   */
												/* is set to MQGMO_NONE                                         */
												/****************************************************************/
												/*                                                              */
												/*   In order to read the messages in sequence, MsgId and       */
												/*   CorrelID must have the default value.  MQGET sets them     */
												/*   to the values in for message it returns, so re-initialise  */
												/*   them before every call                                     */
												/*                                                              */
												/****************************************************************/
			memcpy(md.MsgId, MQMI_NONE, sizeof(md.MsgId));
			memcpy(md.CorrelId, MQCI_NONE, sizeof(md.CorrelId));

			/****************************************************************/
			/*                                                              */
			/*   MQGET sets Encoding and CodedCharSetId to the values in    */
			/*   the message returned, so these fields should be reset to   */
			/*   the default values before every call, as MQGMO_CONVERT is  */
			/*   specified.                                                 */
			/*                                                              */
			/****************************************************************/

			md.Encoding = MQENC_NATIVE;
			md.CodedCharSetId = MQCCSI_Q_MGR;

			MQLONG   messlen;                /* message length received       */

			MQGET(pSub->m_Hcon,                /* connection handle                 */
				pSub->m_Hobj,                /* object handle                     */
				&md,                 /* message descriptor                */
				&gmo,                /* get message options               */
				buflen,              /* buffer length                     */
				buffer,              /* message buffer                    */
				&messlen,            /* message length                    */
				&CompCode,           /* completion code                   */
				&Reason);            /* reason code                       */

			/* report reason, if any     */
			if (Reason != MQRC_NONE)
			{
				if (Reason == MQRC_NO_MSG_AVAILABLE)
				{                         /* special report for normal end    */
					//printf("no more messages\n");
				}
				else                      /* general report for other reasons */
				{

					/*   treat truncated message as a failure for this sample   */
					if (Reason == MQRC_TRUNCATED_MSG_FAILED)
					{
						CompCode = MQCC_FAILED;
					}
				}
			}

			if (CompCode != MQCC_FAILED)
			{
				buffer[messlen] = '\0';            /* add terminator          */
				pSub->NotifyListeners(reinterpret_cast<char*>(buffer));
			}

			if (Reason == MQRC_NO_MSG_AVAILABLE) // réinit (pas de message ne veut pas dire erreur)
				CompCode = 0;
		}

		return 0;
	}

	int CSub::Init(const std::string& rstrQueueManager, const std::string& rstrTopic)
	{
		MQLONG   CompCode;
		MQLONG   CReason;

		MQCONN(const_cast<char*>(rstrQueueManager.c_str()),						/* queue manager                  */
			&m_Hcon,																/* connection handle              */
			&CompCode,															/* completion code                */
			&CReason);															/* reason code                    */
																				/* report reason and stop if it failed     */
		if (CompCode == MQCC_FAILED)
			return CReason;

		MQSD sd = { MQSD_DEFAULT };
		sd.Options = MQSO_CREATE
			| MQSO_NON_DURABLE
			| MQSO_FAIL_IF_QUIESCING
			| MQSO_MANAGED;
		sd.ObjectString.VSPtr = (void*)rstrTopic.c_str();
		sd.ObjectString.VSLength = (MQLONG)rstrTopic.size();

		MQLONG   S_CompCode;
		MQLONG   Reason;
		MQSUB(m_Hcon,                       /* connection handle            */
			&sd,                        /* object descriptor for queue  */
			&m_Hobj,                      /* object handle (output)       */
			&m_Hsub,                      /* object handle (output)       */
			&S_CompCode,                /* completion code              */
			&Reason);                   /* reason code                  */

		/* report reason, if any; stop if failed      */
		if (Reason != MQRC_NONE)
			return Reason;

		if (S_CompCode == MQCC_FAILED)
			return -1;

		m_bStop = false;
		if (pthread_create(&m_thread, NULL, Loop, this)) 
			return -1;

		return 0;
	}

	int CSub::Close()
	{
		if (m_Hcon == MQHO_NONE)
			return 0;

		// Il faut tuer le thread d'abord
		m_bStop = true;
		if (pthread_join(m_thread, NULL)) {
			return -2;
		}

		MQLONG C_options = MQCO_NONE;        /* no close options             */
		MQLONG CompCode;
		MQLONG Reason;

		MQCLOSE(m_Hcon,                    /* connection handle           */
			&m_Hsub,                   /* object handle               */
			C_options,
			&CompCode,               /* completion code             */
			&Reason);                /* reason code                 */

		/* report reason, if any     */
		if (Reason != MQRC_NONE)
			return Reason;
		return 0;
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