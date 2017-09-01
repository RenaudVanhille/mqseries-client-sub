namespace renaud
{
	template <class T>
	CSingleton<T>::CSingleton()
	{

	}

	template <class T>
	CSingleton<T>::~CSingleton()
	{

	}

	template <class T>
	T& CSingleton<T>::GetInstance()
	{
		if (!ms_ptInstance)
			ms_ptInstance = new T;

		return *ms_ptInstance;
	}

	template <class T>
	void CSingleton<T>::Destroy()
	{
		if (ms_ptInstance)
		{
			delete ms_ptInstance;
			ms_ptInstance = 0;
		}
	}
}
