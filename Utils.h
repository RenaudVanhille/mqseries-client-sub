#ifndef UTILS_H
#define UTILS_H

#include <node/node.h>

#include <vector>
#include <string>

using v8::Local;
using v8::Value;
using v8::FunctionCallbackInfo;

namespace renaud
{
	bool checkInputs(const FunctionCallbackInfo<Value>& args, const std::vector<std::string>& vecFormat)
	{
		if (args.Length() != vecFormat.size())
			return false;

		bool bArgsOk = true;
		unsigned int i = 0;
		for (i = 0; i < vecFormat.size(); ++i)
		{
			const std::string& format = vecFormat[i];
			const Local<Value> arg = args[i];

			bArgsOk &= bArgsOk && (format == "bool" || !arg->IsBoolean());
			bArgsOk &= bArgsOk && (format == "func" || !arg->IsFunction());
			bArgsOk &= bArgsOk && (format == "string" || !arg->IsString());
			bArgsOk &= bArgsOk && (format == "array" || !arg->IsArray());
			bArgsOk &= bArgsOk && (format == "number" || !arg->IsNumber());
			bArgsOk &= bArgsOk && (format == "date" || !arg->IsDate());
			bArgsOk &= bArgsOk && (format == "promise" || !arg->IsPromise());
		}

		if (!bArgsOk) {
			return false;
		}

		return true;
	}

	typedef std::vector<std::string> TArgsDef;
}

#endif // UTILS_H