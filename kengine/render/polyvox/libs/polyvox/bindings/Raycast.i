%module Raycast
%{
#include "Raycast.h"

#ifdef SWIGPYTHON

template<typename VolumeType>
class PyCallback
{
private:
	PyObject *func;
	PyCallback& operator=(const PyCallback&); // Not allowed
public:
	PyCallback(const PyCallback& o) : func(o.func)
	{
		Py_XINCREF(func);
	}
	PyCallback(PyObject *func) : func(func)
	{
		Py_XINCREF(this->func);
		assert(PyCallable_Check(this->func));
	}
	~PyCallback()
	{
		Py_XDECREF(func);
	}
	bool operator()(const typename VolumeType::Sampler& sampler)
	{
		if (!func || Py_None == func || !PyCallable_Check(func))
		{
			return false; //Make this raise a Python exception
		}
		PyObject *args = Py_BuildValue("(l)", sampler.getVoxel()); //TODO pass the sampler object itself in
		PyObject *result = PyObject_Call(func,args,0);
		Py_DECREF(args);
		Py_XDECREF(result);
		return (PyInt_AsLong(result) == 0) ? false : true;
	}
};

template<typename VolumeType, typename Callback>
PolyVox::RaycastResult raycastWithEndpointsPython(VolumeType* volData, const PolyVox::Vector3DFloat& v3dStart, const PolyVox::Vector3DFloat& v3dEnd, PyObject *callback)
{
	PyCallback<VolumeType> newCallback(callback);
	return PolyVox::raycastWithEndpoints(volData, v3dStart, v3dEnd, newCallback);
}

#endif

%}

%include "Raycast.h"

#ifdef SWIGPYTHON

template<typename VolumeType, typename Callback>
PolyVox::RaycastResult raycastWithEndpointsPython(VolumeType* volData, const PolyVox::Vector3DFloat& v3dStart, const PolyVox::Vector3DFloat& v3dEnd, PyObject *callback);

%template(raycastWithEndpointsSimpleVolumeuint8) raycastWithEndpointsPython<PolyVox::SimpleVolume<uint8_t>, PyCallback<PolyVox::SimpleVolume<uint8_t> > >;

#endif
