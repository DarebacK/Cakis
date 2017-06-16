#pragma once

namespace DarEngine
{
	template<class T>
	void SafeDelete(T* toBeDeletedObject)
	{
		if(toBeDeletedObject)
		{
			delete toBeDeletedObject;
			toBeDeletedObject = nullptr;
		}
	}

	template<class T>
	void SafeDeleteArray(T* toBeDeletedArray)
	{
		if(toBeDeletedArray)
		{
			delete[] toBeDeletedArray;
			toBeDeletedArray = nullptr;
		}
	}

	template<class T>
	void SafeRelease(T* toBeReleasedObject)
	{
		if(toBeReleasedObject)
		{
			toBeReleasedObject->Release();
			toBeReleasedObject = nullptr;
		}
	}
}