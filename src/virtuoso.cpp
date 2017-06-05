#include "pyodbc.h"
#include "virtuoso.h"
#include "pyodbccompat.h"
#include <wctype.h>
#include <wchar.h>

bool
isVirtuoso(HDBC hdbc)
{
    char buf[0x1000];
    SQLSMALLINT len;
    SQLRETURN ret;

    ret = SQLGetInfo(hdbc, (SQLUSMALLINT)SQL_DBMS_NAME, buf, sizeof(buf), &len);
    if (!SQL_SUCCEEDED(ret))
	return false;
    if (!strncasecmp(buf, "OpenLink Virtuoso", sizeof(buf))) {
	return true;
    }

    return false;
}

bool
isSPASQL(PyObject *pSql)
{
  #if PY_MAJOR_VERSION < 3
    if (PyBytes_Check(pSql))
    {
        char *query = PyBytes_AS_STRING(pSql);
        if (!query)
            return false;
        while (*query && isspace(*query))
            query++;
        return !strncasecmp(query, "SPARQL", 6);
    }
  #endif
    if (PyUnicode_Check(pSql)) {
        // take a reasonable prefix size
        wchar_t prefix[32];
        wchar_t* query = prefix;
      #if PY_MAJOR_VERSION < 3
        Py_ssize_t size = PyUnicode_AsWideChar((PyUnicodeObject*)pSql, query, 32);
      #else
        Py_ssize_t size = PyUnicode_AsWideChar(pSql, query, 32);
      #endif

        while (*query && iswspace(*query)) {
            query++;
            size--;
        }
        if (size < 6)
            return false;
        return !wcsncasecmp(query, L"SPARQL", 6);
    }
    return false;
}
