#include "stdafx.h"
#include "zTDBApi.h"


BOOST_PYTHON_MODULE(zTDB) {
	boost::python::class_<zTDBApi, boost::noncopyable>("TDBApi")
		.def("connect", &zTDBApi::connect)
		.def("getKLines", &zTDBApi::getKLines)
		;
};

