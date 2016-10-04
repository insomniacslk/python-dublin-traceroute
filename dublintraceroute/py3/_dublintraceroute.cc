#include <Python.h>
#include <structmember.h>

#include <dublintraceroute/dublin_traceroute.h>

#define MODULE_NAME	"_dublintraceroute"

std::shared_ptr<DublinTraceroute> dublintraceroute;


typedef struct {
	PyObject_HEAD
	PyObject *target;
	PyObject *sport;
	PyObject *dport;
	PyObject *npaths;
	PyObject *min_ttl;
	PyObject *max_ttl;
	PyObject *delay;
	PyObject *broken_nat;
} DublinTracerouteClass;


static PyModuleDef _dublintraceroutemodule = {
	PyModuleDef_HEAD_INIT,
	"_dublintraceroute",
	"C wrapper around libdublintraceroute. Do not call directly, use "
		"the module dublintraceroute instead",
	-1,
	NULL, NULL, NULL, NULL, NULL
};

static int
DublinTraceroute_init(PyObject *self, PyObject *args,
		PyObject *kwargs) {
	char *target;
	unsigned short sport = DublinTraceroute::default_srcport;
	unsigned short dport = DublinTraceroute::default_dstport;
	unsigned short npaths = DublinTraceroute::default_npaths;
	unsigned short min_ttl = DublinTraceroute::default_min_ttl;
	unsigned short max_ttl = DublinTraceroute::default_max_ttl;
	unsigned int delay = DublinTraceroute::default_delay;
	bool broken_nat = DublinTraceroute::default_broken_nat;
	static const char *arglist[] = { "target", "sport", "dport",
		"npaths", "min_ttl", "max_ttl", "delay", "broken_nat", NULL };
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|HHHHHHH",
				(char **)&arglist, &target, &sport,
				&dport, &npaths, &min_ttl, &max_ttl, &delay,
				&broken_nat)) {
		return -1;
	}

	dublintraceroute = std::make_shared<DublinTraceroute>(
		DublinTraceroute(target, sport, dport, npaths,
			min_ttl, max_ttl, delay, broken_nat));

	// Set the instance attributes from the constructor parameters
	PyObject	*py_sport = PyUnicode_FromString("sport"),
			*py_dport = PyUnicode_FromString("dport"),
			*py_target = PyUnicode_FromString("target"),
			*py_npaths = PyUnicode_FromString("npaths"),
			*py_min_ttl = PyUnicode_FromString("min_ttl"),
			*py_max_ttl = PyUnicode_FromString("max_ttl"),
			*py_delay = PyUnicode_FromString("delay"),
			*py_broken_nat = PyUnicode_FromString("broken_nat");

	Py_INCREF(py_sport);
	Py_INCREF(py_dport);
	Py_INCREF(py_target);
	Py_INCREF(py_npaths);
	Py_INCREF(py_min_ttl);
	Py_INCREF(py_max_ttl);
	Py_INCREF(py_delay);
	Py_INCREF(py_broken_nat);

	PyObject_SetAttr(self, py_sport, Py_BuildValue("i", sport));
	PyObject_SetAttr(self, py_dport, Py_BuildValue("i", dport));
	PyObject_SetAttr(self, py_target, Py_BuildValue("s", target));
	PyObject_SetAttr(self, py_npaths, Py_BuildValue("i", npaths));
	PyObject_SetAttr(self, py_min_ttl, Py_BuildValue("i", min_ttl));
	PyObject_SetAttr(self, py_max_ttl, Py_BuildValue("i", max_ttl));
	PyObject_SetAttr(self, py_delay, Py_BuildValue("i", delay));
	PyObject_SetAttr(self, py_broken_nat, PyBool_FromLong(broken_nat));

	Py_INCREF(Py_None);
	return 0;
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

	PyObject *py_results = PyUnicode_FromString(results->to_json().c_str());

	return py_results;
}


