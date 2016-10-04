#include <Python.h>

#include <dublintraceroute/dublin_traceroute.h>

std::shared_ptr<DublinTraceroute> dublintraceroute;


static PyObject* DublinTraceroute_init(PyObject *self, PyObject *args,
		PyObject *kwargs)
{
	unsigned short sport = DublinTraceroute::default_srcport;
	unsigned short dport = DublinTraceroute::default_dstport;
	char *target;
	unsigned short npaths = DublinTraceroute::default_npaths;
	unsigned short min_ttl = DublinTraceroute::default_min_ttl;
	unsigned short max_ttl = DublinTraceroute::default_max_ttl;
	unsigned int delay = DublinTraceroute::default_delay;
	bool broken_nat = DublinTraceroute::default_broken_nat;
	static const char *kwlist[] = { "self", "target", "sport", "dport",
		"npaths", "min_ttl", "max_ttl", "delay", "broken_nat", NULL };
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Os|HHHHHHH",
				(char **)&kwlist, &self, &target, &sport,
				&dport, &npaths, &min_ttl, &max_ttl, &delay,
				&broken_nat)) {
		PyErr_BadArgument();
		return NULL;
	}

	dublintraceroute = std::make_shared<DublinTraceroute>(
		DublinTraceroute(target, sport, dport, npaths,
			min_ttl, max_ttl));

	// Set the instance attributes from the constructor parameters
	PyObject	*py_sport = PyString_FromString("sport"),
			*py_dport = PyString_FromString("dport"),
			*py_target = PyString_FromString("target"),
			*py_npaths = PyString_FromString("npaths"),
			*py_min_ttl = PyString_FromString("min_ttl"),
			*py_max_ttl = PyString_FromString("max_ttl"),
			*py_delay = PyString_FromString("delay"),
			*py_broken_nat = PyString_FromString("broken_nat");

	Py_INCREF(py_sport);
	Py_INCREF(py_dport);
	Py_INCREF(py_target);
	Py_INCREF(py_npaths);
	Py_INCREF(py_min_ttl);
	Py_INCREF(py_max_ttl);

	PyObject_SetAttr(self, py_sport, Py_BuildValue("i", sport));
	PyObject_SetAttr(self, py_dport, Py_BuildValue("i", dport));
	PyObject_SetAttr(self, py_target, Py_BuildValue("s", target));
	PyObject_SetAttr(self, py_npaths, Py_BuildValue("i", npaths));
	PyObject_SetAttr(self, py_min_ttl, Py_BuildValue("i", min_ttl));
	PyObject_SetAttr(self, py_max_ttl, Py_BuildValue("i", max_ttl));
	PyObject_SetAttr(self, py_delay, Py_BuildValue("i", delay));
	PyObject_SetAttr(self, py_broken_nat, PyBool_FromLong(broken_nat));

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject* DublinTraceroute_traceroute(PyObject *self, PyObject *args)
{
	std::shared_ptr<TracerouteResults> results;
	try {
		results = std::make_shared<TracerouteResults>(dublintraceroute->traceroute());
	} catch (DublinTracerouteException &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return NULL;
	}

	PyObject *py_results = PyString_FromString(results->to_json().c_str());

	return py_results;
}


