#ifndef C_PROMISE_DATA_H
#define C_PROMISE_DATA_H

#include <string>

#include <node/node.h>

using v8::Isolate;
using v8::Promise;
using v8::Persistent;

namespace renaud
{
	class CPromiseData
	{
	public:
		CPromiseData(Isolate* isolate);
		~CPromiseData();

		const std::string& GetResult() const;
		bool IsOnError() const;

		void Set(const std::string& rstrResult, int iCode = 0);
		bool IsEmpty();

		const void* GetArgs() const;
		void SetArgs(void* pArgs);

		const Persistent<Promise::Resolver>& GetResolver() const;

	private:
		Persistent<Promise::Resolver> m_pResolver;

		int m_iCode;
		void* m_pArgs;
		std::string m_strResult;
	};
}

#endif // C_PROMISE_DATA_H