static PyMemberDef DublinTraceroute_members[] = {
	{(char *)"target", T_OBJECT_EX, offsetof(DublinTracerouteClass, target), 0,
	 (char *)"the target host"},
	{(char *)"sport", T_OBJECT_EX, offsetof(DublinTracerouteClass, sport), 0,
	 (char *)"source port"},
	{(char *)"dport", T_OBJECT_EX, offsetof(DublinTracerouteClass, dport), 0,
	 (char *)"starting destination port"},
	{(char *)"npaths", T_OBJECT_EX, offsetof(DublinTracerouteClass, npaths), 0,
	 (char *)"number of paths to probe"},
	{(char *)"min_ttl", T_OBJECT_EX, offsetof(DublinTracerouteClass, min_ttl), 0,
	 (char *)"minimum TTL"},
	{(char *)"max_ttl", T_OBJECT_EX, offsetof(DublinTracerouteClass, max_ttl), 0,
	 (char *)"maximum TTL"},
	{(char *)"delay", T_OBJECT_EX, offsetof(DublinTracerouteClass, delay), 0,
	 (char *)"inter-packet delay"},
	{(char *)"broken_nat", T_OBJECT_EX, offsetof(DublinTracerouteClass, broken_nat), 0,
	 (char *)"broken NAT flag"},
	{NULL}  /* Sentinel */
};

static PyMethodDef DublinTraceroute_methods[] =
{
	{"__init__", (PyCFunction)DublinTraceroute_init,
		METH_VARARGS | METH_KEYWORDS,
		"Initialize DublinTraceroute\n"
		"\n"
		"Arguments:\n"
		"    target  : the target IP address\n"
		"    sport   : the source UDP port (optional, default=12345)\n"
		"    dport   : the destination UDP port to start with (optional, default=33434)\n"
		"    npaths  : the number of paths to cover (optional, default=20)\n"
		"    min_ttl : the minimum Time-To-Live (optiona, default=1)\n"
		"    max_ttl : the maximum Time-To-Live (optiona, default=30)\n"
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

static PyTypeObject DublinTracerouteType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"DublinTraceroute",        /* tp_name */
	sizeof(DublinTracerouteClass),  /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,                         /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT |
		Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"DublinTraceroute objects",/* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	DublinTraceroute_methods,  /* tp_methods */
	DublinTraceroute_members,  /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)DublinTraceroute_init,      /* tp_init */
	0,                         /* tp_alloc */
	PyType_GenericNew /* DublinTraceroute_new */,      /* tp_new */
};

PyMODINIT_FUNC
PyInit__dublintraceroute(void)
{
	PyObject* module;

	if (PyType_Ready(&DublinTracerouteType) < 0)
		return NULL;

	module = PyModule_Create(&_dublintraceroutemodule);
	if (module == NULL)
		return NULL;

	Py_INCREF(&DublinTracerouteType);
	PyModule_AddObject(module, "DublinTraceroute",
		(PyObject *)&DublinTracerouteType);

	/* export the default attributes */
	PyObject_SetAttrString(module, "DEFAULT_SPORT",
			PyLong_FromLong(DublinTraceroute::default_srcport));
	PyObject_SetAttrString(module, "DEFAULT_DPORT",
			PyLong_FromLong(DublinTraceroute::default_dstport));
	PyObject_SetAttrString(module, "DEFAULT_NPATHS",
			PyLong_FromLong(DublinTraceroute::default_npaths));
	PyObject_SetAttrString(module, "DEFAULT_MAX_TTL",
			PyLong_FromLong(DublinTraceroute::default_max_ttl));
	PyObject_SetAttrString(module, "DEFAULT_MIN_TTL",
			PyLong_FromLong(DublinTraceroute::default_min_ttl));
	PyObject_SetAttrString(module, "DEFAULT_DELAY",
			PyLong_FromLong(DublinTraceroute::default_delay));
	PyObject_SetAttrString(module, "DEFAULT_BROKEN_NAT",
			PyLong_FromLong(DublinTraceroute::default_broken_nat));
    return module;
}