static PyMethodDef DublinTracerouteMethods[] =
{
	{"__init__", (PyCFunction)DublinTraceroute_init,
		METH_VARARGS | METH_KEYWORDS,
		"Initialize DublinTraceroute\n"
		"\n"
		"Arguments:\n"
		"    target     : the target IP address\n"
		"    srcport    : the source UDP port (optional, default=12345)\n"
		"    dstport    : the destination UDP port to start with (optional, default=33434)\n"
		"    npaths     : the number of paths to cover (optional, default=20)\n"
		"    min_ttl    : the maximum Time-To-Live (optional, default=1)\n"
		"    max_ttl    : the maximum Time-To-Live (optional, default=30)\n"
		"    delay      : the inter-packet delay in milliseconds (optional, default=10ms)"
		"    broken_nat : the network has a broken NAT configuration (e.g. no payload fixup). Try this if you see less hops than expected"
		"\n"
		"Return value:\n"
		"    a JSON object containing the traceroute data. See example below\n"
		"\n"
		"Example:\n"
		">>> dub = DublinTraceroute(\"8.8.8.8\", 12345, 33434)\n"
		">>> print dub\n"
		"<DublinTraceroute (target='8.8.8.8', sport=12345, dport=33434, npaths=20, min_ttl=1, max_ttl=30)>\n"
	},
	// TODO The docstring here is not inherited by the pure-Python class,
	//      but it's rewritten. Find a clean alternative to this.
	{"traceroute", DublinTraceroute_traceroute, METH_VARARGS,
		"Run the traceroute\n"
		"\n"
		"Example:\n"
		">>> dub = DublinTraceroute(\"8.8.8.8\", 12345, 33434)\n"
		">>> results = dub.traceroute()\n"
		">>> print results\n"
		"{u'flows': {u'33434': [{u'is_last': False,\n"
		"    u'nat_id': 0,\n"
		"    u'received': {u'icmp': {u'code': 11,\n"
		"      u'description': u'TTL expired in transit',\n"
		"      u'type': 0},\n"
		"     u'ip': {u'dst': u'192.168.0.1', u'src': u'192.168.0.254', u'ttl': 64}},\n"
		"    u'rtt_usec': 2801,\n"
		"    u'sent': {u'ip': {u'dst': u'8.8.8.8', u'src': u'192.168.0.1', u'ttl': 1},\n"
		"     u'udp': {u'dport': 33434, u'sport': 12345}}},\n"
		"...\n"
		">>>\n"
	},
	{NULL},
};


static PyMethodDef ModuleMethods[] = { {NULL} };

PyMODINIT_FUNC
init_dublintraceroute(void)
{
    PyMethodDef *def;

    /* create the _dublintraceroute module */
    PyObject *module = Py_InitModule("_dublintraceroute", ModuleMethods);
    PyObject *moduleDict = PyModule_GetDict(module);

    /* export the default attributes */
    PyObject_SetAttrString(module, "DEFAULT_SPORT",
        PyInt_FromLong(DublinTraceroute::default_srcport));
    PyObject_SetAttrString(module, "DEFAULT_DPORT",
        PyInt_FromLong(DublinTraceroute::default_dstport));
    PyObject_SetAttrString(module, "DEFAULT_NPATHS",
        PyInt_FromLong(DublinTraceroute::default_npaths));
    PyObject_SetAttrString(module, "DEFAULT_MIN_TTL",
        PyInt_FromLong(DublinTraceroute::default_min_ttl));
    PyObject_SetAttrString(module, "DEFAULT_MAX_TTL",
        PyInt_FromLong(DublinTraceroute::default_max_ttl));
    PyObject_SetAttrString(module, "DEFAULT_DELAY",
        PyInt_FromLong(DublinTraceroute::default_delay));
    PyObject_SetAttrString(module, "DEFAULT_BROKEN_NAT",
        PyInt_FromLong(DublinTraceroute::default_broken_nat));

    /* create the DublinTraceroute class */
    PyObject *classDictDT = PyDict_New();
    PyObject *classNameDT = PyString_FromString("DublinTraceroute");
    PyObject *dublinTracerouteClass = PyClass_New(NULL, classDictDT, classNameDT);
    PyDict_SetItemString(moduleDict, "DublinTraceroute", dublinTracerouteClass);
    Py_DECREF(classDictDT);
    Py_DECREF(classNameDT);
    Py_DECREF(dublinTracerouteClass);

    /* add methods to class DublinTraceroute */
    for (def = DublinTracerouteMethods; def->ml_name != NULL; def++) {
	PyObject *func = PyCFunction_New(def, NULL);
	PyObject *method = PyMethod_New(func, NULL, dublinTracerouteClass);
	PyDict_SetItemString(classDictDT, def->ml_name, method);
	Py_DECREF(func);
	Py_DECREF(method);
    }
}
