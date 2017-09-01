#include "CPromiseData.h"

namespace renaud
{
	CPromiseData::CPromiseData(Isolate* pIsolate)
		: m_iCode(0),
		m_pArgs(0)
	{
		m_pResolver.Reset(pIsolate, Promise::Resolver::New(pIsolate));
	}

	CPromiseData::~CPromiseData()
	{
		if (m_pArgs)
			delete m_pArgs;
	}

	const std::string& CPromiseData::GetResult() const
	{
		return m_strResult;
	}

	bool CPromiseData::IsOnError() const
	{
		return m_iCode != 0;
	}

	void CPromiseData::Set(const std::string& rstrResult, int iCode)
	{
		m_strResult = rstrResult;
		m_iCode = iCode;
	}

	bool CPromiseData::IsEmpty()
	{
		return m_strResult.size() == 0;
	}

	void CPromiseData::SetArgs(void* pArgs)
	{
		m_pArgs = pArgs;
	}

	const void* CPromiseData::GetArgs() const
	{
		return m_pArgs;
	}

	const Persistent<Promise::Resolver>& CPromiseData::GetResolver() const
	{
		return m_pResolver;
	}
}